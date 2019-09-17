/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/9/12
 **/

#pragma once
#include "TEMPDefine.h"

// 定义ECG数据提供对象需要实现的接口方法。
class TEMPProviderIFace
{
public:
    TEMPProviderIFace() {  }
    virtual ~TEMPProviderIFace() { }

    //发送协议命令
    virtual void sendCmdData(unsigned char /*cmdId*/, const unsigned char */*data*/,
                             unsigned int /*len*/) { }

    //发送自检指令
    virtual void sendTEMPSelfTest() { }

    //获取测量状态
    virtual void sendProbeState() { }

    //发送定标数据。
    virtual void sendCalibrateData(int /*channel*/, int /*value*/) {}

    virtual void enterCalibrateState(void){}

    virtual void exitCalibrateState(void){}

    //通信中断。
    virtual bool isDisconnected() {return false;}
};
