/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/20
 **/

#pragma once
#include "PatientDefine.h"
#include "NIBPDefine.h"
#include <QString>

// 定义ECG数据提供对象需要实现的接口方法。
class NIBPProviderIFace
{
public:
    // 起停测量。
    virtual void startMeasure(PatientType /*type*/) { }
    virtual void stopMeasure(void) { }

    // 透传模式。
    virtual void setPassthroughMode(bool /*flag*/) { }

    // 获取自检状态。
    virtual void sendSelfTest(void) { }

    // 设置预充气压力值。
    virtual void setInitPressure(short /*pressure*/) { }

    // 设置智能压力使能。
    virtual void enableSmartPressure(bool /*enable*/) { }

    // 设置病人类型。
    virtual void setPatientType(unsigned char /*type*/) { }

    // 发送启动指令是否有该指令的应答。
    virtual bool needStartACK(void)
    {
        return false;
    }

    // 发送停止指令是否有该指令的应答。
    virtual bool needStopACK(void)
    {
        return false;
    }

    // 是否为错误数据包。
    virtual NIBPOneShotType isMeasureError(unsigned char */*packet*/)
    {
        return NIBP_ONESHOT_NONE;
    }

    // 发送获取结果请求。
    virtual void getResult(void) { }

    // 是否为结果包。
    virtual bool isResult(unsigned char */*packet*/, short &/*sys*/,
                          short &/*dia*/, short &/*map*/, short &/*pr*/, NIBPOneShotType &/*err*/)
    {
        return false;
    }

    // 进入维护模式。
    virtual void serviceEnter(bool /*enter*/) { }

    // 进入校准模式。
    virtual void serviceCalibrate(bool /*enter*/) { }

    // 校准点压力值输入
    virtual void servicePressurepoint(const unsigned char */*data*/, unsigned int /*len*/) { }

    // 进入压力计模式控制。
    virtual void serviceManometer(bool /*enter*/) { }

    // 进入压力操控模式。
    virtual void servicePressurecontrol(bool /*enter*/) { }

    // 压力控制（充气）
    virtual void servicePressureinflate(short /*pressure*/) { }
    virtual bool isServicePressureinflate(unsigned char */*packet*/)
    {
        return false;
    }

    // 放气控制
    virtual void servicePressuredeflate(void) { }

    // 气泵控制
    virtual void servicePump(bool /*enter*/, unsigned char /*pump*/) { }
    virtual bool isServicePump(unsigned char */*packet*/)
    {
        return false;
    }

    // 气阀控制
    virtual void serviceValve(bool /*enter*/) { }
    virtual bool isServiceValve(unsigned char */*packet*/)
    {
        return false;
    }

    // 压力数据，不是压力数据返回-1。
    virtual int serviceCuffPressure(unsigned char */*packet*/)
    {
        return -1;
    }

    // 进入校零模式
    virtual void serviceCalibrateZero(bool /*enter*/) { }
    virtual bool isServiceCalibrateZero(unsigned char */*packet*/)
    {
        return false;
    }

    /**
     * @brief setRawDataSend 设置原始数据发送开关
     */
    virtual void setRawDataSend(bool /*onOff*/){}

    // 校零
    virtual void servicePressureZero(void) { }
    virtual bool isServicePressureZero(unsigned char */*packet*/)
    {
        return false;
    }

    // 过压保护
    virtual void servicePressureProtect(bool /*enter*/) { }
    virtual bool isServicePressureProtect(unsigned char */*packet*/)
    {
        return false;
    }

    // 进入升级模式
    virtual void serviceUpgradeEnter(void) { }
    virtual bool isServiceUpgradeEnter(unsigned char */*packet*/)
    {
        return false;
    }

    // 数据发送启动
    virtual void serviceUpgradeSend(void) { }
    virtual bool isServiceUpgradeSend(unsigned char */*packet*/)
    {
        return false;
    }

    // 升级模式擦除Flash
    virtual void serviceUpgradeClear(void) { }
    virtual bool isServiceUpgradeClear(unsigned char */*packet*/)
    {
        return false;
    }

    // 升级模式硬件版本号
    virtual void serviceUpgradeRead(void) { }
    virtual bool isServiceUpgradeReade(unsigned char */*packet*/)
    {
        return false;
    }

    // 文件片段传输
    virtual void serviceUpgradeFile(unsigned char */*packet*/) { }
    virtual bool isServiceUpgradeFile(unsigned char */*packet*/)
    {
        return false;
    }

    // 升级结果
    virtual bool isServiceUpgradeFinish(unsigned char */*packet*/)
    {
        return false;
    }

    // 发送协议命令
    virtual void sendCmdData(unsigned char /*cmdId*/, const unsigned char */*data*/,
                             unsigned int /*len*/) { }

    // 转换错误码
    virtual unsigned char convertErrcode(unsigned char /*code*/) {return 0;}

    // 气动控制
    virtual void controlPneumatics(unsigned char /*pump*/,
                                   unsigned char /*controlValve*/,
                                   unsigned char /*dumpValve*/) { }

public:
    NIBPProviderIFace() {  }
    virtual ~NIBPProviderIFace() { }
};
