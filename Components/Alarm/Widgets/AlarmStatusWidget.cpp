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
#include <QPainter>
#include <QHBoxLayout>
#include "IConfig.h"

class AlarmStatusWidgetPrivate
{
public:
    AlarmStatusWidgetPrivate()
        : alarmStatus(ALARM_STATUS_NORMAL),
          isShowAudioOff(false)
    {
        alarmPausePixmap.load("/usr/local/nPM/icons/AlarmPause.png");
        audioOffPixmap.load("/usr/local/nPM/icons/AlarmAudioOff.png");
        alarmOffPixmap.load("/usr/local/nPM/icons/AlarmOff.png");
        alarmResetPixmap.load("/usr/local/nPM/icons/AlarmReset.png");
    }

    QPixmap alarmPausePixmap;            // 报警暂停图标。
    QPixmap audioOffPixmap;              // 报警静音图标。
    QPixmap alarmOffPixmap;              // 报警关闭图标。
    QPixmap alarmResetPixmap;          // alarm reset icon
    AlarmStatus alarmStatus;
    bool isShowAudioOff;
};

/**************************************************************************************************
 * 重绘制。
 *************************************************************************************************/
void AlarmStatusWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);
    QPainter painter(this);

    // 显示正常报警。
    if (d_ptr->alarmStatus == ALARM_STATUS_NORMAL)
    {
        if (d_ptr->isShowAudioOff)
        {
            return;
        }
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
    if (d_ptr->alarmStatus == ALARM_STATUS_NORMAL)
    {
        if (!d_ptr->isShowAudioOff)
        {
            painter.drawPixmap(r, d_ptr->audioOffPixmap, d_ptr->alarmPausePixmap.rect());
        }
    }
    if (d_ptr->alarmStatus == ALARM_STATUS_AUDIO_OFF)
    {
        painter.drawPixmap(r, d_ptr->audioOffPixmap, d_ptr->audioOffPixmap.rect());
    }
    else if (d_ptr->alarmStatus == ALARM_STATUS_OFF)
    {
        painter.drawPixmap(r, d_ptr->alarmOffPixmap, d_ptr->alarmOffPixmap.rect());
    }
    else if (d_ptr->alarmStatus == ALARM_STATUS_PAUSE)
    {
        painter.drawPixmap(r, d_ptr->alarmPausePixmap, d_ptr->alarmPausePixmap.rect());
    }
    else if (d_ptr->alarmStatus == ALARM_STATUS_RESET)
    {
        painter.drawPixmap(r, d_ptr->alarmResetPixmap, d_ptr->alarmResetPixmap.rect());
    }
}

void AlarmStatusWidget::setAlarmStatus(AlarmStatus status)
{
    d_ptr->alarmStatus = status;
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
    updateAlarmAudioStatus();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmStatusWidget::~AlarmStatusWidget()
{
    delete d_ptr;
}

void AlarmStatusWidget::updateAlarmAudioStatus()
{
    int index = 0;
    systemConfig.getNumValue("Alarms|AlarmAudio", index);
    d_ptr->isShowAudioOff = index ? true : false;
    update();
}
