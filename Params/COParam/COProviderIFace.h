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
    virtual bool isMeasureCtrl(unsigned char */*packet*/) {return false;}

    // CO 测量时间间隔设置
    virtual void setInterval(COMeasureInterval /*interval*/){}
    virtual bool isSetInterval(unsigned char */*packet*/) {return false;}

    // Ti 输入模式设置
    virtual void setInputMode(COTiMode /*inputmode*/, unsigned short /*watertemp*/){}
    virtual bool isSetInputMode(unsigned char */*packet*/) {return false;}

    // 注射液体积设定
    virtual void setVolume(unsigned char /*volume*/){}
    virtual bool isSetVolume(unsigned char */*packet*/) {return false;}

    // 漂浮导管系数设定
    virtual void setDuctRatio(unsigned short /*ratio*/){}
    virtual bool isSetDuctRatio(unsigned char */*packet*/) {return false;}

    // 血液动力计算参数设置
    virtual void setHemodymicParam(void){}
    virtual bool isSetHemodymicParam(unsigned char */*packet*/) {return false;}

    // 血液动力学计算
    virtual void hemodymicCalc(void){}
    virtual bool isHemodymicCalc(unsigned char */*packet*/) {return false;}

public:
    COProviderIFace() {  }
    virtual ~COProviderIFace() { }
};
