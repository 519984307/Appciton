/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/23
 **/


#pragma once
#include "IWidget.h"
#include <QMap>
#include "BatteryIconWidget.h"

/***************************************************************************************************
 * 系统状态栏，主要显示一些系统提示图标。
 **************************************************************************************************/
class BatteryBarWidget : public IWidget
{
    Q_OBJECT

public:
    static BatteryBarWidget &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new BatteryBarWidget();
        }
        return *_selfObj;
    }
    static BatteryBarWidget *_selfObj;
    ~BatteryBarWidget();

public:
    // 设置电池状态
    void setStatus(BatteryIconStatus status);

    // 获取当前电池状态
    BatteryIconStatus getStatus()
    {
        return _batteryStatus;
    }

    // 设置电量参数level = 0-15.
    void setVolume(BatteryPowerLevel level);

    // 设置填充颜色
    void setFillColor(const QColor &color);

    // 设置剩余时间参数
    void setTimeValue(BatteryRemainTime time);

    // 设置电池图标
    void setIcon(BatteryPowerLevel level);
    void setIconFull(void);
    void setIconLow(void);
    void charging();

protected:
    void paintEvent(QPaintEvent *e);

private slots:
    void _batRealsed(IWidget *);

private:
    BatteryBarWidget();

    BatteryIconWidget *_batteryIconWidget;

    BatteryIconStatus _batteryStatus;            // 电池状态
};
#define batteryBarWidget (BatteryBarWidget::construction())
