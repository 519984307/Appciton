#include "Debug.h"
#include <QScrollBar>
#include <QTimer>
#include "SummaryStorageManager.h"
#include "TimeManager.h"
#include "LabelButton.h"
#include "FontManager.h"
#include "WindowManager.h"
#include "LanguageManager.h"
#include "CodeMarkerWidget.h"
#include "PublicMenuManager.h"
#include "CodeMarkerList.h"
#include "SystemManager.h"
#include "IConfig.h"

CodeMarkerWidget *CodeMarkerWidget::_selfObj = NULL;

/***************************************************************************************************
 * 焦点进入事件
 **************************************************************************************************/
CodeMarkerButton::CodeMarkerButton(int id): LButton(id)
{
    QPalette myPalette = palette();
    myPalette.setColor(QPalette::Shadow, QColor(146, 208, 80));
    myPalette.setColor(QPalette::HighlightedText, Qt::black);
    setPalette(myPalette);
}

CodeMarkerButton::~CodeMarkerButton()
{
}

void CodeMarkerButton::focusInEvent(QFocusEvent *e)
{
    // 如果是第一个button
    if (0 == this->getID())
    {
        // 调整滚动条位置。
        codeMarkerWidget.changeScrollValue(0);
    }

    // 如果是最后一个button
    if (codeMarkerWidget.getCodeMarkerTypeSize() == this->getID() + 1)
    {
        // 调整滚动条位置。
        codeMarkerWidget.changeScrollValue(1);
    }

    LButton::focusInEvent(e);
}

/***************************************************************************************************
 * 响应重绘事件。
 **************************************************************************************************/
void CodeMarkerButton::paintEvent(QPaintEvent */*e*/)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    if (isDown() || codeMarkerWidget.getPress())
    {
        painter.setBrush(palette().shadow());        // 暗色
    }
    else
    {
        if (hasFocus()) // 聚焦。
        {
            painter.setBrush(palette().highlight()); // 亮色
        }
        else if (!isEnabled())
        {
            painter.setBrush(Qt::gray);              // 暗色
        }
        else          // 非聚焦。
        {
            painter.setBrush(Qt::white);             // 无画刷
        }
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::white);
    painter.drawRoundedRect(rect(), 4, 4);

    // 绘文本。
    painter.setPen(hasFocus() ? palette().color(QPalette::HighlightedText) :
            palette().color(QPalette::WindowText));
    painter.drawText(rect(), Qt::AlignCenter, text());
}

/**************************************************************************************************
 * 处理键盘和飞棱事件。
 *************************************************************************************************/
void CodeMarkerButton::keyPressEvent(QKeyEvent *e)
{
    if (codeMarkerWidget.getPress())
    {
        return;
    }
    switch (e->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:

            if (!e->isAutoRepeat())
            {
                codeMarkerWidget.setPress(true);
                update();
            }
            break;
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            codeMarkerWidget.startTimer();
            update();
            break;
        default:
            break;
    }

    QPushButton::keyPressEvent(e);
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
CodeMarkerWidget::CodeMarkerWidget() : PopupWidget()
{
    // 获取当前Code Marker Type值
    QString codemarkerStr;
    int num = 0;
    superConfig.getNumAttr("Local|Language", "CurrentOption", num);
    QString markerStr = "CodeMarker|SelectMarker|Language";
    markerStr += QString::number(num, 10);
    superConfig.getStrValue(markerStr, codemarkerStr);
    _origCodeMarker = codemarkerStr.split(',');
    for (int i = 0; i < _origCodeMarker.size(); i++)
    {
        _localeCodeMarker.append(trs(_origCodeMarker[i]));
    }

    setPress(false);
    _isChosen = false;

    // 设置背景为白色
    QPalette p;
    p.setColor(QPalette::Background, Qt::white);
    setPalette(p);

    int fontSize = fontManager.getFontSize(1);
    int barWidth = 20;
    int borderWidth = 4;
    setBorderWidth(borderWidth);
    setTitleBarText(trs("CodeMarker"));
    QWidget *widget = new QWidget();
    widget->setFixedWidth(windowManager.getPopMenuWidth() - borderWidth * 2 - barWidth);
    setFixedWidth(windowManager.getPopMenuWidth());

    QGridLayout *layoutG = new QGridLayout();
    layoutG->setContentsMargins(0, 2, 0, 2);
    layoutG->setSpacing(0);

    // 添加滚动条
    _scorllArea = new QScrollArea();
    _scorllArea->setFocusPolicy(Qt::NoFocus);
    _scorllArea->setFrameStyle(QFrame::NoFrame);
    _scorllArea->setAlignment(Qt::AlignTop);
    _scorllArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scorllArea->verticalScrollBar()->setFixedWidth(barWidth);

    CodeMarkerButton *_codeMarkerButton[_localeCodeMarker.size()];
    for (int i = 0; i < _localeCodeMarker.size(); i++)
    {
        _codeMarkerButton[i] = new CodeMarkerButton(i);
        _codeMarkerButton[i]->setText(_localeCodeMarker[i]);
        _codeMarkerButton[i]->setFont(fontManager.textFont(fontSize));
        connect(_codeMarkerButton[i], SIGNAL(released(int)), this, SLOT(_btnReleased(int)));
        layoutG->addWidget(_codeMarkerButton[i], i / 4, i % 4, 1, 1);
    }

    widget->setLayout(layoutG);
    _scorllArea->setWidget(widget);
    contentLayout->addWidget(_scorllArea);

    _timer = new QTimer();
    _timer->setInterval(5000);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_closeWidgetTimerFun()));

    _closeTimer = new QTimer();
    _closeTimer->setInterval(500);
    connect(_closeTimer, SIGNAL(timeout()), this, SLOT(_timerOut()));
}

