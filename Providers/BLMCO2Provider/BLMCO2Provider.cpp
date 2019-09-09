/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/21
 **/

#include "BLMCO2Provider.h"
#include "CO2Param.h"
#include "AlarmDefine.h"
#include "Debug.h"
#include <QString>
#include <sys/time.h>
#include "SystemManager.h"
#include "PMessageBox.h"
#include <QTimer>
#include "ConfigManager.h"
#include "RawDataCollector.h"
#include "IConfig.h"
#include "crc8.h"

#define SOH             (0x01)  // upgrage packet header
#define MODEL_CONNECT_TIME_OUT 500

enum  // 数据包类型。
{
    INSP_VALS = 0x00,
    EXP_VALS = 0x01,
    MOM_VALS = 0x02,
    GEN_VALS = 0x03,
    SENSOR_REGS = 0x04,
    CONFIG_DATA = 0x05,
    SERVICE_DATA = 0x06,
    RAW_DATA = 0x0A,
    CALIBRATE_RESULT = 0x0D
};

enum  // 位操作。
{
    BIT0 = 0x01,
    BIT1 = 0x01 << 1,
    BIT2 = 0x01 << 2,
    BIT3 = 0x01 << 3,
    BIT4 = 0x01 << 4,
    BIT5 = 0x01 << 5,
    BIT6 = 0x01 << 6,
    BIT7 = 0x01 << 7,
    BIT_ALL = 0xFF,
};

enum  // 设置命令。
{
    CMD_SET_MODE = 0x00,      // 设置工作模式
    CMD_SET_APNE_TIME = 0x01,  // 设置窒息时间
    CMD_ENTER_UPGRADE_MODE = 0x03, // 进入升级模式
    CMD_SET_O2 = 0x04,        // 设置氧气补偿的浓度
    CMD_SET_N20 = 0x05,       // 设置笑气补偿的浓度
    CMD_ZERO_CAL = 0x06,      // 模块校零命令。
    CMD_CALIBRATE_DATA = 0x08 // 定标不同CO2浓度
};

/**************************************************************************************************
 * 解析数据包。
 *************************************************************************************************/
