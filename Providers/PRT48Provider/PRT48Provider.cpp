/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/12
 **/

#include <QTimerEvent>
#include "Debug.h"
#include "PRT48Provider.h"
#include "SystemManager.h"
#include "Framework/Utility/Utility.h"
#include "Framework/ErrorLog/ErrorLog.h"
#include "Framework/ErrorLog/ErrorLogItem.h"
#include "AlarmSourceManager.h"
#include "RecorderManager.h"

// 命令类型
enum PrinterCommand
{
    // 有效应答/无效应答
    PRINTER_CMD_NACK = 0x00,
    PRINTER_CMD_ACK = 0x01,

    // 版本查询
    PRINTER_CMD_GET_VERSION = 0x10,
    PRINTER_CMD_VERSION_INFO = 0x11,

    // 状态查询
    PRINTER_CMD_GET_STATUS = 0x12,
    PRINTER_CMD_GET_STATUS_ACK = 0x13,

    // 设置打印速度
    PRINTER_CMD_SET_SPEED = 0x14,
    PRINTER_CMD_SET_SPEED_ACK = 0x15,

    // 立即停止
    PRINTER_CMD_STOP = 0x16,
    PRINTER_CMD_STOP_ACK = 0x17,

    // 空白走纸
    PRINTER_CMD_RUN_PAPER = 0x18,
    PRINTER_CMD_RUN_PAPER_ACK = 0x19,

    // 发送位图数据
    PRINTER_CMD_SEND_BITMAP_DATA = 0x1a,
    PRINTER_CMD_SEND_BITMAP_DATA_ACK = 0x1b,

    // 对标
    PRINTER_CMD_ALIGN_MARKER = 0x1c,
    PRINTER_CMD_ALIGN_MARKER_ACK = 0x1d,

    // 打印测试页
    PRINTER_CMD_PRINT_TEST_PAGE = 0x1e,
    PRINTER_CMD_PRINT_TEST_PAGE_ACK = 0x1f,

    // 获取自检结果
    PRINTER_CMD_GET_SELF_TEST_RESULT = 0x20,
    PRINTER_CMD_SELF_TEST_RESULT = 0x21,

    // 复位重启
    PRINTER_NOTIFY_START = 0x40,

    // 状态信息
    PRINTER_CMD_STATUS_INFO = 0x41,

    // 缓存状态
    PRINTER_CMD_BUF_STAT = 0x42,

    // 保活信息
    PRINTER_CMD_KEEP_ALIVE = 0x5b,

    // 错误报告
    PRINTER_CMD_ERROR_INFO = 0x76,  /* FIXME: PRT48 doesn't exist this info packet */

    //升级保活帧
    PRINTER_UPGRADE_ALIVE = 0xFE,
};

#define MAX_TIMEOUT     (30)    // 单位:秒

// #if defined(Q_WS_QWS)
#if 1

/***************************************************************************************************
 * 游程编码。
 * 参数：
 *      src: 原始数据；
 *      len：原始数据的长度；
 *      dest：带回数据的指针；
 *      destLen：带回数据的长度。
 * 返回：
 *      转换后的数据长度。
 **************************************************************************************************/
int PRT48Provider::_rllc(unsigned char *src, int len, unsigned char *dest, int destLen)
{
    int index = 0;
    unsigned char count = 1;

    // 保证数据空间的大小至少两倍。
    if ((destLen / len) < 2)
    {
        return index;
    }

    if (len == 1)
    {
        dest[0] = src[0];
        dest[1] = 1;
        index = index + 2;
        return index;
    }

    dest[0] = src[0];
    for (int i = 0; i < len - 1; i++)
    {
        if (src[i] == src[i + 1])
        {
            count++;
        }
        else
        {
            dest[index + 1] = count;
            index = index + 2;
            dest[index] = src[i + 1];
            count = 1;
        }
    }

    // 处理最后一个数据。
    if (src[len - 1] == src[len - 2])
    {
        dest[index + 1] = count;
        index = index + 2;
    }
    else
    {
        dest[index] = src[len - 1];
        dest[index + 1] = 1;
        index = index + 2;
    }

    return index;
}

/***************************************************************************************************
 * 发送有效应答
 **************************************************************************************************/
void PRT48Provider::_ack(unsigned char type)
{
    sendCmd(PRINTER_CMD_ACK, &type, sizeof(type));
}

/***************************************************************************************************
 * 发送无效应答
 **************************************************************************************************/
void PRT48Provider::_nack(unsigned char type)
{
    sendCmd(PRINTER_CMD_NACK, &type, sizeof(type));
}

/***************************************************************************************************
 * 状态信息解析
 **************************************************************************************************/
