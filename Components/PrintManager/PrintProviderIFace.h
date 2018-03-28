#pragma once
#include <QString>
#include <QObject>
#include "PrintDefine.h"

// 工作状态定义
enum PrinterStatus
{
    PRINTER_STAT_NORMAL = 0x0,                     // 正常
    PRINTER_STAT_DOOR_OPENED = 0x01,               // 仓门未关闭
    PRINTER_STAT_OUT_OF_PAPER = 0x02,              // 缺纸 (若仓门未关与缺纸状态同时存在，则仓门状态优先级高)
    PRINTER_STAT_DOOR_OUT_OF_PAPER = 0x03,         // 仓门未关闭、缺纸
    PRINTER_STAT_OVER_HEATING = 0x04,              // 过热 (打印头温度)
    PRINTER_STAT_DOOR_OPEND_OVER_HEATING = 0x05,   // 仓门未关闭、过热（打印头温度）
    PRINTER_STAT_OUT_OF_PAPER_OVER_HEATING = 0x06, // 缺纸、过热（打印头温度）
    PRINTER_STAT_DOOR_OPENED_OUT_OF_PAPER_OVER_HEATING = 0x07, // 仓门未关闭、缺纸、过热（打印头温度）
    PRINTER_STAT_TEMP_OVERRUN = 0x08,              // 过热 (电机温度)
    PRINTER_STAT_DOOR_OPEND_TEMP_OVERRUN = 0x09,   // 仓门未关闭、过热（电机温度）
    PRINTER_STAT_OUT_OF_PAPER_TEMP_OVERRUN = 0x0A, // 缺纸、过热
    PRINTER_STAT_DOOR_OPENED_OUT_OF_PAPER_TEMP_OVERRUN = 0x0B, // 仓门未关闭、缺纸、过热（电机温度）
    PRINTER_STAT_OVER_HEATING_TEMP_OVERRUN = 0x0C, // 过热（打印头温度、电机温度）
    PRINTER_STAT_DOOR_OPENED_OVER_HEATING_TEMP_OVERRUN = 0x0D, // 仓门未关闭、过热(打印头温度、电机温度)
    PRINTER_STAT_OUT_OF_PAPER_OVER_HEATING_TEMP_OVERRUN = 0x0E, // 缺纸、过热(打印头温度、电机温度)
    PRINTER_STAT_ALL = 0x0F,                       // 仓门未关闭、缺纸、过热(打印头过热、电机过热)
    PRINTER_STAT_MASK = 0x0F                       // 工作状态掩码
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
};