void BLMCO2Provider::_unpacket(const unsigned char packet[])
{
    co2ModelConnect = true;
    connectTmr.start(MODEL_CONNECT_TIME_OUT);
    if (!isConnectedToParam)
    {
        return;
    }

    unsigned char sum = 0;
    int i;
    for (i = 2; i < _packetLen; i++)
    {
        sum += packet[i];
    }
    if (sum)
    {
        debug("Checksum Error");
        return;
    }

    if (!isConnected)
    {
        co2Param.setConnected(true);
        setWorkMode(CO2_WORK_MEASUREMENT);  // 上电就开始测量。
    }
    feed();

    // STS FIELD
    // 基本信息解包
    unsigned char sts    = packet[3];
    _status.breathDetect = ((sts & BIT0) == BIT0) ? true : false;
    _status.noBreath     = ((sts & BIT1) == BIT1) ? true : false;  // life alarm
    _status.checkAdapt   = ((sts & BIT4) == BIT4) ? true : false;
    _status.unspecAcc    = ((sts & BIT5) == BIT5) ? true : false;
    _status.sensorErr    = ((sts & BIT6) == BIT6) ? true : false;
    _status.o2Replace    = ((sts & BIT3) == BIT3) ? true : false;

    if ((co2Param.getAwRRSwitch() == 1))
    {
#ifdef ENABLE_O2_APNEASTIMULATION
        co2Param.setRespApneaStimulation(_status.noBreath);
#endif
        co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_APNEA, _status.noBreath);
    }
    else
    {
#ifdef ENABLE_O2_APNEASTIMULATION
        co2Param.setRespApneaStimulation(false);
#endif
        co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_APNEA, false);
    }

    // 波形数据，每秒更新20次。
    if (packet[2] != RAW_DATA)
    {
        int waveValue = packet[4];
        waveValue <<= 8;
        waveValue |= packet[5];
        bool invalid = false;
        if (waveValue == 0xFFFF)
        {
            invalid = true;
            waveValue = 0;
        }

        waveValue = (waveValue > 2500) ? 2500 : waveValue;
        co2Param.addWaveformData(waveValue, invalid);
    }

    // 数据，每秒更新2次。
    short value;
    unsigned char val;
    int bcd_val_hi = 0, bcd_val_lo = 0;
    int bcd_val_hi2 = 0, bcd_val_lo2 = 0;
    int tmp_val = 0;
    bool isZeroInProgress = false;
    switch (packet[2])
    {
    case INSP_VALS:  // 吸入浓度值
        // --- CO2 ---
        value = (packet[14] == 0xFF) ? InvData() : packet[14];
        _fico2Value = value;
        break;

    case EXP_VALS:   // 呼吸末浓度值
        // --- CO2 ---
        value = (packet[14] == 0xFF) ? InvData() : packet[14];
        _etco2Value = value;
        break;

    case MOM_VALS:    // 瞬时浓度值
        break;

    case GEN_VALS:    //一些其他附加信息值
        value = (packet[14] == 0xFF) ? 0 : packet[14];  // 当获取不到CO2数据时候将数据置为0
        co2Param.setRR(value);

        value = packet[18];
        value <<= 8;
        value |= packet[19];
        co2Param.setBaro(value);

        {
            UnitType unit = co2Param.getUnit();
            UnitType dunit = paramInfo.getUnitOfSubParam(SUB_PARAM_ETCO2);
            int mul = paramInfo.getMultiOfSubParam(SUB_PARAM_ETCO2);
            if (unit == UNIT_MMHG)
            {
                QString tempStr = Unit::convert(unit, dunit, _etco2Value * 1.0 / mul, co2Param.getBaro());
                int val = tempStr.toInt();
                if ((co2Param.getEtCO2MinValue() <= val) && (val <= co2Param.getEtCO2MaxValue()))
                {
                    value = (packet[14] == 0xFF) ? 0 : packet[14];
                    co2Param.setFiCO2(_fico2Value);
                    co2Param.setEtCO2(_etco2Value);
                    co2Param.setBR(value);
                }
                else
                {
                    co2Param.setFiCO2(InvData());
                    co2Param.setEtCO2(InvData());
                    co2Param.setBR(InvData());
                }
            }
            else
            {
                value = (packet[14] == 0xFF) ? 0 : packet[14];
                co2Param.setFiCO2(_fico2Value);
                co2Param.setEtCO2(_etco2Value);
                co2Param.setBR(value);
            }
        }

        _status.lastBrethTime = packet[15];
        break;

    case SENSOR_REGS:    // 状态寄存器的值
        _status.workMode = packet[14] & 0x07;     // 工作模式。
        _status.o2CompensationConc = packet[15];  // O2补偿值。
        co2Param.verifyWorkMode((CO2WorkMode)_status.workMode);

        val = packet[16];
        _status.softwareErr   = ((val & BIT0) == BIT0) ? true : false;
        _status.hardwareErr   = ((val & BIT1) == BIT1) ? true : false;
        _status.motorSpeedOut = ((val & BIT2) == BIT2) ? true : false;
        _status.uncalibrated  = ((val & BIT3) == BIT3) ? true : false;
        _status.o2Error       = ((val & BIT4) == BIT4) ? true : false;
        co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_SOFTWARE_ERROR, _status.softwareErr);
        co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_HARDWARE_ERROR, _status.hardwareErr);
        co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_MOTOR_ERROR, _status.motorSpeedOut);
        co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_MISS_FACTORY_CALI, _status.uncalibrated);
//            if (_status.sidestreamConfig)
//            {
//                //旁流CO2模块报警
//                co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_O2_SENSOR_ERROR, _status.o2Error);
//            }

        val = packet[17];
        _status.replaceAdapt  = ((val & BIT0) == BIT0) ? true : false;
        _status.noAdapt       = ((val & BIT1) == BIT1) ? true : false;
        _status.o2Clogged     = ((val & BIT2) == BIT2) ? true : false;

        // CO2模块类型要在收到ID=0x05的报文中才能获取到，因此这里的报警放在ID=0x05中处理
