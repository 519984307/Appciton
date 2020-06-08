/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/8
 **/

#pragma once
#include "PatientDefine.h"
#include "CODefine.h"
#include <QString>

// 定义CO数据提供对象需要实现的接口方法。
class COProviderIFace
{
public:
    // CO 测量控制
    virtual void measureCtrl(COInstCtl /*instctl*/){}

    // CO 测量时间间隔设置
    virtual void setInterval(COMeasureInterval /*interval*/){}

    // Ti 输入模式设置
    virtual void setInputMode(COTiMode /*inputmode*/, unsigned short /*watertemp*/){}

    // 注射液体积设定
    virtual void setVolume(unsigned char /*volume*/){}

    // 漂浮导管系数设定
    virtual void setDuctRatio(unsigned short /*ratio*/){}

    // 血液动力计算参数设置
    virtual void setHemodymicParam(void){}

    // 血液动力学计算
    virtual void hemodymicCalc(void){}

public:
    COProviderIFace() {  }
    virtual ~COProviderIFace() { }
};
