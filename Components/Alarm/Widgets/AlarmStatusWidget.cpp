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

class AlarmStatusWidgetPrivate
{
public:
    AlarmStatusWidgetPrivate()
        :audioStatus(ALARM_AUDIO_NORMAL)
    {
        alarmPausePixmap.load("/usr/local/nPM/icons/AlarmAudioPause.png");
        audioOffPixmap.load("/usr/local/nPM/icons/AlarmAudioOff.png");
        alarmOffPixmap.load("/usr/local/nPM/icons/AlarmOff.png");
        alarmResetPixmap.load("/usr/local/nPM/icons/AlarmReset.png");
    }

    QPixmap alarmPausePixmap;            // 报警暂停图标。
    QPixmap audioOffPixmap;              // 报警静音图标。
    QPixmap alarmOffPixmap;              // 报警关闭图标。
    QPixmap alarmResetPixmap;          // alarm reset icon
    AlarmAudioStatus audioStatus;
};

/**************************************************************************************************
 * 重绘制。
 *************************************************************************************************/
void AlarmStatusWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);
    QPainter painter(this);

    // 显示正常报警。
    if (d_ptr->audioStatus == ALARM_AUDIO_NORMAL)
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
    if (d_ptr->audioStatus == ALARM_AUDIO_OFF)
    {
        painter.drawPixmap(r, d_ptr->audioOffPixmap, d_ptr->audioOffPixmap.rect());
    }
    else if (d_ptr->audioStatus == ALARM_OFF)
    {
        painter.drawPixmap(r, d_ptr->alarmOffPixmap, d_ptr->alarmOffPixmap.rect());
    }
    else if (d_ptr->audioStatus == ALARM_AUDIO_SUSPEND)
    {
        painter.drawPixmap(r, d_ptr->alarmPausePixmap, d_ptr->alarmPausePixmap.rect());
    }
    else if (d_ptr->audioStatus == ALARM_RESET)
    {
        painter.drawPixmap(r, d_ptr->alarmResetPixmap, d_ptr->alarmResetPixmap.rect());
    }
}

void AlarmStatusWidget::setAlarmStatus(AlarmAudioStatus status)
{
    d_ptr->audioStatus = status;
    update();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmStatusWidget::AlarmStatusWidget()
    :IWidget("AlarmStatusWidget"),
    d_ptr(new AlarmStatusWidgetPrivate())
{
    setFocusPolicy(Qt::NoFocus);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmStatusWidget::~AlarmStatusWidget()
{
    delete d_ptr;
}
