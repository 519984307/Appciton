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

class PowerMangerBriefPrivate;
class PowerMangerBrief : public QObject
{
    Q_OBJECT
public:
    static PowerMangerBrief &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new PowerMangerBrief;
        }
        return *_selfObj;
    }
    static PowerMangerBrief *_selfObj;
    ~PowerMangerBrief();

    /**
     * @brief setBatteryQuantity 设置电池电量
     * @param adc
     */
    void setBatteryCapacity(short adc);

    /**
     * @brief getBatteryQuantity 获取电池电量
     * @return
     */
    QString getBatteryCapacity();

public:
    void run(void);

private slots:
    void powerOff();

private:
    PowerMangerBrief();
    PowerMangerBriefPrivate * const d_ptr;
};

#define powerMangerBrief (PowerMangerBrief::construction())
