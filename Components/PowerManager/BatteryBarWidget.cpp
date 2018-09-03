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
void BatteryBarWidget::setVolume(int volume)
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
void BatteryBarWidget::setTimeValue(int time)
{
    _batteryIconWidget->setTimeValue(time);
}

/**************************************************************************************************
 * 设置剩余时间参数
 *************************************************************************************************/
void BatteryBarWidget::setIcon(int time)
{
    if (time >= 6)
    {
        _batteryIconWidget->setVolume(4);
    }
    else if (time >= 4)
    {
        _batteryIconWidget->setVolume(3);
    }
    else if (time >= 2)
    {
        _batteryIconWidget->setVolume(2);
    }
    else if (time >= -1)
    {
        _batteryIconWidget->setVolume(1);
    }
    else
    {
        _batteryIconWidget->setVolume(0);
    }

    _batteryIconWidget->setTimeValue(time);
    _batteryIconWidget->setFillColor(QColor(0, 128, 0));
}

void BatteryBarWidget::setIconFull()
{
    _batteryIconWidget->setVolume(5);
    _batteryIconWidget->setTimeValue(-2);
    _batteryIconWidget->setFillColor(QColor(0, 128, 0));
}

void BatteryBarWidget::setIconLow()
{
    _batteryIconWidget->setVolume(1);
    _batteryIconWidget->setTimeValue(-1);
    _batteryIconWidget->setFillColor(Qt::yellow);
}

/**************************************************************************************************
 * 释放事件回调函数
 *************************************************************************************************/
void BatteryBarWidget::_batRealsed(IWidget * iWidget)
{
    windowManager.showWindow(&_batteryIndicatorWindow , WindowManager::ShowBehaviorModal);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
BatteryBarWidget::BatteryBarWidget() : IWidget("BatteryBarWidget")
{
    int topBarHeight;
    systemConfig.getNumValue("PrimaryCfg|UILayout|WidgetsOrder|ScreenVLayoutStretch|topBarRow", topBarHeight);
    int batteryWidth = topBarHeight * 1.5;
    setFixedWidth(batteryWidth);

    _batteryIconWidget = new BatteryIconWidget(topBarWidget.getTopBarBlackGroundColor());
    _batteryStatus = BATTERY_NORMAL;

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(6);
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