//            if (_status.sidestreamConfig)
//            {
//                //旁流CO2模块报警
//                co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_SAMPLING_LINE_CLOGGED, _status.replaceAdapt);
//                co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_NO_SAMPLING_LINE, _status.noAdapt);
//            }
//            else
//            {
//                //主流CO2模块报警
//                co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_REPLACE_ADAPTER, _status.replaceAdapt);
//                co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_NO_ADAPTER, _status.noAdapt);
////                co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_O2_PORT_FAILURE, _status.o2Clogged);
//            }

        val = packet[18];
        _status.co2OR         = ((val & BIT0) == BIT0) ? true : false;
        _status.n2oOR         = ((val & BIT1) == BIT1) ? true : false;
        _status.axOR          = ((val & BIT2) == BIT2) ? true : false;
        _status.o2OR          = ((val & BIT3) == BIT3) ? true : false;
        _status.tempOR        = ((val & BIT4) == BIT4) ? true : false;
        _status.pressOR       = ((val & BIT5) == BIT5) ? true : false;
        _status.zeroRequired  = ((val & BIT6) == BIT6) ? true : false;
        _status.agentUnreliable = ((val & BIT7) == BIT7) ? true : false;

        co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_CO2_OUT_RANGE, _status.co2OR);
//            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_N2O_OUT_RANGE, _status.n2oOR);
//            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_AX_OUT_RANGE, _status.axOR);
//            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_O2_OUT_RANGE, _status.o2OR);
        co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_TEMP_OUT_RANGE, _status.tempOR);
        co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_PRESS_OUT_RANGE, _status.pressOR);
//            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_AGENT_UNRELIABLE, _status.agentUnreliable);

        static bool lastZeroRequired = _status.zeroRequired;
        if (lastZeroRequired != _status.zeroRequired)
        {
            lastZeroRequired = _status.zeroRequired;      // 需要校零。
            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_ZERO_REQUIRED, _status.zeroRequired);
        }
        break;

    case CONFIG_DATA:  // 模块配置信息
        val = packet[14];

        // 设备是否支持O2参数。
        _status.o2Config = ((val & BIT0) == BIT0) ? true : false;

        // 设备是否支持CO2参数。
        _status.co2Config = ((val & BIT1) == BIT1) ? true : false;

        // 设备是否支持N2O参数。
        _status.n2oConfig = ((val & BIT2) == BIT2) ? true : false;

        // 麻醉气体支持。
        _status.halConfig = ((val & BIT3) == BIT3) ? true : false;
        _status.enfConfig = ((val & BIT4) == BIT4) ? true : false;
        _status.isoConfig = ((val & BIT5) == BIT5) ? true : false;
        _status.sevConfig = ((val & BIT6) == BIT6) ? true : false;
        _status.desConfig = ((val & BIT7) == BIT7) ? true : false;

        val = packet[15];
        bcd_val_hi = (val & 0xf0) >> 4;
        bcd_val_lo = val & 0x0f;
        _status.hardwaveVersion = bcd_val_hi * 10 + bcd_val_lo;  // BCD code

        val = packet[16];
        bcd_val_hi = (val & 0xf0) >> 4;
        bcd_val_lo = val & 0x0f;
        val = packet[17];
        bcd_val_hi2 = (val & 0xf0) >> 4;
        bcd_val_lo2 = val & 0x0f;
        _status.softwaveVersion = bcd_val_hi * 1000 + bcd_val_lo * 100 + bcd_val_hi2 * 10
                                  + bcd_val_lo2;  // BCD code

        val = packet[18];
        _status.axIDConfig = ((val & BIT0) == BIT0) ? true : false;  // if auto ag id avaiable
        _status.n2oCompensationAvaiable = ((val & BIT1) == BIT1) ? true : false;
        _status.o2SensorType = (val & 0x0C) >> 2;
        _status.scientificConfig = ((val & BIT6) == BIT6) ? true : false;
        _status.sidestreamConfig = ((val & BIT7) == BIT7) ? true : false;

        // ID=0x04中的报警，因为CO2模块类型要在这里获取到所以报警也放在这里
        if (_status.sidestreamConfig)
        {
            // 旁流CO2模块报警
            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_SAMPLING_LINE_CLOGGED, _status.replaceAdapt);
            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_NO_SAMPLING_LINE, _status.noAdapt);
        }
        else
        {
            // 主流CO2模块报警
            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_REPLACE_ADAPTER, _status.replaceAdapt);
            co2Param.setZeroStatus(CO2_ZERO_REASON_NO_ADAPTER, _status.noAdapt);
            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_NO_ADAPTER, _status.noAdapt);