void PRT48Provider::_parseStatusInfo(const unsigned char *data, unsigned int len)
{
    if (NULL == data)
    {
        return;
    }

    unsigned char type = data[0];

    if (len != 2)
    {
        if (PRINTER_CMD_STATUS_INFO == type)
        {
            // 发送无效应答
            _nack(PRINTER_CMD_STATUS_INFO);
        }

        debug("Invalid status info packet!");
        return;
    }

    qDebug() << "Printer status" << hex << data[1];

    if (PRINTER_CMD_STATUS_INFO == type)
    {
        // 发送有效应答
        _ack(PRINTER_CMD_STATUS_INFO);
    }

    PrinterStatus status = PrinterStatus(data[1] & PRINTER_STAT_MASK);

    // 通知打印管理部件。
    QMetaObject::invokeMethod(_sigSender, "statusChanged", Q_ARG(PrinterStatus, status));
}

/***************************************************************************************************
 * 自检结果解析
 **************************************************************************************************/
void PRT48Provider::_parseSelfTestResult(const unsigned char *data, unsigned int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 2)
    {
        debug("Invalid self test result packet!");
        return;
    }

    _selfTestResult = PrinterSelfTestResult(data[1] & PRINTER_SELF_TEST_RESULT_MASK);

    if (0 == _selfTestResult)
    {
        systemManager.setPoweronTestResult(PRINTER72_SELFTEST_RESULT, SELFTEST_SUCCESS);
    }
    else
    {
        ErrorLogItem *item = new CriticalFaultLogItem();
        item->setName(QString("Printer selftest Fail:0x%1").arg(_selfTestResult, 0, 16));
        QString log("");
        PrinterSelfTestResult result = (PrinterSelfTestResult)(data[1] & PRINTER_SELF_TEST_RESULT_MASK);
        if (PRINTER_SELF_TEST_ADC_CAL_ERROR == result)
        {
            log += "ADC Calibration Error.\r\n";
        }

        if (PRINTER_SELF_TEST_TEMP_SENSOR_ERROR == result)
        {
            log += "Temperature sensor Error.\r\n";
        }

        if (PRINTER_SELF_TEST_PAPER_SENSOR_ERROR == result)
        {
            log += "Paper sensor Error.\r\n";
        }

        if (PRINTER_SELF_TEST_MOTOR_SENSOR_ERROR == result)
        {
            log += "Motor sensor Error.\r\n";
        }

        if (PRINTER_SELF_TEST_DATA_ERROR == result)
        {
            log += "Data transfer error between MCU and print head.\r\n";
        }

        if (!log.isEmpty())
        {
            item->setLog(log);
            item->setSubSystem(ErrorLogItem::SUB_SYS_PRINTER);
            item->setSystemState(ErrorLogItem::SYS_STAT_SELFTEST);
            item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
        }
        errorLog.append(item);

        systemManager.setPoweronTestResult(PRINTER72_SELFTEST_RESULT, SELFTEST_FAILED);
    }
}

/***************************************************************************************************
 * 错误报告解析
 **************************************************************************************************/
void PRT48Provider::_parseErrorReport(const unsigned char *data, unsigned int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 2)
    {
        _nack(PRINTER_CMD_ERROR_INFO);
        debug("Invalid error report packet!");
        return;
    }

    _ack(PRINTER_CMD_ERROR_INFO);

    // 通知打印管理部件。
    QMetaObject::invokeMethod(_sigSender, "error", Q_ARG(unsigned char, data[1]));
}

/***************************************************************************************************
 * 缓存状态解析
 **************************************************************************************************/
void PRT48Provider::_parseBufStat(const unsigned char *data, unsigned int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 2)
    {
        _nack(PRINTER_CMD_BUF_STAT);
        debug("Invalid buffer stat packet!");
        return;
    }

    _ack(PRINTER_CMD_BUF_STAT);

    // 通知打印管理部件。
    QMetaObject::invokeMethod(_sigSender, "bufferFull", Q_ARG(bool, (bool)data[1]));
}

/***************************************************************************************************
 * 位图数据确认信息处理
 **************************************************************************************************/
void PRT48Provider::_parseBitmapDataAck(const unsigned char *data, unsigned int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 2)
    {
        qdebug("Invalid bitmap data ack packet!\n");
        return;
    }

    unsigned char packetNum = data[1];

    // 判断是否丢包
    if ((qAbs(packetNum - _prePacketNum) % 255) > 1)
    {
        _errorCount++;
        qdebug("Data lost: packetNum = %d, prePacketNum = %d\n", packetNum, _prePacketNum);
    }

    // 更新位图数据包序列号
    _prePacketNum = packetNum;
}

/***************************************************************************************************
 * 打印机通信中断。
 **************************************************************************************************/
