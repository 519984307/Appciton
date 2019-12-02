/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/8
 **/

#include "SuntechProvider.h"
#include "NIBPParam.h"
#include "NIBPAlarm.h"
#include "Debug.h"
#include <QString>
#include <QTimer>
#include "NIBPMonitorStateDefine.h"
#include "PatientManager.h"
#include "AlarmSourceManager.h"

#define     HOST_START_BYTE           0x3A     // receive packet header :
#define     MODULE_START_BYTE         0x3E     // send packet header >

// RESPONSES

#define     MODULE_ACK                0x04      // 模块应答

#define     MODULE_RECEIVED           0x4F      // “O”
#define     MODULE_EXECUTED           0x4B      // "K"
#define     MODULE_ABORT              0x41      // "A"
#define     MODULE_BUSY               0x42      // "B"
#define     MODULE_ERROR              0x45      // "E"
#define     MODULE_UNDIFINE           0x4E      // "N"
#define     MODULE_UNRECOGNIZABLE     0x44      // "D"
#define     MODULE_SLEEP              0x53      // "S"
#define     MODULE_TIMEOUT            0x54      // "T"

#define     SUNTECH_RSP_CUFF_PRESSURE           0x05        // 当前压力
#define     SUNTECH_RSP_GET_MEASUREMENT         0x18        // 测量结果
#define     SUNTECH_RSP_GET_RETURN_STRING       0x43        // 版本信息
#define     SUNTECH_RSP_GET_CALIBRATE_RESULT    0x06        // 校准结果


// COMMANDS

#define     SUNTECH_CMD_SEND_START                  0x79
#define     SUNTECH_CMD_SET_INITIAL_INFIAL          0X17      // DATA=I0
#define     SUNTECH_CMD_START_ADU_BP                0X20      // 成人
#define     SUNTECH_CMD_START_PED_BP                0X87      // 小儿
#define     SUNTECH_CMD_START_NEO_BP                0X28      // 新生儿
#define     SUNTECH_CMD_ABORT_BP                    0x01      // 停止测量
#define     SUNTECH_CMD_GET_CUFF_PRESSURE           0x05      // 获取当前压力
#define     SUNTECH_CMD_GET_BP_DATA                 0x03      // 获取测量结果
#define     SUNTECH_CMD_GET_MODULE_DATA             0x00
#define     SUNTECH_CMD_GET_RETURN_STRING           0x79      // DATA=0x02 0x40
#define     SUNTECH_CMD_SET_SLEEP_MODE              0x81      // DATA=B0 B1 B2 B3
#define     SUNTECH_CMD_CONTROL_PNEUMATICS          0x0C      // DATA=B0 B1 B2
#define     SUNTECH_CMD_CALIBRATE_TRANSDUCER        0X04      // DATA=B0
#define     SUNTECH_CMD_GET_RETURN_CODE             0x79      // DATA=0x02 0x03
#define     SUNTECH_CMD_STATUS                      0x79      // DATA=0x10 0x03
#define     SUNTECH_CMD_STATUS_DATA_FIRST           0x10
#define     SUNTECH_CMD_STATUS_DATA_SECOND          0x03
#define     SUNTECH_A_PLUS_CMD_STATUS_DATA_FIRST    0x11      // Suntech A+ STATUS Data
#define     SUNTECH_A_PLUS_CMD_STATUS_DATA_SECOND   0x00
#define     SUNTECH_CMD_RESET                       0x8A
#define     SUNTECH_CMD_START_NEONATE_BP            0x28      // 新生儿
#define     SUNTECH_CMD_VENOUS_STASIS               0x86      // DATA=B0 B1 B2

