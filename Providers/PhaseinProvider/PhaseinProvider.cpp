/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/26
 **/

#include "PhaseinProvider.h"
#include "Debug.h"
#include "AGParam.h"

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

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool PhaseinProvider::attachParam(Param *param)
{
    if (param->getParamID() == PARAM_AG)
    {
        agParam.setProvider(this);
        Provider::attachParam(param);
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 读取数据。
 *************************************************************************************************/
void PhaseinProvider::dataArrived()
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
//            debug("BLMCO2Provider discard data = 0x%x\n", ringBuff.at(0));
            ringBuff.pop(1);
        }
    }
}

/**************************************************************************************************
 * send command.
 *************************************************************************************************/
void PhaseinProvider::sendCmd(unsigned char dataId, const unsigned char data)
{
    unsigned char cmdBuf[_maxPacketLen] = {0};

    cmdBuf[0] = 0xAA;
    cmdBuf[1] = 0x55;
    cmdBuf[2] = dataId;
    cmdBuf[3] = data;
    cmdBuf[4] = ~(cmdBuf[2] + cmdBuf[3]) + 1;

    writeData(cmdBuf, _maxPacketLen);
}

void PhaseinProvider::disconnected()
{
    agParam.setConnected(false);
}

void PhaseinProvider::reconnected()
{
    agParam.setConnected(true);
}

/**************************************************************************************************
 * set work mode.
 *************************************************************************************************/
void PhaseinProvider::setWorkMode(AGWorkMode mode)
{
    sendCmd(CMD_SET_MODE, mode);
}

/**************************************************************************************************
 * set no-breaths timeout.
 *************************************************************************************************/
void PhaseinProvider::setApneaTimeout(ApneaAlarmTime t)
{
    unsigned char time;
    switch (t)
    {
    case APNEA_ALARM_TIME_20_SEC:
        time = 20;
        break;
    case APNEA_ALARM_TIME_25_SEC:
        time = 25;
        break;
    case APNEA_ALARM_TIME_30_SEC:
        time = 30;
        break;
    case APNEA_ALARM_TIME_35_SEC:
        time = 35;
        break;
    case APNEA_ALARM_TIME_40_SEC:
        time = 40;
        break;
    case APNEA_ALARM_TIME_45_SEC:
        time = 45;
        break;
    case APNEA_ALARM_TIME_50_SEC:
        time = 50;
        break;
    case APNEA_ALARM_TIME_55_SEC:
        time = 55;
        break;
    case APNEA_ALARM_TIME_60_SEC:
        time = 60;
        break;
    default:
        return;
    }
    sendCmd(CMD_SET_APNE_TIME, time);
}

/**************************************************************************************************
 * set primary agent ID.
 *************************************************************************************************/
void PhaseinProvider::setPrimaryID(AGAnaestheticType type)
{
    sendCmd(CMD_SET_AGENT_ID, type);
}

/**************************************************************************************************
 * set O2 compensation.
 *************************************************************************************************/
void PhaseinProvider::setO2Compensation(int comp)
{
    sendCmd(CMD_SET_O2, comp);
}

/**************************************************************************************************
 * set N2O compensation.
 *************************************************************************************************/
void PhaseinProvider::setN2OCompensation(int comp)
{
    sendCmd(CMD_SET_N20, comp);
}

/**************************************************************************************************
 * zero calibration.
 *************************************************************************************************/
