/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/6
 **/

#pragma once
#include <QString>
#include <QObject>
#include "PrintDefine.h"

// 工作状态定义
enum PrinterStatus
{
    PRINTER_STAT_NORMAL = 0x0,                              // 正常
    PRINTER_STAT_OUT_OF_PAPER = 0x01,                       // 缺纸
    PRINTER_STAT_OVER_HEATING = 0x02,                       // 过热
    PRINTER_STAT_OUT_OF_PAPER_OVER_HEATING = 0x03,          // 缺纸,过热
    PRINTER_STAT_FAULT = 0x04,                              // 故障
    PRINTER_STAT_OUT_OF_PAPER_FAULT = 0x05,                 // 缺纸,故障
    PRINTER_STAT_OVER_HEATING_FAULT = 0x06,                 // 过热.故障
    PRINTER_STAT_OUT_OF_PAPER_OVER_HEATING_FAULT = 0x07,    // 缺纸,过热,故障
    PRINTER_STAT_PRINTING = 0x08,                           // 正在打印
    PRINTER_STAT_MASK = 0x0F,                               // 掩码
    PRINTER_STAT_COMMUNICATION_STOP = 0xFF,                 // 通信中断
};

// 自检状态定义
enum PrinterSelfTestResult
{
    PRINTER_SELF_TEST_NOT_PERFORMED = -1,            // 未自检
    PRINTER_SELF_TEST_OK = 0x0,                      // 自检正常
    PRINTER_SELF_TEST_ADC_CAL_ERROR = 0x01,          // ADC校准错误
    PRINTER_SELF_TEST_TEMP_SENSOR_ERROR = 0x02,      // 温度传感器错误
    PRINTER_SELF_TEST_PAPER_SENSOR_ERROR = 0x04,     // 纸张传感器错误
    PRINTER_SELF_TEST_MOTOR_SENSOR_ERROR = 0x08,     // 电机传感器错误
    PRINTER_SELF_TEST_DATA_ERROR = 0x10,             // 数据传输故障
    PRINTER_SELF_TEST_RESULT_MASK = 0x1f             // 自检状态掩码
};

/**
 * @brief The PrinterProviderSignalSender class
 *        use to emit the print provider signal
 */
class PrinterProviderSignalSender: public QObject
{
    Q_OBJECT
public:
    explicit PrinterProviderSignalSender(QObject *parent = 0)
        :QObject(parent)
    {
    }

signals:
    /**
     * @brief restart emit when the provider restart
     */
    void restart();

    /**
     * @brief connectionChanged emit when the connection changed
     * @param isConnected connected or not
     */
    void connectionChanged(bool isConnected);

    /**
     * @brief statusChanged emit when the status changed
     * @param status new status
     */
    void statusChanged(PrinterStatus status);

    /**
     * @brief bufferFull emit when buffer status changed
     * @param isFull is full or not
     */
    void bufferFull(bool isFull);

    /**
     * @brief error emit when in error
     * @param err error code
     */
    void error(unsigned char err);
};

/***************************************************************************************************
 * 定义打印机需要实现的接口方法
 **************************************************************************************************/
class PrintProviderIFace
{
public:
    PrintProviderIFace() {}
    virtual ~PrintProviderIFace(void) {}

    // 立即停止
    virtual void stop(void) = 0;

    // 版本查询
    virtual void getVersionInfo(void) = 0;

    // 状态查询
    virtual void getStatusInfo(void) = 0;

    // 设置打印速度
    virtual void setPrintSpeed(PrintSpeed speed) = 0;

    // 空白走纸，单位:毫米(mm)
    virtual void runPaper(unsigned char len) = 0;

    // 发送位图数据
    virtual bool sendBitmapData(unsigned char *data, unsigned int len) = 0;

    // 对标
    virtual void alignMarker(void) = 0;

    // 打印测试页
    virtual void printTestPage(void) = 0;

    // 启动自检
    virtual void startSelfTest(void) = 0;

    // 自检结果
    virtual PrinterSelfTestResult selfTestResult(void) = 0;

    // 版本信息
    virtual void versionInfo(QString &hwVer, QString &swVer, QString &protoVer) = 0;

    //发送协议命令
    virtual void sendCmdData(unsigned char /*cmdId*/, const unsigned char */*data*/,
                             unsigned int /*len*/) { }

    virtual void sendUART(unsigned int /*rate*/) { }

    virtual void flush(void) {}

    virtual PrinterProviderSignalSender* signalSender() const {return NULL;}
};
