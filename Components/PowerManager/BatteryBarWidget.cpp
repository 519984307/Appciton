/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/23
 **/


#include <QPixmap>
#include "BatteryBarWidget.h"
#include "FontManager.h"
#include "WindowManager.h"
#include "MenuManager.h"
#include "TopBarWidget.h"
#include "IConfig.h"
#include <QPainter>
#include "BatteryIndicatorWindow.h"

#define blackgroundColor QColor(20 , 20 , 20)

BatteryBarWidget *BatteryBarWidget::_selfObj = NULL;

/**************************************************************************************************
 * 设置电池状态
 *************************************************************************************************/
void BatteryBarWidget::setStatus(BatteryIconStatus status)
{
    _batteryIconWidget->setStatus(status);
    _batteryStatus = status;
    update();
}

/**************************************************************************************************
 * 设置电量参数level = 0-5.
 *************************************************************************************************/
void BatteryBarWidget::setVolume(BatteryPowerLevel volume)
{
    _batteryIconWidget->setVolume(volume);
}

/**************************************************************************************************
 * 设置填充颜色
 *************************************************************************************************/
void BatteryBarWidget::setFillColor(const QColor &color)
{
    _batteryIconWidget->setFillColor(color);
}

/**************************************************************************************************
 * 设置剩余时间参数
 *************************************************************************************************/
void BatteryBarWidget::setTimeValue(BatteryRemainTime time)
{
    _batteryIconWidget->setTimeValue(time);
}

/**************************************************************************************************
 * 设置剩余时间参数
 *************************************************************************************************/
void BatteryBarWidget::setIcon(BatteryPowerLevel level)
{
    if (level == BAT_VOLUME_5)
    {
        _batteryIconWidget->setVolume(BAT_VOLUME_5);
    }
    else if (level == BAT_VOLUME_4)
    {
        _batteryIconWidget->setVolume(BAT_VOLUME_4);
    }
    else if (level == BAT_VOLUME_3)
    {
        _batteryIconWidget->setVolume(BAT_VOLUME_3);
    }
    else if (level == BAT_VOLUME_2)
    {
        _batteryIconWidget->setVolume(BAT_VOLUME_2);
    }
    else if (level == BAT_VOLUME_1)
    {
        _batteryIconWidget->setVolume(BAT_VOLUME_1);
    }
    else
    {
        _batteryIconWidget->setVolume(BAT_VOLUME_0);
    }

    _batteryIconWidget->setTimeValue(BAT_REMAIN_TIME_NULL);
    _batteryIconWidget->setFillColor(QColor(0, 128, 0));
}

void BatteryBarWidget::setIconFull()
{
    _batteryIconWidget->setVolume(BAT_VOLUME_5);
    _batteryIconWidget->setTimeValue(BAT_REMAIN_TIME_NULL);
    _batteryIconWidget->setFillColor(QColor(0, 128, 0));
}

void BatteryBarWidget::setIconLow()
{
    _batteryIconWidget->setVolume(BAT_VOLUME_1);
    _batteryIconWidget->setTimeValue(BAT_REMAIN_TIME_LOW);
    _batteryIconWidget->setFillColor(Qt::red);
}

void BatteryBarWidget::charging()
{
    _batteryIconWidget->charging();
}

void BatteryBarWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.fillRect(this->rect(), blackgroundColor);
    IWidget::paintEvent(e);
}

/**************************************************************************************************
 * 释放事件回调函数
 *************************************************************************************************/
void BatteryBarWidget::_batRealsed(IWidget * iWidget)
{
    Q_UNUSED(iWidget)
    BatteryIndicatorWindow batWin;
    batWin.exec();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
BatteryBarWidget::BatteryBarWidget() : IWidget("BatteryBarWidget")
{
    _batteryIconWidget = new BatteryIconWidget(blackgroundColor);
    _batteryStatus = BATTERY_NORMAL;

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->addWidget(_batteryIconWidget);
    setLayout(layout);

    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_batRealsed(IWidget *)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
BatteryBarWidget::~BatteryBarWidget()
{
}