//                co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_O2_PORT_FAILURE, _status.o2Clogged);
        }

        val = packet[19];
        bcd_val_hi = (val & 0xf0) >> 4;
        bcd_val_lo = val & 0x0f;
        _status.commProtocolVersion = bcd_val_hi * 10 + bcd_val_lo;
        break;

    case SERVICE_DATA:  // 维护信息,包括模块的序列号,校零状态等.
        val = packet[18];
        _status.sensorSN = val;
        _status.sensorSN <<= 16;

        tmp_val = packet[14];
        _status.sensorSN |= (tmp_val << 8);
        val = packet[15];
        _status.sensorSN |= val;

        val = packet[16];
        _status.zeroDisable = ((val & BIT0) == BIT0) ? true : false;
        isZeroInProgress = _status.zeroInProgress;
        _status.zeroInProgress = ((val & BIT1) == BIT1) ? true : false;

        if (!_status.zeroDisable && !isZeroInProgress && _status.zeroInProgress)
        {
            // 当前非校零禁止时，前一刻非校零中，目前校零中
            co2Param.setZeroStatus(CO2_ZERO_REASON_IN_PROGRESS, _status.zeroInProgress);
            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_ZERO_IN_PROGRESS, _status.zeroInProgress);
        }

        if (_status.sidestreamConfig)
        {
            if (isZeroInProgress && !_status.zeroInProgress)
            {
                // 前一刻校零中，目前非校零中
                co2Param.setZeroStatus(CO2_ZERO_REASON_IN_PROGRESS, _status.zeroInProgress);
                co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_ZERO_IN_PROGRESS, _status.zeroInProgress);
            }

            co2Param.setZeroStatus(CO2_ZERO_REASON_DISABLED, _status.zeroDisable);
            // 旁流co2是的zeroDisable是禁止校准和校零
            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_ZERO_AND_SPAN_DISABLE, _status.zeroDisable);
        }
        else
        {
            if (isZeroInProgress && !_status.zeroInProgress)
            {
                // 前一刻校零中，目前非校零中
                co2Param.setZeroStatus(CO2_ZERO_REASON_IN_PROGRESS, _status.zeroInProgress);
                co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_ZERO_IN_PROGRESS, _status.zeroInProgress);
            }

            co2Param.setZeroStatus(CO2_ZERO_REASON_DISABLED, _status.zeroDisable);
            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_ZERO_DISABLE, _status.zeroDisable);
        }

        // For ISA Only
        _status.spanError = ((val & BIT2) == BIT2) ? true : false;
        _status.spanCalibInProgress = ((val & BIT3) == BIT3) ? true : false;
        _status.irO2Delay = (val & 0xf0) >> 4;

        if (_status.sidestreamConfig)
        {
            // 旁流CO2模块报警
            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_SPAN_CALIB_FAILED, _status.spanError);
            co2Param.setZeroStatus(CO2_ZERO_REASON_IN_PROGRESS, _status.spanCalibInProgress);
            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_SPAN_CALIB_IN_PROGRESS, _status.spanCalibInProgress);
        }
//            co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_IR_O2_DELAY, _status.irO2Delay);

        val = packet[17];
        _status.n2oCompensationConc = val;

        val = packet[19];
        _status.cuvetteAdjuestPress = (val == 0xff) ? InvData() : val;
        break;

    case RAW_DATA:
        rawDataCollector.collectData(RawDataCollector::CO2_DATA, packet + 3, _packetLen - 4);
        break;

    case CALIBRATE_RESULT:
        co2Param.setCalibrateData(packet + 2);
        break;
    default:
        break;
    }
}