/***************************************************************************************************
 * 析构
 **************************************************************************************************/
CodeMarkerWidget::~CodeMarkerWidget()
{
    if (NULL != _timer)
    {
        delete _timer;
    }

    if (NULL != _closeTimer)
    {
        delete _closeTimer;
    }

    qDeleteAll(_lButton);
    _lButton.clear();
}

/***************************************************************************************************
 * 回调函数
 **************************************************************************************************/
void CodeMarkerWidget::_btnReleased(int id)
{
    if (!_isChosen)
    {
        _closeTimer->start();
        _codeMarkerNum = id;
        _isChosen = true;
    }
}

/***************************************************************************************************
 * 5秒后关闭code Marker窗口(当5s内未点击)
 **************************************************************************************************/
void CodeMarkerWidget::_closeWidgetTimerFun(void)
{
    hide();
}

void CodeMarkerWidget::_timerOut()
{
    _closeTimer->stop();
    // 存储code marker数据到summary。
    summaryStorageManager.addCodeMarker(timeManager.getCurTime(),
            _localeCodeMarker[_codeMarkerNum].toLocal8Bit().constData());
    hide();
}

/***************************************************************************************************
 * 显示事件
 **************************************************************************************************/
void CodeMarkerWidget::showEvent(QShowEvent *e)
{
    setPress(false);
    _isChosen = false;
    _timer->start();
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
}

void CodeMarkerWidget::hideEvent(QHideEvent *event)
{
    // 未点击任一code marker，存储默认code marker。
    if (!getPress())
    {
        QString defCodeMarker("CodeMarkerDefault");
        summaryStorageManager.addCodeMarker(timeManager.getCurTime(),
                defCodeMarker.toLocal8Bit().constData());
    }
    _timer->stop();

    setPress(false);
    _isChosen = false;

    PopupWidget::hideEvent(event);
}

void CodeMarkerWidget::exit(void)
{
    hide();
}

/**************************************************************************************************
 * 改变滚动区域滚动条的值
 *************************************************************************************************/
void CodeMarkerWidget::changeScrollValue(int value)
{
    QScrollBar *bar = _scorllArea->verticalScrollBar();
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

/**************************************************************************************************
 * 启动启动定时器。
 *************************************************************************************************/
void CodeMarkerWidget::startTimer(void)
{
    _timer->start();
}

/**************************************************************************************************
 * 获取code marker Type个数。
 *************************************************************************************************/
int CodeMarkerWidget::getCodeMarkerTypeSize()
{
    return _localeCodeMarker.size();
}

void CodeMarkerWidget::setPress(bool flag)
{
    _isPress = flag;
}

bool CodeMarkerWidget::getPress()
{
    return _isPress;
}
