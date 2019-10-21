/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/12
 **/

#pragma once
#include "Provider.h"
#include "NIBPProviderIFace.h"

struct SuntechCMD
{
    SuntechCMD() : cmdLength(0)
    {
        memset(cmd, 0, 10);
    }
    unsigned char cmd[10];
    unsigned int cmdLength;
};

class QTimer;
class SuntechProvider: public Provider, public NIBPProviderIFace
{
    Q_OBJECT

public:
    virtual bool attachParam(Param &param);
    virtual void dataArrived(void);

    virtual void sendVersion(void);

public:
    // 起停测量。
    virtual void startMeasure(PatientType /*type*/);
    virtual void stopMeasure(void);

    // 设置预充气压力值。
    virtual void setInitPressure(short pressure);

    // 发送启动指令是否有该指令的应答。
    virtual bool needStartACK(void);

    // 发送停止指令是否有该指令的应答。
    virtual bool needStopACK(void);

    //校准点压力值输入
    virtual void servicePressurepoint(const unsigned char *data, unsigned int len);

    // 获取校准结果
    void getCalibrateResult();

    // 是否为错误数据包。
    virtual NIBPOneShotType isMeasureError(unsigned char *packet);

    // 发送获取结果请求。
    virtual void getResult(void);

    // 是否为结果包。
    virtual bool isResult(unsigned char *packet, short &sys,
                          short &dia, short &map, short &pr, NIBPOneShotType &err);

    // 获取模块信息
    void getReturnString(void);

    // 转换错误码
    virtual unsigned char convertErrcode(unsigned char code);

    // 气动控制
    virtual void controlPneumatics(unsigned char pump,
                                   unsigned char controlValve,
                                   unsigned char dumpValve);
    /**
     * @brief sendSelfTest 发送自检命令
     */
    virtual void sendSelfTest();

    // 构造与析构。
    SuntechProvider();
    ~SuntechProvider();

protected:
    virtual void disconnected(void);
    virtual void reconnected(void);

private slots:
    void _getCuffPressure(void);
    void _sendCMD(void);
    void _sendInitval(void);
private:
    void _handlePacket(unsigned char *data, int len);

    void _sendCmd(const unsigned char *data, unsigned int len);

    // 协议数据校验
    unsigned char _calcCheckSum(const unsigned char *data, int len);

    void _sendReset(void);

    bool _NIBPStart;

    static const int _minPacketLen = 3;      // 最小数据包长度。

    int _flagStartCmdSend;
    int _pressure;

    QTimer *_presssureTimer;
    QTimer *_cmdTimer;

    bool _isModuleDataRespond;              // 是否为版本信息回复
    bool _isCalibrationRespond;             // 是否为校准回复
    bool _powerOnSelfTest;                  // 开机自检

    QList<SuntechCMD> list;
};
