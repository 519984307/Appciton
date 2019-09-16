/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/12
 **/

#pragma once
#include <QBasicTimer>
#include "BLMProvider.h"
#include "PrintProviderIFace.h"

class QTimerEvent;

/***************************************************************************************************
 * 打印机通信对象
 **************************************************************************************************/
class PRT48Provider : public BLMProvider, public PrintProviderIFace
{
    Q_OBJECT

public:
    //获取版本号
    virtual void sendVersion(void);

    // 立即停止。
    virtual void stop(void);

    // 版本查询。
    virtual void getVersionInfo(void);

    // 状态查询。
    virtual void getStatusInfo(void);

    // 设置打印速度。
    virtual void setPrintSpeed(PrintSpeed speed);

    // 空白走纸，单位:毫米(mm)。
    virtual void runPaper(unsigned char len);

    // 发送位图数据。
    virtual bool sendBitmapData(unsigned char *data, unsigned int len);

    // 对标。
    virtual void alignMarker(void);

    // 打印测试页。
    virtual void printTestPage(void);

    // 向打印机获取自检结果。
    virtual void startSelfTest(void);

    // 自检结果。
    virtual PrinterSelfTestResult selfTestResult(void);

    // 版本信息。
    virtual void versionInfo(QString &hwVer, QString &swVer, QString &protoVer);

    // 返回错误计数。
    unsigned errorCount(void);

    // 发送协议命令
    virtual void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    //修改波特率
    virtual void sendUART(unsigned int rate);

    virtual void flush(void);

    virtual PrinterProviderSignalSender *signalSender() const;

    PRT48Provider();
    ~PRT48Provider();

protected:
    // 通信中断。
    virtual void disconnected(void);
    virtual void reconnected(void);
    virtual void sendDisconnected();

// #if defined(Q_WS_QWS)
#if 1
    // 数据包解析
    void handlePacket(unsigned char *data, int len);

private:
    // 发送有效应答
    void _ack(unsigned char type);

    // 发送无效应答
    void _nack(unsigned char type);

    // 状态信息解析
    void _parseStatusInfo(const unsigned char *data, unsigned int len);

    // 版本信息解析
    void _parseVersionInfo(const unsigned char *data, unsigned int len);

    // 自检结果解析
    void _parseSelfTestResult(const unsigned char *data, unsigned int len);

    // 错误报告解析
    void _parseErrorReport(const unsigned char *data, unsigned int len);

    // 缓存状态解析
    void _parseBufStat(const unsigned char *data, unsigned int len);

    // 位图数据确认信息处理
    void _parseBitmapDataAck(const unsigned char *data, unsigned int len);

private:
    int _rllc(unsigned char *src, int len, unsigned char *dest, int destLen);
    QString _hwVer;
    QString _swVer;
    QString _protoVer;

    unsigned int _errorCount;
    unsigned char _prePacketNum;
    PrinterSelfTestResult _selfTestResult;
    PrinterProviderSignalSender *_sigSender;
#endif
};
