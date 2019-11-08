/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/25
 **/


#include "AlarmTechInfoBarWidget.h"
#include "Framework/Language/LanguageManager.h"
#include "FontManager.h"
#include "AlarmIndicator.h"
#include <QPainter>
#include "WindowManager.h"
#include "SoundManagerInterface.h"

/**************************************************************************************************
 * 绘制背景。
 *************************************************************************************************/
void AlarmTechInfoBarWidget::_drawBackground(void)
{
    if (_text.isEmpty())
    {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::transparent);
    if (_alarmPriority == ALARM_PRIO_HIGH)
    {
        painter.setBrush(QColor(255, 0, 0));
    }
    else if (_alarmPriority == ALARM_PRIO_MED)
    {
        painter.setBrush(QColor(255, 255, 0));
    }
    else if (_alarmPriority == ALARM_PRIO_LOW)
    {
        painter.setBrush(QColor(0, 175, 219));
    }
    else
    {
        painter.setBrush(QColor(30, 30, 30));
    }

    int border =  focusedBorderWidth() - 1;
    QRect tempRect = rect().adjusted(border, border, -border, -border);
    painter.drawRoundedRect(tempRect, 4, 4);
}

/**************************************************************************************************
 * 绘制文本。
 *************************************************************************************************/
void AlarmTechInfoBarWidget::_drawText(void)
{
    if (_text.isEmpty())
    {
        return;
    }

    QPainter painter(this);
    if (_alarmPriority == ALARM_PRIO_HIGH || _alarmPriority == ALARM_PRIO_PROMPT)
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
    switch (_alarmPriority)
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

    if (_type != _alarmType && 0 != _pauseTime)
    {
        r.adjust(0, 0, -6, 0);
        QImage image("/usr/local/nPM/icons/MutePause.png");
        painter.drawImage(QRect(width() - 60, (height() - 16) / 2, 16, 16), image);
    }
}

/**************************************************************************************************
 * 重绘制。
 *************************************************************************************************/
void AlarmTechInfoBarWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);
    _drawBackground();
    _drawText();
}

void AlarmTechInfoBarWidget::mousePressEvent(QMouseEvent *e)
{
    IWidget::mousePressEvent(e);
    // 触屏点击播放按键音
    SoundManagerInterface *sound = SoundManagerInterface::getSoundManager();
    if (sound)
    {
        sound->keyPressTone();
    }
}

void AlarmTechInfoBarWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    //报警少于一个时，不显示。
    int total = alarmIndicator.getAlarmCount(_alarmType);
    if (total <= 1)
    {
        return;
    }

    if (NULL == _alarmWindow)
    {
        _alarmWindow = new AlarmInfoWindow(trs("TechAlarmList"), _alarmType);
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

void AlarmTechInfoBarWidget::_alarmListHide()
{
    updateList();
}

/**************************************************************************************************
 * 清除信息。
 *************************************************************************************************/
void AlarmTechInfoBarWidget::clear(void)
{
    _alarmPriority = ALARM_PRIO_LOW;
    _text.clear();
    _type = _alarmType;
    _latch = false;
    _pauseTime = 0;
    update();
    updateList();
}

/**************************************************************************************************
 * 设置报警提示信息。
 *************************************************************************************************/
void AlarmTechInfoBarWidget::display(const AlarmInfoNode &node)
{
    _alarmPriority = node.alarmPriority;
    _text = node.alarmMessage;
    _type = node.alarmType;
    _latch = node.latch;
    _acknowledge = node.acknowledge;
    _pauseTime = node.pauseTime;
    update();
    updateList();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmTechInfoBarWidget::AlarmTechInfoBarWidget(const QString &name) :
    IWidget(name),
    _alarmWindow(NULL),
    _alarmType(ALARM_TYPE_TECH)
{
    AlarmInfoBarWidget *old = registerAlarmInfoBar(ALARM_TYPE_TECH, this);
    if (old)
    {
        delete old;
    }
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmTechInfoBarWidget::~AlarmTechInfoBarWidget()
{
}

void AlarmTechInfoBarWidget::updateList()
{
    if (NULL != _alarmWindow && _alarmWindow->isActiveWindow())
    {
        // 报警信息窗口活跃时，才更新窗口数据
        _alarmWindow->updateData();
    }
}