enum ErrCode
{
    SUNTECH_ERRCODE_WEAK_SIGNAL                 = 0x01,         // 弱或者没有信号
    SUNTECH_ERRCODE_ERRATIC_SIGNAL              = 0x02,         // 人为或者不稳定信号
    SUNTECH_ERRCODE_OUT_OF_RANGE                = 0x03,         // BP值超出范围
    SUNTECH_ERRCODE_MEASURE_TIMEOUT             = 0x04,         // 超出测量时间范围
    SUNTECH_ERRCODE_PNEUMATIC_BLOCK             = 0x55,         // 气动堵塞
    SUNTECH_ERRCODE_BP_READ_TERMINATE           = 0x56,         // BP读取被用户终止
    SUNTECH_ERRCODE_LOOSE_CUFF                  = 0x57,         // 充气超时、漏气或袖口松脱
    SUNTECH_ERRCODE_SAFETY_TIMEOUT              = 0x58,         // 安全超时
    SUNTECH_ERRCODE_CUFF_OVERPRESSURE           = 0x59,         // 袖带超压
    SUNTECH_ERRCODE_HARDWORD_PROBLEM            = 0x5A,         // 电源超出范围或其他硬件问题
    SUNTECH_ERRCODE_PERMISSION_PROBLEM          = 0x5B,         // 许可问题，如安全超控装置安装或自动归零超出范围
    SUNTECH_ERRCODE_TRANSDUCTER_OUT_OF_RANGE    = 0x61,         // 传感器超出范围
    SUNTECH_ERRCODE_EEPROM_CALIBR_FAILURE       = 0x63,         // EEPROM校准数据失败
};

#define SUNTECH_VERSION "LX"
#define SUNTECH_A_PLUS_VERSION "SB"

/**************************************************************************************************
 * 发送复位命令。
 *************************************************************************************************/