void PhaseinProvider::zeroCalibration()
{
    sendCmd(CMD_ZERO_CAL, 255);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PhaseinProvider::PhaseinProvider(): Provider("PHASEIN_AG"), _status(AGProviderStatus())
{
    UartAttrDesc portAttr(9600, 8, 'N', 1);
    initPort(portAttr);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PhaseinProvider::~PhaseinProvider()
{
}

/**************************************************************************************************
 * 解析数据包。
 *************************************************************************************************/
void PhaseinProvider::_unpacket(const unsigned char packet[])
{
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
        debug("Checksum Error\n");
        return;
    }

    if (!isConnected)
    {
        agParam.setConnected(true);
    }

    // STS FIELD
    // 基本信息解包
    unsigned char sts    = packet[3];
    _status.breathDetect = (sts & BIT0) ? true : false;
    _status.noBreath     = (sts & BIT1) ? true : false;
    _status.o2Replace    = (sts & BIT3) ? true : false;
    _status.checkAdapt   = (sts & BIT4) ? true : false;
    _status.unspecAcc    = (sts & BIT5) ? true : false;
    _status.sensorErr    = (sts & BIT6) ? true : false;
    _status.o2Calib      = (sts & BIT7) ? true : false;
    agParam.setOneShotAlarm(AG_ONESHOT_ALARM_APNEA, _status.noBreath);

    // CO2波形数据
//    unsigned short co2WaveValue = packet[4];
//    co2WaveValue <<= 8;
//    co2WaveValue |= packet[5];
//    bool co2Invalid = false;
//    if (co2WaveValue == 0xFFFF)
//    {
//        co2Invalid = true;
//        co2WaveValue = 0;
//    }

    // N2O波形数据
    unsigned short n2oWaveValue = packet[6];
    n2oWaveValue <<= 8;
    n2oWaveValue |= packet[7];
    bool n2oInvalid = false;
    if (n2oWaveValue == 0xFFFF)
    {
        n2oInvalid = true;
        n2oWaveValue = 0;
    }
    agParam.addWaveformData(n2oWaveValue, n2oInvalid, AG_TYPE_N2O);

    // AA1波形数据
    unsigned short aa1WaveValue = packet[8];
    aa1WaveValue <<= 8;
    aa1WaveValue |= packet[9];
    bool aa1Invalid = false;
    if (aa1WaveValue == 0xFFFF)
    {
        aa1Invalid = true;
        aa1WaveValue = 0;
    }
    agParam.addWaveformData(aa1WaveValue, aa1Invalid, AG_TYPE_AA1);

    // AA2波形数据
    unsigned short aa2WaveValue = packet[10];
    aa2WaveValue <<= 8;
    aa2WaveValue |= packet[11];
    bool aa2Invalid = false;
    if (aa2WaveValue == 0xFFFF)
    {
        aa2Invalid = true;
        aa2WaveValue = 0;
    }
    agParam.addWaveformData(aa2WaveValue, aa2Invalid, AG_TYPE_AA2);

    // o2波形数据
    unsigned short o2WaveValue = packet[12];
    o2WaveValue <<= 8;
    o2WaveValue |= packet[13];
    bool o2Invalid = false;
    if (o2WaveValue == 0xFFFF)
    {
        o2Invalid = true;
        o2WaveValue = 0;
    }
    agParam.addWaveformData(o2WaveValue, o2Invalid, AG_TYPE_O2);

    // slow data
    switch (packet[2])
    {
    case INSP_VALS:     // inspire concentration
    {
        unsigned char fiValueC02 = packet[14];
        unsigned char fiValueN2O = packet[15];
        unsigned char fiValueAA1 = packet[16];
        unsigned char fiValueAA2 = packet[17];
        unsigned char fiValueO2 = packet[18];

        agParam.setFiData(fiValueC02, AG_TYPE_CO2);
        agParam.setFiData(fiValueN2O, AG_TYPE_N2O);
        agParam.setFiData(fiValueAA1, AG_TYPE_AA1);
        agParam.setFiData(fiValueAA2, AG_TYPE_AA2);
        agParam.setFiData(fiValueO2, AG_TYPE_O2);
        break;
    }
    case EXP_VALS:      // expiration concentration
    {
        unsigned char etValueCO2 = packet[14];
        unsigned char etValueN2O = packet[15];
        unsigned char etValueAA1 = packet[16];
        unsigned char etValueAA2 = packet[17];
        unsigned char etValueO2 = packet[18];

        agParam.setEtData(etValueCO2, AG_TYPE_CO2);
        agParam.setEtData(etValueN2O, AG_TYPE_N2O);
        agParam.setEtData(etValueAA1, AG_TYPE_AA1);
        agParam.setEtData(etValueAA2, AG_TYPE_AA2);
        agParam.setEtData(etValueO2, AG_TYPE_O2);
        break;
    }
    case MOM_VALS:      // moment concentration
    {
        break;
    }
    case GEN_VALS:      // additional information
    {
        _status.respRate = (packet[14] == 0xFF) ? 0 : packet[14];
        _status.lastBrethTime = packet[15];
        _status.primaryAgent = packet[16];
        _status.secondaryAgent = packet[17];
        _status.atmPress = packet[18] << 8 | packet[19];
        agParam.setAnestheticType((AGAnaestheticType)_status.primaryAgent, (AGAnaestheticType)_status.secondaryAgent);
        break;
    }
    case SENSOR_REGS:
    {
        _status.workMode = packet[14] & 0x07;
        _status.o2CompensationConc = packet[15];

        unsigned char sensorErr = packet[16];
        _status.softwareErr     = (sensorErr & BIT0) ? true : false;
        _status.hardwareErr     = (sensorErr & BIT1) ? true : false;
        _status.motorSpeedOut   = (sensorErr & BIT2) ? true : false;
        _status.uncalibrated    = (sensorErr & BIT3) ? true : false;
        _status.o2Error         = (sensorErr & BIT4) ? true : false;
        agParam.setOneShotAlarm(AG_ONESHOT_ALARM_SOFTWARE_ERROR, _status.softwareErr);
        agParam.setOneShotAlarm(AG_ONESHOT_ALARM_HARDWARE_ERROR, _status.hardwareErr);
        agParam.setOneShotAlarm(AG_ONESHOT_ALARM_MOTOR_ERROR, _status.motorSpeedOut);
        agParam.setOneShotAlarm(AG_ONESHOT_ALARM_MISS_FACTORY_CALI, _status.uncalibrated);
        agParam.setOneShotAlarm(AG_ONESHOT_ALARM_O2_SENSOR_ERROR, _status.o2Error);

        unsigned char adapterSta = packet[17];
        _status.replaceAdapt    = (adapterSta & BIT0) ? true : false;
        _status.noAdapt         = (adapterSta & BIT1) ? true : false;
        _status.o2Clogged       = (adapterSta & BIT2) ? true : false;

        unsigned char dataValid = packet[18];
        _status.co2OR           = (dataValid & BIT0) ? true : false;
        _status.n2oOR           = (dataValid & BIT1) ? true : false;
        _status.axOR            = (dataValid & BIT2) ? true : false;
        _status.o2OR            = (dataValid & BIT3) ? true : false;
        _status.tempOR          = (dataValid & BIT4) ? true : false;
        _status.pressOR         = (dataValid & BIT5) ? true : false;
        _status.zeroRequired    = (dataValid & BIT6) ? true : false;
        _status.agentUnreliable = (dataValid & BIT7) ? true : false;
        agParam.setOneShotAlarm(AG_ONESHOT_ALARM_CO2_OUT_RANGE, _status.co2OR);
        agParam.setOneShotAlarm(AG_ONESHOT_ALARM_N2O_OUT_RANGE, _status.n2oOR);
        agParam.setOneShotAlarm(AG_ONESHOT_ALARM_AX_OUT_RANGE, _status.axOR);
        agParam.setOneShotAlarm(AG_ONESHOT_ALARM_O2_OUT_RANGE, _status.o2OR);
        agParam.setOneShotAlarm(AG_ONESHOT_ALARM_TEMP_OUT_RANGE, _status.tempOR);
        agParam.setOneShotAlarm(AG_ONESHOT_ALARM_PRESS_OUT_RANGE, _status.pressOR);

        static bool lastZeroRequired = _status.zeroRequired;
        if (lastZeroRequired != _status.zeroRequired)
        {
            lastZeroRequired = _status.zeroRequired;      // 需要校零。
            agParam.setOneShotAlarm(AG_ONESHOT_ALARM_ZERO_REQUIRED, _status.zeroRequired);
        }


        _status.apneaTime = packet[19];

        break;
    }
    case CONFIG_DATA:
    {
        unsigned char configReg0 = packet[14];
        // O2 support
        _status.o2Config        = (configReg0 & BIT0) ? true : false;

        // CO2 support
        _status.co2Config       = (configReg0 & BIT1) ? true : false;

        // N2O support
        _status.n2oConfig       = (configReg0 & BIT2) ? true : false;

        // Anesthetic support
        _status.halConfig       = (configReg0 & BIT3) ? true : false;
        _status.enfConfig       = (configReg0 & BIT4) ? true : false;
        _status.isoConfig       = (configReg0 & BIT5) ? true : false;
        _status.sevConfig       = (configReg0 & BIT6) ? true : false;
        _status.desConfig       = (configReg0 & BIT7) ? true : false;

        unsigned char hardwareVersion = packet[15];
        // hardware version BCD code.
        _status.hardwareVersion = ((hardwareVersion & 0xf0) >> 4) * 10 + (hardwareVersion & 0xf);

        unsigned short softwareVersion = packet[16] | (packet[17] << 8);
        // software version BCD code.
        _status.softwareVersion = (softwareVersion & 0xf) + ((softwareVersion & 0xf0) >> 4) * 10
                                  + ((softwareVersion & 0xf00) >> 8) * 100 + ((softwareVersion & 0xf000) >> 12) * 1000;

        unsigned char configReg1 = packet[18];
        // sensor configuration register1
        _status.axIDConfig      = (configReg1 & BIT0) ? true : false;
        _status.n2oCompensationAvaiable = (configReg1 & BIT1) ? true : false;
        _status.o2SensorType    = (configReg1 & 0x0C) >> 2;
        _status.scientificConfig = (configReg1 & BIT6) ? true : false;
        _status.sidestreamConfig = (configReg1 & BIT7) ? true : false;

        // ID=0x04中的报警，因为CO2模块类型要在这里获取到所以报警也放在这里
        if (_status.sidestreamConfig)
        {
            // 旁流CO2模块报警
            agParam.setOneShotAlarm(AG_ONESHOT_ALARM_SAMPLING_LINE_CLOGGED, _status.replaceAdapt);
            agParam.setOneShotAlarm(AG_ONESHOT_ALARM_NO_SAMPLING_LINE, _status.noAdapt);
        }
        else
        {
            // 主流CO2模块报警
            agParam.setOneShotAlarm(AG_ONESHOT_ALARM_REPLACE_ADAPTER, _status.replaceAdapt);
            agParam.setOneShotAlarm(AG_ONESHOT_ALARM_NO_ADAPTER, _status.noAdapt);
            agParam.setOneShotAlarm(AG_ONESHOT_ALARM_O2_PORT_FAILURE, _status.o2Clogged);
        }

        // communication protocol version with BCD code
        unsigned char protocolVersion = packet[19];
        _status.commProtocolVersion = ((protocolVersion & 0xf0) >> 4) * 10 + (protocolVersion & 0xf);

        break;
    }
    case SERVICE_DATA:
    {
        _status.sensorSN = (packet[14] << 8) | packet[15];

        unsigned char staRegister = packet[16];
        _status.zeroDisable = (staRegister & BIT0) ? true : false;
        _status.zeroInProgress = (staRegister & BIT1) ? true : false;

        // For ISA only
        _status.spanError = ((staRegister & BIT2) == BIT2) ? true : false;
        _status.spanCalibInProgress = ((staRegister & BIT3) == BIT3) ? true : false;
        _status.irO2Delay = ((staRegister & 0xf0) >> 4);
        if (_status.sidestreamConfig)
        {
            // 旁流CO2模块报警
            agParam.setOneShotAlarm(AG_ONESHOT_ALARM_SPAN_CALIB_FAILED, _status.spanError);
            agParam.setOneShotAlarm(AG_ONESHOT_ALARM_SPAN_CALIB_IN_PROGRESS, _status.spanCalibInProgress);
        }
        agParam.setOneShotAlarm(AG_ONESHOT_ALARM_IR_O2_DELAY, _status.irO2Delay);

        _status.n2oCompensationConc = packet[17];

        _status.cuvetteAdjuestPress = (packet[19] == 0xff) ? InvData() : packet[19];


        break;
    }
    default:
        break;
    }

    agParam.AGModuleStatus(_status);
}