bool BLMCO2Provider::_checkUpgradePacketValid(const unsigned char *data, unsigned int len)
{
    unsigned int minPacketLen = 5;
    if ((NULL == data) || (len < minPacketLen))
    {
        debug("Invalid packet!\n");
        debug("%s: FCS not match!\n", qPrintable(getName()));
        return false;
    }

    unsigned char crc = calcCRC(data, (len - 1));
    if (data[len - 1] != crc)
    {
//        outHex(data, (int)len);
//        debug("%s: FCS not match : %x!\n", qPrintable(getName()), crc);
        return false;
    }

    return true;
}

void BLMCO2Provider::_readUpgradeData()
{
    unsigned char buff[ringBuffLen] = {0};
    int len = uart->read(buff, ringBuffLen);
    if (len <= 0 || (buff[0] == 0xaa && buff[1] == 0x55))
    {
        return;
    }

    int startIndex = 0;
    bool isok;
    unsigned char v = ringBuff.head(isok);

    if (isok && len > 0)
    {
        if ((!_isLastSOHPaired) && (v == SOH) && (buff[0] == SOH))  // SOH为数据包起始数据。
        {
            _isLastSOHPaired = true;
            startIndex = 1;                  // 说明有连续两个SOH出现，需要丢弃一个。
        }
    }

    for (int i = startIndex; i < len; i++)
    {
        ringBuff.push(buff[i]);
        if (buff[i] != SOH)
        {
            _isLastSOHPaired = false;
            continue;
        }

        _isLastSOHPaired = false;
        i++;
        if (i >= len)
        {
            break;
        }

        if (buff[i] == SOH)                    // 剔除。
        {
            _isLastSOHPaired = true;
            continue;
        }

        ringBuff.push(buff[i]);
    }
}

bool BLMCO2Provider::_sendUpgradeData(const unsigned char *data, unsigned int len)
{
    if ((NULL == data) || (0 == len))
    {
        return false;
    }

    if (SOH != data[0])
    {
        // 协议数据以SOH字节开始
        debug("Invalid command!");
        return false;
    }

    int index = 0;
    unsigned char sendBuf[2 * len];

    sendBuf[index++] = data[0];
    for (unsigned int i = 1; i < len; i++)
    {
        if (SOH == data[i])
        {
            // 对SOH字节进行转义
            sendBuf[index++] = SOH;
        }

        sendBuf[index++] = data[i];
    }
//    outHex(sendBuf, index);
    return writeData(sendBuf, index) == index;
}

/**************************************************************************************************
 * 计算校验和。
 *************************************************************************************************/
static void _calcCheckSum(unsigned char buff[])
{
    buff[4] = ~(buff[2] + buff[3]) + 1;
}

/**************************************************************************************************
 * 与模块的通信中断。
 *************************************************************************************************/
void BLMCO2Provider::disconnected(void)
{
    co2Param.setConnected(false);
}

void BLMCO2Provider::reconnected()
{
    co2Param.setConnected(true);
}

