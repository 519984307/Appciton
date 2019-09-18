/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/11/13
 **/

#pragma once
#include "BatteryBarWidget.h"
#include "SystemBoardProvider.h"

class PowerMangerPrivate;
class PowerManger : public QObject
{
    Q_OBJECT
public:
    static PowerManger &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new PowerManger;
        }
        return *_selfObj;
    }
    static PowerManger *_selfObj;
    ~PowerManger();

    /**
     * @brief setBatteryCapacity 设置电池AD值
     * @param adc
     */
    void setBatteryCapacity(short adc);

    /**
     * @brief getBatteryCapacity 获取电池电量
     * @return
     */
    QString getBatteryCapacity();

public:
    void run(void);

private slots:
    void powerOff();

private:
    PowerManger();
    PowerMangerPrivate * const d_ptr;
};

#define powerManger (PowerManger::construction())
