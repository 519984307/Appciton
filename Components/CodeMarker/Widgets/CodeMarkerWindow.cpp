/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/6
 **/


#include "CodeMarkerWindow.h"
#include <QScrollBar>
#include "ConfigManager.h"
#include <QGridLayout>
#include <QScrollArea>
#include <LanguageManager.h>
#include "FontManager.h"
#include "WindowManager.h"
#include <QTimer>
#include "EventStorageManager.h"

CodeMarkerWindow *CodeMarkerWindow::_selfObj = NULL;
class CodeMarkerWindowPrivate
{
public:
    CodeMarkerWindowPrivate()
        : _isPress(false)
        , _isChosen(false)
        , _codeMarkerNum(0)
        , _scrollArea(NULL)
        , _timer(NULL)
        , _closeTimer(NULL)
    {
        for (int i = 0 ; i < 30 ; i++)
        {
            _codeMarkerButton[i] = NULL;
        }
    }

    QScrollArea *_scrollArea;
    QList<Button *> _button;
    Button *_codeMarkerButton[30];
    QStringList _origCodeMarker;      // 未经过翻译的code emarker。
    QStringList _localeCodeMarker;    // 翻译后的code emarker。
    QTimer *_timer;
    QTimer *_closeTimer;
    bool _isPress;                    // codemark按钮被按下
    bool _isChosen;                   // codemark按钮已按下选中
    int _codeMarkerNum;
};

CodeMarkerWindow::CodeMarkerWindow() : Window()
        , d_ptr(new CodeMarkerWindowPrivate())
{
    QString codemarkerStr;
    int num = 0;
    currentConfig.getNumAttr("Local|Language", "CurrentOption", num);
    QString markerStr = "CodeMarker|SelectMarker|Language";
    markerStr += QString::number(num, 10);
    currentConfig.getStrValue(markerStr, codemarkerStr);
    d_ptr->_origCodeMarker = codemarkerStr.split(',');
    for (int i = 0; i < d_ptr->_origCodeMarker.size(); i++)
    {
        d_ptr->_localeCodeMarker.append(trs(d_ptr->_origCodeMarker[i]));
    }

    setPress(false);
    d_ptr->_isChosen = false;


    int fontSize = fontManager.getFontSize(1);
    int barWidth = 20;
    int borderWidth = 4;

    setWindowTitle(trs("CodeMarker"));
    setFixedWidth(windowManager.getPopMenuWidth());
    QWidget *widget = new QWidget();
    widget->setFixedWidth(windowManager.getPopMenuWidth() - borderWidth * 2 - barWidth);
    setFixedWidth(windowManager.getPopMenuWidth());
    // srocll
    d_ptr->_scrollArea = new QScrollArea();
    d_ptr->_scrollArea->setFocusPolicy(Qt::NoFocus);
    d_ptr->_scrollArea->setFrameStyle(QFrame::NoFrame);
    d_ptr->_scrollArea->setAlignment(Qt::AlignTop);
    d_ptr->_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->_scrollArea->verticalScrollBar()->setFixedWidth(barWidth);

    QGridLayout *layoutG = new QGridLayout();
    layoutG->setContentsMargins(4, 2, 4, 2);
    layoutG->setSpacing(0);

    CodeMarkerWindowButton *codeMarkerButton[d_ptr->_localeCodeMarker.size()];
    for (int i = 0 ; i < d_ptr->_localeCodeMarker.size() ; i++)
    {
        codeMarkerButton[i] = new CodeMarkerWindowButton(i);
        codeMarkerButton[i]->setText(d_ptr->_localeCodeMarker[i]);
        codeMarkerButton[i]->setButtonStyle(Button::ButtonTextOnly);
        codeMarkerButton[i]->setBorderWidth(0);
        codeMarkerButton[i]->setFont(fontManager.textFont(fontSize));
        connect(codeMarkerButton[i], SIGNAL(released()), this, SLOT(_btnReleased()));
        codeMarkerButton[i]->setProperty("Item" , qVariantFromValue(i));
        codeMarkerButton[i]->setFixedWidth(200);
        layoutG->addWidget(codeMarkerButton[i], i / 4, i % 4, 1, 1);
    }

    setWindowLayout(layoutG);

    d_ptr->_timer = new QTimer();
    d_ptr->_timer->setInterval(5000);
    connect(d_ptr->_timer, SIGNAL(timeout()), this, SLOT(_closeWidgetTimerFun()));

    d_ptr->_closeTimer = new QTimer();
    d_ptr->_closeTimer->setInterval(500);
    connect(d_ptr->_closeTimer, SIGNAL(timeout()), this, SLOT(_timerOut()));
}

