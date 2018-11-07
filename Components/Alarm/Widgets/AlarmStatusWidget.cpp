/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/31
 **/


#include "AlarmStatusWidget.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "AlarmIndicator.h"
#include "AlarmInfoPopListView.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QPushButton>
#include "TopBarWidget.h"

/**************************************************************************************************
 * 重绘制。
 *************************************************************************************************/
void AlarmStatusWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);
    QPainter painter(this);

    // 显示正常报警。
    if (_audioStatus == ALARM_AUDIO_NORMAL)
    {
        // painter.fillRect(rect(), topBarWidget.getTopBarBlackGroundColor());
        return;
    }

    // 显示报警暂停/停止信息。
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::white);

    int border =  focusedBorderWidth() - 1;
    QRect tempRect = rect().adjusted(border, border, -border, -border);
    painter.drawRoundedRect(tempRect, 4, 4);

    int offx = (rect().width() - (rect().height() - 10)) / 2;
    QRect r = rect().adjusted(offx, 5, -offx, -5);
    if (_audioStatus == ALARM_AUDIO_OFF)
    {
        painter.drawPixmap(r, _muteAudioOff, _muteAudioOff.rect());
    }
    else if (_audioStatus == ALARM_OFF)
    {
        painter.drawPixmap(r, _muteAlarmOff, _muteAlarmOff.rect());
    }
    else if (_audioStatus == ALARM_AUDIO_SUSPEND)
    {
        painter.drawPixmap(r, _muteAlarmPause, _muteAlarmPause.rect());
    }
}

/**************************************************************************************************
 * 设置报警静音关闭状态。
 *************************************************************************************************/
void AlarmStatusWidget::setAudioNormal(void)
{
    _audioStatus = ALARM_AUDIO_NORMAL;
    update();
}

/**************************************************************************************************
 * 设置报警音停止打开状态。
 *************************************************************************************************/
void AlarmStatusWidget::setAudioOff(void)
{
    _audioStatus = ALARM_AUDIO_OFF;
    update();
}

/**************************************************************************************************
 * 设置报警停止打开状态。
 *************************************************************************************************/
void AlarmStatusWidget::setAlarmOff(void)
{
    _audioStatus = ALARM_OFF;
    update();
}

/**************************************************************************************************
 * 设置报警暂停。
 *************************************************************************************************/
void AlarmStatusWidget::setAlarmPause(void)
{
    _audioStatus = ALARM_AUDIO_SUSPEND;
    update();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmStatusWidget::AlarmStatusWidget() : IWidget("AlarmStatusWidget")
{
    setFocusPolicy(Qt::NoFocus);

    _muteAlarmPause.load("/usr/local/nPM/icons/AlarmAudioPause.png");
    _muteAudioOff.load("/usr/local/nPM/icons/AlarmAudioOff.png");
    _muteAlarmOff.load("/usr/local/nPM/icons/AlarmOff.png");

    _audioStatus = ALARM_AUDIO_NORMAL;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmStatusWidget::~AlarmStatusWidget()
{
}
