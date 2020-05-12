/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by lianghuan <lianghuan@blmed.cn>, 2020/6/8
 **/


#pragma once
#include "PatientDefine.h"
#include "IBPDefine.h"
#include <QString>

// 定义IBP数据提供对象需要实现的接口方法。
class IBPProviderIFace
{
public:
    // 模块信息查询
    virtual void moduleInfo(void){}
    virtual bool isModuleInfo(unsigned char */*packet*/) {return false;}

    // 模块自检结果查询
    virtual void moduleSelfCheck(void){}
    virtual bool isModuleSelfCheck(unsigned char */*packet*/) {return false;}

    // IBP 平均时间设置
    virtual void setAvergTime(IBPSignalInput /*IBP1/2*/, unsigned char /*time*/){}
    virtual bool isSetAvergTime(unsigned char */*packet*/) {return false;}

    // IBP校准、校零设置
    virtual void setZero(IBPSignalInput /*IBP1/2*/, IBPCalibration /*calibration*/, unsigned short /*pressure*/){}
    virtual bool isSetZero(unsigned char */*packet*/) {return false;}

    // IBP滤波设置
    virtual void setFilter(IBPSignalInput /*IBP1/2*/, IBPFilterMode /*filter*/){}
    virtual bool isSetFilter(unsigned char */*packet*/) {return false;}

    // IBP表明设置
    virtual void setIndicate(IBPLabel /*pressurenameIBP1*/, IBPLabel /*pressurenameIBP2*/,
                             IBPAuxiliarySet /*auxiliarysetIBP1*/, IBPAuxiliarySet /*auxiliarysetIBP2*/){}
    virtual bool isSetIndicate(unsigned char */*packet*/) {return false;}

    // IBP 校零/校准时间设定
    virtual void setTimeZero(IBPSignalInput /*IBP1/2*/, IBPCalibration /*calibration*/,
                             unsigned char /*second*/, unsigned char /*minute*/,
                             unsigned char /*hour*/, unsigned char /*day*/,
                             unsigned char /*month*/, unsigned char /*year*/){}
    virtual bool isSetTimeZero(unsigned char */*packet*/) {return false;}

    // 获取波形的采样速度。
    virtual int getIBPWaveformSample(void) = 0;

    // 获取波形基线
    virtual int getIBPBaseLine(void) = 0;

    // 获取最大的波形值
    virtual int getIBPMaxWaveform(void) = 0;

    /**
     * @brief getIBPModuleType get the provide type
     * @return the type of the provider
     */
    virtual IBPModuleType getIBPModuleType() const = 0;

public:
    IBPProviderIFace() {  }
    virtual ~IBPProviderIFace() { }
};