CodeMarkerWindow::~CodeMarkerWindow()
{
    delete d_ptr;
}

void CodeMarkerWindow::setPress(bool flag)
{
    d_ptr->_isPress = flag;
}

bool CodeMarkerWindow::getPress()
{
    return d_ptr->_isPress;
}

void CodeMarkerWindow::changeScrollValue(int value)
{
    QScrollBar *bar = d_ptr->_scrollArea->verticalScrollBar();
    if (NULL != bar)
    {
        if (1 == value)
        {
            bar->setValue(bar->maximum());
        }
        else
        {
            bar->setValue(0);
        }
    }
}

int CodeMarkerWindow::getCodeMarkerTypeSize()
{
    return d_ptr->_localeCodeMarker.size();
}

void CodeMarkerWindow::startTimer()
{
    d_ptr->_timer->start();
}

void CodeMarkerWindow::showEvent(QShowEvent *e)
{
    setPress(false);
    d_ptr->_isChosen = false;
    d_ptr->_timer->start();
    Window::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
}

void CodeMarkerWindow::hideEvent(QHideEvent *e)
{
    // 未点击任一code marker，存储默认code marker。
    if (!getPress())
    {
        QString defCodeMarker("CodeMarkerDefault");
        // summaryStorageManager.addCodeMarker(timeManager.getCurTime(),
        //        defCodeMarker.toLocal8Bit().constData());
    }
    d_ptr->_timer->stop();

    setPress(false);
    d_ptr->_isChosen = false;

    Window::hideEvent(e);
}

void CodeMarkerWindow::exit()
{
    hide();
}

void CodeMarkerWindow::_btnReleased()
{
    CodeMarkerWindowButton *btn = qobject_cast<CodeMarkerWindowButton *>(sender());
    int index = btn->property("Item").toInt();
    if (!d_ptr->_isChosen)
    {
        d_ptr->_closeTimer->start();
        d_ptr->_codeMarkerNum = index;
        d_ptr->_isChosen = true;
    }
}

void CodeMarkerWindow::_closeWidgetTimerFun()
{
    hide();
}

void CodeMarkerWindow::_timerOut()
{
    d_ptr->_closeTimer->stop();
    // 存储code marker数据到summary。
    // summaryStorageManager.addCodeMarker(timeManager.getCurTime(),
    //        _localeCodeMarker[_codeMarkerNum].toLocal8Bit().constData());
    hide();
}

void CodeMarkerWindowButton::focusInEvent(QFocusEvent *e)
{
    // 如果是第一个button
    if (0 == _id)
    {
        // 调整滚动条位置。
        codeMarkerWindow.changeScrollValue(0);
    }

    // 如果是最后一个button
    if (codeMarkerWindow.getCodeMarkerTypeSize() == _id + 1)
    {
        // 调整滚动条位置。
        codeMarkerWindow.changeScrollValue(1);
    }
    Button::focusInEvent(e);
}

void CodeMarkerWindowButton::keyPressEvent(QKeyEvent *e)
{
    if (codeMarkerWindow.getPress())
    {
        return;
    }
    switch (e->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:

            if (!e->isAutoRepeat())
            {
                codeMarkerWindow.setPress(true);
                update();
            }
            eventStorageManager.triggerCodeMarkerEvent(text().toLatin1().data());
            break;
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            codeMarkerWindow.startTimer();
            update();
            break;
        default:
            break;
    }

    Button::keyPressEvent(e);
}
