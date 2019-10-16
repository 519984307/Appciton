/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/25
 **/


#include "AlarmPhyInfoBarWidget.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "AlarmIndicator.h"
#include <QPainter>
#include "WindowManager.h"
#include "Alarm.h"
#include "SoundManagerInterface.h"

/**************************************************************************************************
 * 绘制背景。
 *************************************************************************************************/
void AlarmPhyInfoBarWidget::_drawBackground(void)
{
    if (_text.isEmpty())
    {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::transparent);
    AlarmPriority priority = _alarmSource->getAlarmPriority(_alarmID);
    if (priority == ALARM_PRIO_HIGH)
    {
        painter.setBrush(QColor(255, 0, 0));
    }
    else if (priority == ALARM_PRIO_PROMPT)
    {
        painter.setBrush(QColor(30, 30, 30));
    }
    else
    {
        painter.setBrush(QColor(255, 255, 0));
    }

    int border =  focusedBorderWidth() - 1;
    QRect tempRect = rect().adjusted(border, border, -border, -border);
    painter.drawRoundedRect(tempRect, 4, 4);
}

/**************************************************************************************************
 * 绘制文本。
 *************************************************************************************************/
void AlarmPhyInfoBarWidget::_drawText(void)
{
    if (_text.isEmpty())
    {
        return;
    }

    QPainter painter(this);
    AlarmPriority priority = _alarmSource->getAlarmPriority(_alarmID);
    if (priority == ALARM_PRIO_HIGH)
    {
        painter.setPen(Qt::white);
    }
    else
    {
        painter.setPen(Qt::black);
    }

    QRect r = this->rect();
    if (_acknowledge)
    {
        QImage image("/usr/local/nPM/icons/select.png");
        painter.drawImage(QRect(focusedBorderWidth() + 6, (height() - 16) / 2, 16, 16), image);
        r.adjust(focusedBorderWidth() + 6 + 16 + 6, 0, 0, 0);
    }
    else
    {
        r.adjust(focusedBorderWidth() + 6, 0, 0, 0);
    }

    QString nameStr;
    switch (_alarmSource->getAlarmPriority(_alarmID))
    {
    case ALARM_PRIO_LOW:
        nameStr = "*";
        break;
    case ALARM_PRIO_MED:
        nameStr = "**";
        break;
    case ALARM_PRIO_HIGH:
        nameStr = "***";
        break;
    default:
        break;
    }
    nameStr += " ";
    nameStr += trs(_text);
    int fontSize = fontManager.textFontSize(r, nameStr, false, fontManager.getFontSize(4));
    painter.setFont(fontManager.textFont(fontSize));
    painter.drawText(r, Qt::AlignVCenter | Qt::AlignLeft, nameStr);
}

void AlarmPhyInfoBarWidget::_drawAlarmPauseMessage()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(QColor(0, 175, 219));
    painter.setPen(Qt::black);
    int border = focusedBorderWidth() - 1;
    QRect tempRect = rect().adjusted(border, border, -border, -border);
    painter.drawRoundedRect(tempRect, 4, 4);

    QString s;
    if (_alarmPauseTime == INT_MAX)
    {
        s = QString(trs("AlarmOff"));
    }
    else
    {
        s = QString("%1 %2s").arg(trs("AlarmPause")).arg(_alarmPauseTime);
    }

    int fontSize = fontManager.textFontSize(tempRect, s, false, fontManager.getFontSize(4));
    painter.setFont(fontManager.textFont(fontSize));
    painter.drawText(tempRect, Qt::AlignCenter, s);
}

/**************************************************************************************************
 * 重绘制。
 *************************************************************************************************/
void AlarmPhyInfoBarWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);
    if (_alarmPauseTime > 0)
    {
        // draw the alarm pause message
        _drawAlarmPauseMessage();
    }
    else
    {
        _drawBackground();
        _drawText();
    }
}

void AlarmPhyInfoBarWidget::mousePressEvent(QMouseEvent *e)
{
    IWidget::mousePressEvent(e);
    // 触屏点击播放按键音
    SoundManagerInterface *sound = SoundManagerInterface::getSoundManager();
    if (sound)
    {
        sound->keyPressTone();
    }
}

void AlarmPhyInfoBarWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)

    //报警少于一个时，不显示。
    int total = alarmIndicator.getAlarmCount(_alarmType);
    if (total < 1)
    {
        return;
    }

    if (NULL == _alarmWindow)
    {
        _alarmWindow = new AlarmInfoWindow(trs("PhyAlarmList"), _alarmType);
        connect(_alarmWindow, SIGNAL(windowHide(Dialog *)), this, SLOT(_alarmListHide()));
        windowManager.showWindow(_alarmWindow, WindowManager::ShowBehaviorCloseOthers
                                 | WindowManager::ShowBehaviorCloseIfVisiable);
    }
    else if (_alarmWindow->isHidden())
    {
        windowManager.showWindow(_alarmWindow, WindowManager::ShowBehaviorCloseOthers
                                 | WindowManager::ShowBehaviorCloseIfVisiable);
    }
    else
    {
        return;
    }

    update();
}

void AlarmPhyInfoBarWidget::_alarmListHide()
{
    updateList();
}

/**************************************************************************************************
 * 清除信息。
 *************************************************************************************************/
void AlarmPhyInfoBarWidget::clear(void)
{
    _alarmPriority = ALARM_PRIO_LOW;
    _text.clear();
    _latch = false;
    update();
    updateList();
}

/**************************************************************************************************
 * 设置报警提示信息。
 *************************************************************************************************/
void AlarmPhyInfoBarWidget::display(AlarmInfoNode &node)
{
    _alarmPriority = node.alarmPriority;
    _text = node.alarmMessage;
    _message = node.alarmMessage;
    _latch = node.latch;
    _acknowledge = node.acknowledge;
    _alarmSource = node.alarmSource;
    _alarmID = node.alarmID;
    update();
    updateList();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmPhyInfoBarWidget::AlarmPhyInfoBarWidget(const QString &name) :
    IWidget(name),
    _alarmPriority(ALARM_PRIO_LOW),
    _text(QString()),
    _message(NULL),
    _latch(false),
    _acknowledge(false),
    _alarmSource(NULL),
    _alarmID(0),
    _alarmPauseTime(-1),
    _alarmWindow(NULL),
    _alarmType(ALARM_TYPE_PHY)
{
    AlarmInfoBarWidget *old = registerAlarmInfoBar(ALARM_TYPE_PHY, this);
    if (old)
    {
        delete old;
    }
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmPhyInfoBarWidget::~AlarmPhyInfoBarWidget()
{
}

void AlarmPhyInfoBarWidget::updateList()
{
    // 窗口显示时不做数据更新
    if (NULL != _alarmWindow && _alarmWindow->isHidden())
    {
        _alarmWindow->updateData();
    }
}

void AlarmPhyInfoBarWidget::setAlarmPause(int seconds)
{
    if (_alarmPauseTime == seconds)
    {
        return;
    }

    _alarmPauseTime = seconds;

    update();
}
