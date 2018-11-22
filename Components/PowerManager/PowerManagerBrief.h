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
#include "BatteryMessage.h"
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

public:
    void run(void);

private slots:
    void powerOff();

private:
    PowerMangerBrief();
    PowerMangerBriefPrivate * const d_ptr;
};

#define powerMangerBrief (PowerMangerBrief::construction())