void PRT48Provider::disconnected()
{
    QMetaObject::invokeMethod(_sigSender, "connectionChanged", Q_ARG(bool, false));
    systemManager.setPoweronTestResult(PRINTER72_SELFTEST_RESULT, SELFTEST_FAILED);
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_PRINT);
    if (alarmSource)
    {
        alarmSource->clear();
        alarmSource->setOneShotAlarm(PRINT_ONESHOT_ALARM_COMMUNICATION_STOP, true);
    }
}

/***************************************************************************************************
 * 打印机通信恢复。
 **************************************************************************************************/
void PRT48Provider::reconnected()
{
    QMetaObject::invokeMethod(_sigSender, "connectionChanged", Q_ARG(bool, true));
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_PRINT);
    if (alarmSource)
    {
        alarmSource->clear();
        alarmSource->setOneShotAlarm(PRINT_ONESHOT_ALARM_COMMUNICATION_STOP, false);
    }
}

void PRT48Provider::sendDisconnected()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_PRINT);
    if (alarmSource && !alarmSource->isAlarmed(PRINT_ONESHOT_ALARM_COMMUNICATION_STOP))
    {
        alarmSource->setOneShotAlarm(PRINT_ONESHOT_ALARM_SEND_COMMUNICATION_STOP, true);
    }
}

/***************************************************************************************************
 * 数据包处理
 **************************************************************************************************/
void PRT48Provider::handlePacket(unsigned char *data, int len)
{
    BLMProvider::handlePacket(data, len);

    unsigned char type = data[0];
    switch (type)
    {
    case PRINTER_CMD_GET_STATUS_ACK:
    case PRINTER_CMD_STATUS_INFO:
        _parseStatusInfo(data, len);
        break;

    case PRINTER_CMD_VERSION_INFO:
        break;

    case PRINTER_CMD_SELF_TEST_RESULT:
        _parseSelfTestResult(data, len);
        break;

    case PRINTER_CMD_ERROR_INFO:
        _parseErrorReport(data, len);
        break;

    case PRINTER_NOTIFY_START:
    {
        _ack(type);
        QMetaObject::invokeMethod(_sigSender, "restart");

        systemManager.setPoweronTestResult(PRINTER72_SELFTEST_RESULT, SELFTEST_MODULE_RESET);
        ErrorLogItem *item = new ErrorLogItem();
        item->setName(QString("Printer Start"));
        errorLog.append(item);
        break;
    }

    case PRINTER_CMD_KEEP_ALIVE:
    {
        feed();
        if (_selfTestResult == PRINTER_SELF_TEST_NOT_PERFORMED)
        {
            startSelfTest();
        }
        break;
    }

    case PRINTER_CMD_BUF_STAT:
        _parseBufStat(data, len);
        break;

    case PRINTER_CMD_SEND_BITMAP_DATA_ACK:
        _parseBitmapDataAck(data, len);
        break;

    case PRINTER_CMD_SET_SPEED_ACK:
    case PRINTER_CMD_STOP_ACK:
    case PRINTER_CMD_RUN_PAPER_ACK:
    case PRINTER_CMD_ALIGN_MARKER_ACK:
    case PRINTER_CMD_PRINT_TEST_PAGE_ACK:
        /* TODO */
        break;

    default:
//                debug("Unknown message type!");
//                outHex(data, (int)len);
        break;
    }
}

/**************************************************************************************************
 * 获取版本号。
 *************************************************************************************************/
void PRT48Provider::sendVersion()
{
    sendCmd(PRINTER_CMD_GET_VERSION, NULL, 0);
}

/***************************************************************************************************
 * 立即停止
 **************************************************************************************************/
void PRT48Provider::stop(void)
{
    sendCmd(PRINTER_CMD_STOP, NULL, 0);
}

/***************************************************************************************************
 * 版本查询
 **************************************************************************************************/
void PRT48Provider::getVersionInfo(void)
{
    sendCmd(PRINTER_CMD_GET_VERSION, NULL, 0);
}

/***************************************************************************************************
 * 状态查询
 **************************************************************************************************/
void PRT48Provider::getStatusInfo(void)
{
    sendCmd(PRINTER_CMD_GET_STATUS, NULL, 0);
}

/***************************************************************************************************
 * 设置打印速度
 **************************************************************************************************/
void PRT48Provider::setPrintSpeed(PrintSpeed speed)
{
    unsigned char actualSpeed = speed;
    sendCmd(PRINTER_CMD_SET_SPEED, &actualSpeed, 1);
}

/***************************************************************************************************
 * 空白走纸，单位:毫米(mm)
 **************************************************************************************************/
void PRT48Provider::runPaper(unsigned char len)
{
    sendCmd(PRINTER_CMD_RUN_PAPER, &len, sizeof(len));
}

/***************************************************************************************************
 * 发送位图数据
 **************************************************************************************************/
