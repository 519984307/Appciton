/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/12
 **/

#pragma once

class O2ProviderIFace
{
public:
    O2ProviderIFace(){}
    virtual ~O2ProviderIFace(){}

    // 发送协议命令
    virtual void sendCmdData(unsigned char /*cmdId*/, const unsigned char */*data*/, unsigned int /*len*/){}

    // 获取版本号
    virtual void sendVersion(void){}

    // 发送自检指令
    virtual void sendO2SelfTest(void){}

    // 获取探头状态
    virtual void sendProbeState(void){}

    // 传感器校准
    virtual void sendCalibration(int /*concentration*/){}

    // 马达控制
    virtual void sendMotorControl(int /*status*/){}

    // 震动强度控制
    virtual void sendVibrationIntensity(int /*intensity*/){}
};
