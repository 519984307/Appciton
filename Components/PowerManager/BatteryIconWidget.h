/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/31
 **/


#pragma once
#include <QWidget>
#include <QMap>
#include "BatteryDefine.h"

enum BatteryIconStatus
{
    BATTERY_NONE = -1,
    BATTERY_NORMAL,                 // 正常
    BATTERY_NOT_EXISTED,            // 无电池
    BATTERY_COM_FAULT,              // 通信错误
    BATTERY_ERROR,                  // 电池错误
    BATTERY_CALIBRATION_REQUIRED,   // 电池需要校准
    BATTERY_CHARGING                // 充电中
};

/***************************************************************************************************
 * 系统状态栏，主要显示一些系统提示图标。
 **************************************************************************************************/
class BatteryIconWidgetPrivate;

class BatteryIconWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryIconWidget(QColor iconBackGround = QColor());
    ~BatteryIconWidget();

    // 设置电池状态
    void setStatus(BatteryIconStatus status);

    // 设置电量参数level = 0-5.
    void setVolume(BatteryPowerLevel volume);

    // 设置填充颜色
    void setFillColor(const QColor &color);

    // 设置剩余时间参数
    void setTimeValue(BatteryRemainTime time);

    /**
     * @brief charging 充电中，动画显示
     */
    void charging();

protected:
    void paintEvent(QPaintEvent *e);

private slots:
    /**
     * @brief chargingTimeOut 电池充电定时器，刷新电池充电动画
     */
    void chargingTimeOut();

private:
    BatteryIconWidgetPrivate *const d_ptr;
};