bool PRT48Provider::sendBitmapData(unsigned char *data, unsigned int len)
{
    static unsigned char _packetNum = 0;

    unsigned char packet[2 * len + 1 + 1];  // RLE worst case data len (2n+1), plus a packet number
    ::memset(packet, 0, sizeof(packet));
    int retLen;

    packet[0] = _packetNum++;

#if 0
    retLen = _rllc(data, len, &packet[1], sizeof(packet) - 1);
#else
    Util::RunLengthEncode(reinterpret_cast<char *>(data), len, reinterpret_cast<char *>(&packet[1]), &retLen);
#endif

    return sendCmd(PRINTER_CMD_SEND_BITMAP_DATA, packet, retLen + 1);
}

/***************************************************************************************************
 * 对标
 **************************************************************************************************/
void PRT48Provider::alignMarker(void)
{
    sendCmd(PRINTER_CMD_ALIGN_MARKER, NULL, 0);
}

/***************************************************************************************************
 * 打印测试页
 **************************************************************************************************/
void PRT48Provider::printTestPage(void)
{
    sendCmd(PRINTER_CMD_PRINT_TEST_PAGE, NULL, 0);
}

/***************************************************************************************************
 * 向打印机获取自检结果。
 **************************************************************************************************/
void PRT48Provider::startSelfTest(void)
{
    sendCmd(PRINTER_CMD_GET_SELF_TEST_RESULT, NULL, 0);
}

/***************************************************************************************************
 * 返回自检结果。
 **************************************************************************************************/
PrinterSelfTestResult PRT48Provider::selfTestResult(void)
{
    return _selfTestResult;
}

/***************************************************************************************************
 * 返回错误计数。
 **************************************************************************************************/
unsigned PRT48Provider::errorCount(void)
{
    return _errorCount;
}

/***************************************************************************************************
 * 发送数据。
 **************************************************************************************************/
void PRT48Provider::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    sendCmd(cmdId, data, len);
}

/***************************************************************************************************
 * 修改波特率。
 **************************************************************************************************/
void PRT48Provider::sendUART(unsigned int rate)
{
    UartAttrDesc portAttr(rate, 8, 'N', 1);
    initPort(portAttr);
}

/***************************************************************************************************
 * flush cache data
 **************************************************************************************************/
void PRT48Provider::flush(void)
{
    uart->sync();
}


/**
 * @brief PRT72Provider::signalSender get the signal sender
 * @return
 */
PrinterProviderSignalSender *PRT48Provider::signalSender() const
{
    return _sigSender;
}

/***************************************************************************************************
 * 构造函数
 **************************************************************************************************/
PRT48Provider::PRT48Provider()
    : BLMProvider("PRT48")
    , _errorCount(0)
    , _prePacketNum(0)
    , _selfTestResult(PRINTER_SELF_TEST_NOT_PERFORMED)
    , _sigSender(new PrinterProviderSignalSender(this))
{
    // new mainboard support flow control, use block io
    // UartAttrDesc portAttr(460800, 8, 'N', 1, 0, FlOW_CTRL_HARD, false);
    UartAttrDesc portAttr(115200, 8, 'N', 1, 0);
    initPort(portAttr);
    needConnectedToParam = false;
}

/***************************************************************************************************
 * 析构函数
 **************************************************************************************************/
PRT48Provider::~PRT48Provider()
{
}

#elif defined(Q_WS_X11)
void PRT72Provider::disconnected(bool /*flag*/) {}
void PRT72Provider::sendVersion(void) {}
void PRT72Provider::stop(void) {}
void PRT72Provider::getVersionInfo(void) {}
void PRT72Provider::getStatusInfo(void) {}
void PRT72Provider::setPrintSpeed(PrintSpeed /*speed*/) {}
void PRT72Provider::runPaper(unsigned char /*len*/) {}
bool PRT72Provider::sendBitmapData(unsigned char */*data*/, unsigned int /*len*/)
{
    return true;
}
void PRT72Provider::alignMarker(void) {}
void PRT72Provider::printTestPage(void) {}
void PRT72Provider::startSelfTest(void) {}
void PRT72Provider::reset(void) {}
void PRT72Provider::flush(void) {}
PrinterSelfTestResult PRT72Provider::selfTestResult(void)
{
    return PRINTER_SELF_TEST_OK;
}
void PRT72Provider::versionInfo(QString &hwVer, QString &swVer, QString &protoVer)
{
    hwVer = "1.0";
    swVer = "1.0";
    protoVer = "1.0";
}
unsigned PRT72Provider::errorCount(void)
{
    return 0;
}
void PRT72Provider::sendCmdData(unsigned char /*cmdId*/, const unsigned char */*data*/, unsigned int /*len*/) { }
void PRT72Provider::sendUART(unsigned int /*rate*/) { }
PRT72Provider::PRT72Provider() : BLMProvider("PRT72Provider") {}
PRT72Provider::~PRT72Provider() {}

#endif
