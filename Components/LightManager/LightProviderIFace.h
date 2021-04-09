/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/17
 **/


#pragma once
#include "AlarmDefine.h"

class LightProviderIFace
{
public:
    // 自检，发生在开机时刻，依次亮静音灯、低、中、高级别报警灯，最后熄灭。
    virtual void selfTest(void) = 0;

    // 设置当前的报警级别。
    // 当hasAlarmed为false时，表明当前无报警后面的priority不起作用。
    // 当hasAlarmed为true时，priority表示报警的级别。
    virtual void updateAlarm(bool hasAlarmed, AlarmPriority priority) = 0;

    // ebable the backlight of the Keyboard
    virtual void enableKeyboardBacklight(bool enable) = 0;

    // 指示灯控制。
    virtual void enableIndicatorLight(bool enable) = 0;

    //发送协议命令
    virtual void sendCmdData(unsigned char /*cmdId*/, const unsigned char */*data*/,
                             unsigned int /*len*/) { }

    // 构造与析构。
    LightProviderIFace() {}
    virtual ~LightProviderIFace() {}
};