void BLMCO2Provider::connectTimeOut()
{
    connectTmr.stop();
    co2ModelConnect = false;
}

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool BLMCO2Provider::attachParam(Param &param)
{
    if (param.getParamID() == PARAM_CO2)
    {
        co2Param.setProvider(this);
        Provider::attachParam(param);
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 读取数据。
 *************************************************************************************************/
void BLMCO2Provider::dataArrived(void)
{
    if (upgradeIface == NULL)
    {
        readData();
        if (ringBuff.dataSize() < _packetLen)
        {
            return;
        }

        unsigned char buff[64];
        while (ringBuff.dataSize() >= _packetLen)
        {
            if ((ringBuff.at(0) == 0xAA) && (ringBuff.at(1) == 0x55))
            {
                for (int i = 0; i < _packetLen; i++)
                {
                    buff[i] = ringBuff.at(0);
                    ringBuff.pop(1);
                }
                _unpacket(buff);
            }
            else
            {
    //            debug("BLMCO2Provider discard data = 0x%x", ringBuff.at(0));
                ringBuff.pop(1);
            }
        }
    }
    else
    {
        _readUpgradeData();
        unsigned char packet[570];
        int minPacketLen = 5;
        while (ringBuff.dataSize() >= minPacketLen)
        {
            if (ringBuff.at(0) != SOH)
            {
                // debug("discard (%s:%x)\n", qPrintable(getName()), ringBuff.at(0));
                ringBuff.pop(1);
                continue;
            }

            int len = (ringBuff.at(1) + (ringBuff.at(2) << 8));
            if ((len <= 0) || (len > 15))
            {
                ringBuff.pop(1);
                break;
            }
            if (len > ringBuff.dataSize()) // 数据还不够，继续等待。
            {
                break;
            }

            // 数据包不会超过packet长度，当出现这种情况说明发生了不可预料的错误，直接丢弃该段数据。
            if (len > static_cast<int>(sizeof(packet)))
            {
                ringBuff.pop(1);
                continue;
            }

            // 将数据包读到buff中。
            for (int i = 0; i < len; i++)
            {
                packet[i] = ringBuff.at(0);
                ringBuff.pop(1);
            }

            if (_checkUpgradePacketValid(packet, len))
            {
                if (upgradeIface)
                {
                    upgradeIface->handlePacket(&packet[3], len - 4);
                }
            }
            else
            {
                outHex(packet, len);
                debug("FCS error (%s)\n", qPrintable(getName()));
                ringBuff.pop(1);
            }
        }
    }
}

/**************************************************************************************************
 * 校零。
 *************************************************************************************************/
void BLMCO2Provider::zeroCalibration(void)
{
    unsigned char cmd[5] = {0};
    cmd[0] = 0xAA;
    cmd[1] = 0x55;
    cmd[2] = CMD_ZERO_CAL;
    cmd[3] = 255;
    _calcCheckSum(cmd);
    writeData(cmd, sizeof(cmd));
}

/**************************************************************************************************
 * 设置窒息超时时间。
 *************************************************************************************************/
void BLMCO2Provider::setApneaTimeout(ApneaAlarmTime t)
{
    unsigned char cmd[5] = {0};
    cmd[0] = 0xAA;
    cmd[1] = 0x55;
    cmd[2] = CMD_SET_APNE_TIME;
    cmd[3] = 20;
    switch (t)
    {
    case APNEA_ALARM_TIME_20_SEC:
        cmd[3] = 20;
        break;
    case APNEA_ALARM_TIME_25_SEC:
        cmd[3] = 25;
        break;
    case APNEA_ALARM_TIME_30_SEC:
        cmd[3] = 30;
        break;
    case APNEA_ALARM_TIME_35_SEC:
        cmd[3] = 35;
        break;
    case APNEA_ALARM_TIME_40_SEC:
        cmd[3] = 40;
        break;
    case APNEA_ALARM_TIME_45_SEC:
        cmd[3] = 45;
        break;
    case APNEA_ALARM_TIME_50_SEC:
        cmd[3] = 50;
        break;
    case APNEA_ALARM_TIME_55_SEC:
        cmd[3] = 55;
        break;
    case APNEA_ALARM_TIME_60_SEC:
        cmd[3] = 60;
        break;
    default:
        break;
    }

    _calcCheckSum(cmd);
    writeData(cmd, sizeof(cmd));
}

/**************************************************************************************************
 * 设置N2O补偿。
 *************************************************************************************************/
void BLMCO2Provider::setN2OCompensation(int comp)
{
    unsigned char cmd[5] = {0};
    cmd[0] = 0xAA;
    cmd[1] = 0x55;
    cmd[2] = CMD_SET_N20;
    cmd[3] = comp;

    _calcCheckSum(cmd);
    writeData(cmd, sizeof(cmd));
}

/**************************************************************************************************
 * 设置O2补偿。
 *************************************************************************************************/
void BLMCO2Provider::setO2Compensation(int comp)
{
    unsigned char cmd[5] = {0};
    cmd[0] = 0xAA;
    cmd[1] = 0x55;
    cmd[2] = CMD_SET_O2;
    cmd[3] = comp;

    _calcCheckSum(cmd);
    writeData(cmd, sizeof(cmd));
}

/**************************************************************************************************
 * 获取波形的采样速度。。
 *************************************************************************************************/
int BLMCO2Provider::getCO2WaveformSample(void)
{
    return 20;
}

/**************************************************************************************************
 * 获取最大的波形值。
 *************************************************************************************************/
int BLMCO2Provider::getCO2MaxWaveform(void)
{
    // 2000对应波形最大百分比，目前设置为20%
    return 2000;
}

/**************************************************************************************************
 * 设置工作模式。
 *************************************************************************************************/
void BLMCO2Provider::setWorkMode(CO2WorkMode mode)
{
    unsigned char cmd[5] = {0};
    cmd[0] = 0xAA;
    cmd[1] = 0x55;
    cmd[2] = CMD_SET_MODE;
    cmd[3] = mode;

    _calcCheckSum(cmd);
    writeData(cmd, sizeof(cmd));
}

void BLMCO2Provider::enterUpgradeMode()
{
    unsigned char cmd[5] = {0};
    cmd[0] = 0xAA;
    cmd[1] = 0x55;
    cmd[2] = CMD_ENTER_UPGRADE_MODE;
    cmd[3] = 0x0;

    _calcCheckSum(cmd);
    writeData(cmd, sizeof(cmd));
}

void BLMCO2Provider::sendCalibrateData(int value)
{
    unsigned char cmd[5] = {0};
    cmd[0] = 0xAA;
    cmd[1] = 0x55;
    cmd[2] = CMD_CALIBRATE_DATA;
    cmd[3] = value;
    _calcCheckSum(cmd);
    writeData(cmd, sizeof(cmd));
}

void BLMCO2Provider::setUpgradeIface(BLMProviderUpgradeIface *iface)
{
    if (iface == NULL)
    {
        stopCheckConnect(false);
    }
    else
    {
        stopCheckConnect(true);
    }
    upgradeIface = iface;
}

bool BLMCO2Provider::sendUpgradeCmd(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    // 数据包长度包括包头、包长、帧类型、数据和校验和，即：数据长度＋4。
    unsigned int minPacketLen = 5;
    unsigned int maxPacketLen = (1 << 9);
    unsigned int cmdLen = len + minPacketLen;
    unsigned char cmdBuf[maxPacketLen] = {0};
    if (cmdLen > maxPacketLen)
    {
        debug("Comand too long!");
        return false;
    }

    cmdBuf[0] = SOH;
    cmdBuf[1] = (cmdLen & 0xFF);
    cmdBuf[2] = ((cmdLen >> 8) & 0xFF);
    cmdBuf[3] = cmdId;
    if ((NULL != data) && (0 != len))
    {
        qMemCopy(cmdBuf + 4, data, len);
    }

    cmdBuf[cmdLen - 1] = calcCRC(cmdBuf, (cmdLen - 1));
    return _sendUpgradeData(cmdBuf, cmdLen);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
BLMCO2Provider::BLMCO2Provider(const QString &name)
    : Provider(name), CO2ProviderIFace(), _status(CO2ProviderStatus()),
      upgradeIface(NULL),
      _isLastSOHPaired(false),
      co2ModelConnect(false)
{
    UartAttrDesc portAttr(9600, 8, 'N', 1, _packetLen);
    if (name == "MASIMO_CO2")
    {
        UartAttrDesc desc(9600, 8, 'N', 1, _packetLen);
        portAttr = desc;
    }
    else if (name == "BLM_CO2")
    {
        UartAttrDesc desc(115200, 8, 'N', 1);
        portAttr = desc;
    }

    if (!initPort(portAttr))
    {
        systemManager.setPoweronTestResult(CO2_MODULE_SELFTEST_RESULT, SELFTEST_FAILED);
    }

    setDisconnectThreshold(1);
    _etco2Value = InvData();
    _fico2Value = InvData();

    connect(&connectTmr, SIGNAL(timeout()), this, SLOT(connectTimeOut()));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
BLMCO2Provider::~BLMCO2Provider()
{
}

bool BLMCO2Provider::isConnectModel()
{
    return co2ModelConnect;
}
