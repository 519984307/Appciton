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
#include "Framework/Language/LanguageManager.h"
#include "FontManager.h"
#include "WindowManager.h"
#include <QTimer>
#include "EventStorageManager.h"
#include "TimeManager.h"
#include "IConfig.h"
#include <QKeyEvent>

class CodeMarkerWindowPrivate
{
public:
    CodeMarkerWindowPrivate()
        : scrollArea(NULL)
        , closeTimer(NULL)
        , isPress(false)
        , isChosen(false)
        , codeMarkerNum(0)
    {
        for (int i = 0 ; i < 30 ; i++)
        {
            codeMarkerButton[i] = NULL;
        }
    }

    QScrollArea *scrollArea;
    QList<Button *> button;
    Button *codeMarkerButton[30];
    QStringList origCodeMarker;      // 未经过翻译的code emarker。
    QStringList localeCodeMarker;    // 翻译后的code emarker。
    QTimer *closeTimer;
    bool isPress;                    // codemark按钮被按下
    bool isChosen;                   // codemark按钮已按下选中
    int codeMarkerNum;
};

CodeMarkerWindow::CodeMarkerWindow() : Dialog()
        , d_ptr(new CodeMarkerWindowPrivate())
{
    QString codemarkerStr;
    int num = 0;
    systemConfig.getNumAttr("General|Language", "CurrentOption", num);
    QString markerStr = "CodeMarker|SelectMarker|Language";
    markerStr += QString::number(num, 10);
    currentConfig.getStrValue(markerStr, codemarkerStr);
    d_ptr->origCodeMarker = codemarkerStr.split(',');
    for (int i = 0; i < d_ptr->origCodeMarker.size(); i++)
    {
        d_ptr->localeCodeMarker.append(trs(d_ptr->origCodeMarker[i]));
    }

    setPress(false);
    d_ptr->isChosen = false;


    int fontSize = fontManager.getFontSize(3);

    setWindowTitle(trs("CodeMarker"));
    setFixedWidth(windowManager.getPopWindowWidth());
    // scroll
    d_ptr->scrollArea = new QScrollArea();
    d_ptr->scrollArea->setFocusPolicy(Qt::NoFocus);
    d_ptr->scrollArea->setFrameStyle(QFrame::NoFrame);
    d_ptr->scrollArea->setAlignment(Qt::AlignTop);
    d_ptr->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGridLayout *layoutG = new QGridLayout();
    layoutG->setContentsMargins(4, 2, 4, 2);
    layoutG->setSpacing(0);

    CodeMarkerWindowButton *codeMarkerButton[d_ptr->localeCodeMarker.size()];
    for (int i = 0 ; i < d_ptr->localeCodeMarker.size() ; i++)
    {
        codeMarkerButton[i] = new CodeMarkerWindowButton(i);
        codeMarkerButton[i]->setText(d_ptr->localeCodeMarker[i]);
        codeMarkerButton[i]->setButtonStyle(Button::ButtonTextOnly);
        codeMarkerButton[i]->setBorderWidth(0);
        codeMarkerButton[i]->setFont(fontManager.textFont(fontSize));
        connect(codeMarkerButton[i], SIGNAL(released()), this, SLOT(_btnReleased()));
        codeMarkerButton[i]->setProperty("Item" , qVariantFromValue(i));
        layoutG->addWidget(codeMarkerButton[i], i / 4, i % 4, 1, 1);
    }

    setWindowLayout(layoutG);

    d_ptr->closeTimer = new QTimer();
    d_ptr->closeTimer->setInterval(500);
    connect(d_ptr->closeTimer, SIGNAL(timeout()), this, SLOT(_timerOut()));
}

CodeMarkerWindow &CodeMarkerWindow::getInstance()
{
    static CodeMarkerWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new CodeMarkerWindow();
    }
    return *instance;
}

CodeMarkerWindow::~CodeMarkerWindow()
{
    delete d_ptr;
}

void CodeMarkerWindow::setPress(bool flag)
{
    d_ptr->isPress = flag;
}

bool CodeMarkerWindow::getPress()
{
    return d_ptr->isPress;
}

void CodeMarkerWindow::ensureWidgetVisiable(int id)
{
    d_ptr->scrollArea->ensureWidgetVisible(d_ptr->codeMarkerButton[id]);
}

int CodeMarkerWindow::getCodeMarkerTypeSize()
{
    return d_ptr->localeCodeMarker.size();
}


void CodeMarkerWindow::showEvent(QShowEvent *e)
{
    setPress(false);
    d_ptr->isChosen = false;
    Dialog::showEvent(e);
}

void CodeMarkerWindow::hideEvent(QHideEvent *e)
{
    setPress(false);
    d_ptr->isChosen = false;

    Dialog::hideEvent(e);
}

void CodeMarkerWindow::exit()
{
    hide();
}

void CodeMarkerWindow::_btnReleased()
{
    CodeMarkerWindowButton *btn = qobject_cast<CodeMarkerWindowButton *>(sender());
    int index = btn->property("Item").toInt();
    if (!d_ptr->isChosen)
    {
        d_ptr->closeTimer->start();
        d_ptr->codeMarkerNum = index;
        d_ptr->isChosen = true;
    }
    unsigned currentTime = timeManager.getCurTime();
    eventStorageManager.triggerCodeMarkerEvent(btn->text().toLatin1().data(), currentTime);
}

void CodeMarkerWindow::_timerOut()
{
    d_ptr->closeTimer->stop();
    hide();
}

void CodeMarkerWindowButton::focusInEvent(QFocusEvent *e)
{
    // 如果是第一个button
    if (0 == _id)
    {
        // 调整滚动条位置。
        codeMarkerWindow.ensureWidgetVisiable(0);
    }

    // 如果是最后一个button
    if (codeMarkerWindow.getCodeMarkerTypeSize() == _id + 1)
    {
        // 调整滚动条位置。
        codeMarkerWindow.ensureWidgetVisiable(_id);
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
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        update();
        break;
    default:
        break;
    }

    Button::keyPressEvent(e);
}