void SuntechProvider::_sendReset(void)
{
    unsigned char cmd[1] = {0};
    cmd[0] = SUNTECH_CMD_RESET;
    _sendCmd(cmd, 1);
}

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool SuntechProvider::attachParam(Param *param)
{
    if (param->getParamID() == PARAM_NIBP)
    {
        nibpParam.setProvider(this);
        Provider::attachParam(param);
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 读取数据。
 *************************************************************************************************/
void SuntechProvider::dataArrived(void)
{
    readData();
    if (ringBuff.dataSize() < _minPacketLen)
    {
        return;
    }

    // the max number of the every data pack is 67. set the buff size as 84.
    unsigned char buff[84];
    while (ringBuff.dataSize() >= _minPacketLen)
    {
        if (ringBuff.at(0) != MODULE_START_BYTE)
        {
            // debug("discard (%s:%d)\n", qPrintable(getName()), ringBuff.at(0));
            ringBuff.pop(1);
            continue;
        }

        int len = ringBuff.at(1);
        if ((len <= 0) || (len > 570))
        {
            ringBuff.pop(1);
            break;
        }

        if (len > ringBuff.dataSize())  // 数据还不够，继续等待。
        {
            break;
        }

        // 数据包不会超过packet长度，当出现这种情况说明发生了不可预料的错误，直接丢弃该段数据。
        if (len > static_cast<int>(sizeof(buff)))
        {
            ringBuff.pop(1);
            continue;
        }

        // 将数据包读到buff中。
        for (int i = 0; i < len; i++)
        {
            buff[i] = ringBuff.at(0);
            ringBuff.pop(1);
        }

        if (buff[len - 1] == _calcCheckSum(buff, len - 1))
        {
            _handlePacket(&buff[1], len - 2);
        }
        else
        {
            outHex(buff, len);
            debug("FCS error (%s)\n", qPrintable(getName()));
            ringBuff.pop(1);
        }
    }
}

void SuntechProvider::sendVersion()
{
    unsigned char cmd = SUNTECH_CMD_GET_MODULE_DATA;
    _sendCmd(&cmd, 1);
    _isModuleDataRespond = true;
}

/**************************************************************************************************
 * 启动测量。
 *************************************************************************************************/
void SuntechProvider::startMeasure(PatientType type)
{
    unsigned char cmd = 0;
    if (type == PATIENT_TYPE_ADULT)  // 如果是成人模式
    {
        cmd = SUNTECH_CMD_START_ADU_BP;
    }
    else if (type == PATIENT_TYPE_PED)  //  如果是少儿模式
    {
        cmd = SUNTECH_CMD_START_PED_BP;
    }
    else if (type == PATIENT_TYPE_NEO)  //  如果是新生儿模式
    {
        cmd = SUNTECH_CMD_START_NEO_BP;
    }

    _sendCmd(&cmd, 1);

    _NIBPStart = true;
    _flagStartCmdSend = 1;

    if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
    {
        if (nibpParam.isSTATFirst())
        {
            nibpParam.setSTATFirst(false);
        }
    }
}

/**************************************************************************************************
 * 停止测量。
 *************************************************************************************************/
void SuntechProvider::stopMeasure(void)
{
    unsigned char cmd[3] = {0};

    cmd[0] = SUNTECH_CMD_SEND_START;
    cmd[1] = SUNTECH_CMD_ABORT_BP;
    cmd[2] = 0x00;

    _sendCmd(cmd, 3);

    _flagStartCmdSend = 1;
}

/**************************************************************************************************
 * 设置初始化压力。
 *************************************************************************************************/
void SuntechProvider::setInitPressure(short pressure)
{
    if (_NIBPStart)                // 模块同时发送停止测量跟设置初始压力会导致收不到停止命令回复
    {
        _pressure = pressure;
        QTimer::singleShot(500, this, SLOT(_sendInitval()));
        return;
    }
    unsigned char cmd[3] = {0};

    cmd[0] = SUNTECH_CMD_SET_INITIAL_INFIAL;
    cmd[1] = pressure & 0xFF;
    cmd[2] = (pressure & 0xFF00) >> 8;

    _sendCmd(cmd, 3);
}

/**************************************************************************************************
 * 发送启动指令是否有该指令的应答。
 *************************************************************************************************/
bool SuntechProvider::needStartACK(void)
{
    return true;
}

/**************************************************************************************************
 * 发送停止指令是否有该指令的应答。
 *************************************************************************************************/
bool SuntechProvider::needStopACK(void)
{
    return true;
}

void SuntechProvider::servicePressurepoint(const unsigned char *data, unsigned int len)
{
    _isCalibrationRespond = true;
    unsigned char cmd[2] = {0};
    cmd[0] = SUNTECH_CMD_CALIBRATE_TRANSDUCER;
    int pressure = data[0] | (data[1] << 8);
    if (pressure)
    {
        cmd[1] = 0x01;
    }
    else
    {
        cmd[1] = 0x00;
    }
    len = 2;
    _sendCmd(cmd, len);
}

void SuntechProvider::getCalibrateResult()
{
    unsigned char cmd[3] = {0};
    cmd[0] = SUNTECH_CMD_GET_RETURN_CODE;
    cmd[1] = 0x02;
    cmd[2] = 0x03;
    _sendCmd(cmd, 3);
}

/**************************************************************************************************
 * 是否为错误数据包。
 *************************************************************************************************/
NIBPOneShotType SuntechProvider::isMeasureError(unsigned char *)
{
    return NIBP_ONESHOT_NONE;
}

/**************************************************************************************************
 * 获取结果请求。
 *************************************************************************************************/
void SuntechProvider::getResult(void)
{
    unsigned char cmd[3] = {0};

    cmd[0] = SUNTECH_CMD_SEND_START;
    cmd[1] = SUNTECH_CMD_GET_BP_DATA;
    cmd[2] = 0x00;

    _sendCmd(cmd, 3);
}

/**************************************************************************************************
 * 是否为结果包。
 *************************************************************************************************/
bool SuntechProvider::isResult(unsigned char *packet,
                               short *sys, short *dia, short *map, short *pr, NIBPOneShotType *err)
{
    if (packet[0] != 0x18)
    {
        return false;
    }

    *err = NIBP_ONESHOT_NONE;

    // 测量有错误，获取错误码。
    if (packet[19] != 0x00)
    {
        switch (packet[19])
        {
        case 0x01:
            *err = NIBP_ONESHOT_ALARM_SIGNAL_WEAK;
            break;
        case 0x02:
            *err = NIBP_ONESHOT_ALARM_EXCESSIVE_MOVING;
            break;
        case 0x03:
            *err = NIBP_ONESHOT_ALARM_MEASURE_OVER_RANGE;
            break;
        case 0x04:
            *err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT;
            break;
        case 0x85:
            *err = NIBP_ONESHOT_ALARM_PNEUMATIC_BLOCKAGE;
            break;
        case 0x87:
            *err = NIBP_ONESHOT_ALARM_CUFF_ERROR;
            break;
        case 0x88:
            *err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT;
            break;
        case 0x89:
            *err = NIBP_ONESHOT_ALARM_CUFF_OVER_PRESSURE;
            break;
        case 0x90:
            *err = NIBP_ONESHOT_ALARM_HARDWARE_ERROR;
            break;
        case 0x91:
            *err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT;
            break;
        case 0x97:
            *err = NIBP_ONESHOT_ALARM_TRANSDUCER_OVER_RANGE;
            break;
        case 0x99:
            *err = NIBP_ONESHOT_ALARM_EEPROM_FAILURE;
            break;
        default:
            break;
        }
        return true;
    }

    *sys = static_cast<int>((packet[2] << 8) + packet[1]);
    *dia = static_cast<int>((packet[4] << 8) + packet[3]);
    *map = static_cast<int>((packet[18] << 8) + packet[17]);
    *pr = static_cast<int>((packet[16] << 8) + packet[15]);

    return true;
}

void SuntechProvider::getReturnString()
{
    unsigned char cmd[3] = {0};
    cmd[0] = SUNTECH_CMD_GET_RETURN_STRING;
    cmd[1] = 0x02;
    cmd[2] = 0x40;
    _sendCmd(cmd, 3);
}

unsigned char SuntechProvider::convertErrcode(unsigned char code)
{
    unsigned char err;
    switch (code)
    {
    case SUNTECH_ERRCODE_WEAK_SIGNAL:
        err = NIBP_ONESHOT_ALARM_SIGNAL_WEAK;
        break;
    case SUNTECH_ERRCODE_ERRATIC_SIGNAL:
        err = NIBP_ONESHOT_ALARM_EXCESSIVE_MOVING;
        break;
    case SUNTECH_ERRCODE_OUT_OF_RANGE:
        err = NIBP_ONESHOT_ALARM_MEASURE_OVER_RANGE;
        break;
    case SUNTECH_ERRCODE_MEASURE_TIMEOUT:
        err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT;
        break;
    case SUNTECH_ERRCODE_PNEUMATIC_BLOCK:
        err = NIBP_ONESHOT_ALARM_PNEUMATIC_BLOCKAGE;
        break;
    case SUNTECH_ERRCODE_LOOSE_CUFF:
        err = NIBP_ONESHOT_ALARM_CUFF_ERROR;
        break;
    case SUNTECH_ERRCODE_SAFETY_TIMEOUT:
        err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT;
        break;
    case SUNTECH_ERRCODE_CUFF_OVERPRESSURE:
        err = NIBP_ONESHOT_ALARM_CUFF_OVER_PRESSURE;
        break;
    case SUNTECH_ERRCODE_HARDWORD_PROBLEM:
        err = NIBP_ONESHOT_ALARM_HARDWARE_ERROR;
        break;
    case SUNTECH_ERRCODE_PERMISSION_PROBLEM:
        err = NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT;
        break;
    case SUNTECH_ERRCODE_TRANSDUCTER_OUT_OF_RANGE:
        err = NIBP_ONESHOT_ALARM_TRANSDUCER_OVER_RANGE;
        break;
    case SUNTECH_ERRCODE_EEPROM_CALIBR_FAILURE:
        err = NIBP_ONESHOT_ALARM_EEPROM_FAILURE;
        break;
    default:
        err = NIBP_ONESHOT_NONE;
        break;
    }
    return err;
}

void SuntechProvider::controlPneumatics(unsigned char pump, unsigned char controlValve, unsigned char dumpValve)
{
    unsigned char cmd[4] = {0};
    cmd[0] = SUNTECH_CMD_CONTROL_PNEUMATICS;
    cmd[1] = pump;
    cmd[2] = controlValve;
    cmd[3] = dumpValve;
    _sendCmd(cmd, 4);
}

void SuntechProvider::sendSelfTest()
{
    unsigned char cmd[3] = {0};
    if (_suntechModel == SUNTECH_A_PLUS_MODEL)
    {
        cmd[0] = SUNTECH_CMD_STATUS;
        cmd[1] = SUNTECH_A_PLUS_CMD_STATUS_DATA_FIRST;
        cmd[2] = SUNTECH_A_PLUS_CMD_STATUS_DATA_SECOND;
    }
    else
    {
        cmd[0] = SUNTECH_CMD_STATUS;
        cmd[1] = SUNTECH_CMD_STATUS_DATA_FIRST;
        cmd[2] = SUNTECH_CMD_STATUS_DATA_SECOND;
    }
    _sendCmd(cmd, 3);
    _powerOnSelfTest = true;
}
/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SuntechProvider::SuntechProvider() :
    Provider("SUNTECH_NIBP"), NIBPProviderIFace(),
    _NIBPStart(false), _flagStartCmdSend(-1), _pressure(-1),
    _presssureTimer(NULL), _cmdTimer(NULL), _isModuleDataRespond(false),
    _isCalibrationRespond(false), _powerOnSelfTest(true),
    _suntechModel(SUNTECH_MODEL)
{
    UartAttrDesc portAttr(9600, 8, 'N', 1);
    initPort(portAttr);
    setDisconnectThreshold(5);

    // 发送版本
    sendVersion();

    _presssureTimer = new QTimer();
    _presssureTimer->setInterval(200);

    _cmdTimer = new QTimer();
    _cmdTimer->setInterval(100);
    _cmdTimer->start();
    connect(_presssureTimer, SIGNAL(timeout()), this, SLOT(_getCuffPressure()));
    connect(_cmdTimer, SIGNAL(timeout()), this, SLOT(_sendCMD()));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SuntechProvider::~SuntechProvider()
{
    if (NULL != _presssureTimer)
    {
        delete _presssureTimer;
        _presssureTimer = NULL;
    }

    if (NULL != _cmdTimer)
    {
        delete _cmdTimer;
        _cmdTimer = NULL;
    }
}

void SuntechProvider::disconnected()
{
    nibpParam.connectedFlag(false);
    nibpParam.setConnected(false);
}

void SuntechProvider::reconnected()
{
    nibpParam.connectedFlag(true);
    nibpParam.setConnected(true);
}

/**************************************************************************************************
 * get cuff pressure。
 *************************************************************************************************/
void SuntechProvider::_getCuffPressure()
{
    unsigned char cmd[3] = {0};

    cmd[0] = SUNTECH_CMD_SEND_START;
    cmd[1] = SUNTECH_CMD_GET_CUFF_PRESSURE;
    cmd[2] = 0x00;

    _sendCmd(cmd, 3);
}

void SuntechProvider::_sendCMD()
{
    if (!list.empty())
    {
        SuntechCMD cmd = list.takeFirst();
        writeData(cmd.cmd, cmd.cmdLength);
    }
}

void SuntechProvider::_sendInitval()
{
    unsigned char cmd[3] = {0};
    cmd[0] = SUNTECH_CMD_SET_INITIAL_INFIAL;
    cmd[1] = _pressure & 0xFF;
    cmd[2] = (_pressure & 0xFF00) >> 8;
    _sendCmd(cmd, 3);
}

static NIBPMeasureResultInfo getMeasureResultInfo(unsigned char *data)
{
    NIBPMeasureResultInfo info;
    short t = static_cast<short>(data[0] + (data[1] << 8));
    info.sys = t;
    t = static_cast<short>(data[2] + (data[3] << 8));
    info.dia = t;
    t = static_cast<short>(data[16] + (data[17] << 8));
    info.map = t;
    t = static_cast<short>(data[14] + (data[15] << 8));
    info.pr = t;
    t = static_cast<short>(data[18]);
    info.errCode = t;
    return info;
}

/**************************************************************************************************
 * 数据处理。
 *************************************************************************************************/
void SuntechProvider::_handlePacket(unsigned char *data, int len)
{
    Q_UNUSED(len)
    if (!isConnectedToParam)
    {
        return;
    }

    if (!isConnected)
    {
        nibpParam.setConnected(true);
    }

    switch (data[0])
    {
    case MODULE_ACK:
        // "O"
        if (data[1] == MODULE_RECEIVED)
        {
            if (_flagStartCmdSend == 1)
            {
                // 启动测量
                if (_NIBPStart)
                {
                    nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_START_MEASURE, NULL, 0);
                }
                _flagStartCmdSend = 2;
            }
        }

        // "A"
        else if (data[1] == MODULE_ABORT)
        {
            if (_flagStartCmdSend == 1)
            {
                _NIBPStart = false;
                _flagStartCmdSend = 2;
            }
        }

        // "AK"  "OK"
        else if (data[1] == MODULE_EXECUTED)
        {
            if (_isModuleDataRespond)
            {
                _isModuleDataRespond = false;
                getReturnString();
            }
            else if (_isCalibrationRespond)
            {
                _isCalibrationRespond = false;
                getCalibrateResult();
            }
            else if (_flagStartCmdSend == 2)
            {
                _flagStartCmdSend = 0;
                // 测量完成
                if (_NIBPStart)
                {
                    nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_MEASURE_DONE, NULL, 0);
                    _NIBPStart = false;
                }
                // 测量停止
                else
                {
                    nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_STOP, NULL, 0);
                }
            }
        }
        else if (data[1] == MODULE_BUSY)
        {
            NIBPMonitorStateID state = (NIBPMonitorStateID)nibpParam.curStatusType();
            if (state == NIBP_MONITOR_STARTING_STATE)
            {
                nibpParam.provider().startMeasure(patientManager.getType());
            }
            else if (state == NIBP_MONITOR_SAFEWAITTIME_STATE || state == NIBP_MONITOR_STOPE_STATE
                                                           || state == NIBP_MONITOR_ERROR_STATE )
            {
                nibpParam.provider().stopMeasure();
            }
        }
        break;

    // 当前压力
    case SUNTECH_RSP_CUFF_PRESSURE:
    {
        feed();
        if (_powerOnSelfTest)
        {
            if (data[1] != 0)
            {
                AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_NIBP);
                if (alarmSource)
                {
                    alarmSource->setOneShotAlarm(NIBP_ONESHOT_ALARM_MODULE_ABNORMAL, true);   // 模块异常
                }
            }
            _powerOnSelfTest = false;
            _presssureTimer->start();
        }
        else
        {
            nibpParam.handleNIBPEvent(NIBP_EVENT_CURRENT_PRESSURE, &data[1], 2);
            int16_t pressure;
            pressure = (data[2] << 8) + data[1];
            if (pressure != -1)
            {
                nibpParam.setManometerPressure(pressure);
            }
        }
        break;
    }
    // 测量结果
    case SUNTECH_RSP_GET_MEASUREMENT:
    {
        NIBPMeasureResultInfo info = getMeasureResultInfo(&data[1]);
        if (nibpParam.curStatusType() == NIBP_MONITOR_STOPE_STATE)
        {
            nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_STOP, NULL, 0);
        }
        nibpParam.handleNIBPEvent(NIBP_EVENT_MONITOR_GET_RESULT,
                                  reinterpret_cast<unsigned char *>(&info), sizeof(info));
        break;
    }
    // 版本信息
    case SUNTECH_RSP_GET_RETURN_STRING:
    {
        const char *p = reinterpret_cast<const char*>(data + 1);
        versionInfo = QString("%1-%2").arg(QString(p)).arg(QString(p + 0x08));

        if (versionInfo.left(2) == SUNTECH_VERSION)
        {
            _suntechModel = SUNTECH_MODEL;
        }
        else if (versionInfo.left(2) == SUNTECH_A_PLUS_VERSION)
        {
            _suntechModel = SUNTECH_A_PLUS_MODEL;
        }
        break;
    }
    // 校准结果
    case SUNTECH_RSP_GET_CALIBRATE_RESULT:
    {
        bool result = !data[3];
        nibpParam.setResult(result);
    }
    default:
        break;
    }
}

void SuntechProvider::_sendCmd(const unsigned char *data, unsigned int len)
{
    int index = 0;
    unsigned char sendBuf[10] = {0};

    sendBuf[index++] = HOST_START_BYTE;
    for (unsigned int i = 0; i < len; i++)
    {
        sendBuf[index++] = data[i];
    }

    sendBuf[index++] = _calcCheckSum(sendBuf, len + 1);

    SuntechCMD cmd;
    for (unsigned int i = 0; i < len + 2; i++)
    {
        cmd.cmd[i] = sendBuf[i];
    }
    cmd.cmdLength = len + 2;
    list.append(cmd);
}

/**************************************************************************************************
 * calculate check sum.
 *************************************************************************************************/
unsigned char SuntechProvider::_calcCheckSum(const unsigned char *data, int len)
{
    unsigned char sum = 0x00;
    for (int i = 0; i < len; i++)
    {
        sum = sum + data[i];
    }
    sum = 0x00 - sum;
    return sum;
}
