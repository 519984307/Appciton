/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/1/7
 **/

#include <QString>
#include "E5Provider.h"
#include "ECGParam.h"
#include "ECGAlarm.h"
#include "ECGDupParam.h"
#include "RESPParam.h"
#include "RESPDupParam.h"
#include "RESPAlarm.h"
#include "ECGAlarm.h"
#include "Debug.h"
#include "SystemManager.h"

#include "TimeManager.h"
#include <sys/time.h>
#include "Framework/ErrorLog/ErrorLog.h"
#include "Framework/ErrorLog/ErrorLogItem.h"
#include "IConfig.h"
#include "RawDataCollector.h"
#include "AlarmSourceManager.h"
#include "ConfigManager.h"

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool E5Provider::attachParam(Param *param)
{
    Provider::attachParam(param);
    if (param->getParamID() == PARAM_ECG)
    {
        ecgParam.setProvider(this);
        return true;
    }
    if (param->getParamID() == PARAM_RESP)
    {
        respParam.setProvider(this);
        return true;
    }
    return false;
}

/**************************************************************************************************
 * 处理心电数据。
 *************************************************************************************************/
void E5Provider::_handleECGRawData(const unsigned char *data, unsigned len)
{
    // 记录下各种标记的状态以免丢失。
    static short leadoff = 0;
    static bool overLoad = false;
    static bool ipaceMark = false;
    static bool rMark = false;
    static bool pdBlank = false;
    static bool epaceMark = false;
    static bool qrsTone = false;

    short rawData[13];
    unsigned short v = 0;

    for (unsigned j = 5; j < len ; j += 28)
    {
        int rawDataIndex = 0;
        for (unsigned i = j + 2; i < j + 28; i += 2)
        {
            v = (data[i + 1] << 8) | (data[i]);
//            rawData[rawDataIndex++] = static_cast<int>((v & 0x8000) ? (-((~(v - 1)) & 0xffff)) : v);
            rawData[rawDataIndex++] = (short) v;
        }

        int leadData[ECG_LEAD_NR];
        leadData[ECG_LEAD_I]   = rawData[0];
        leadData[ECG_LEAD_II]  = rawData[1];
        leadData[ECG_LEAD_III] = rawData[2];
        leadData[ECG_LEAD_AVR] = rawData[3];
        leadData[ECG_LEAD_AVL] = rawData[4];
        leadData[ECG_LEAD_AVF] = rawData[5];
        leadData[ECG_LEAD_V1]  = rawData[6];
        leadData[ECG_LEAD_V2]  = rawData[7];
        leadData[ECG_LEAD_V3]  = rawData[8];
        leadData[ECG_LEAD_V4]  = rawData[9];
        leadData[ECG_LEAD_V5]  = rawData[10];
        leadData[ECG_LEAD_V6]  = rawData[11];

        // 记录下各种标记的状态以免丢失。
        leadoff |= (data[j + 1] | ((data[j] & 0x03) << 8));
        overLoad |= (data[j] >> 5) & 0x01;

        if (_waveSampleRate == WAVE_SAMPLE_RATE_500)
        {
            ipaceMark |= (data[j] >> 4) & 0x01;
        }
        else
        {
            ipaceMark |= (data[j] >> 3) & 0x01;
        }

        rMark |= (data[j] >> 2) & 0x01;
        qrsTone += (data[j] >> 6) & 0x01;
        pdBlank |= (data[j] >> 7) & 0x01;
        epaceMark |= (data[j] >> 7) & 0x01;

        bool leadOff[ECG_LEAD_NR];
        memset(leadOff, 0, ECG_LEAD_NR);

        if (ecgParam.getLeadMode() == ECG_LEAD_MODE_5)
        {
            leadoff &= 0xFFE0;
        }
        else if (ecgParam.getLeadMode() == ECG_LEAD_MODE_3)
        {
            leadoff &= 0x1C0;
        }

        if (!leadoff && !_isFristConnect)
        {
            ecgParam.setFristConnect();
            _isFristConnect = true;
        }

        if (ecgParam.getLeadMode() == ECG_LEAD_MODE_3)
        {
            leadoff &= 0xFFC0;
        }

        // 根据导联脱落处理各导联的数据。
#ifdef CONFIG_ECG_TEST    //此选项供算法组调试使用
        // bit9 RL, bit8 LA, bit7 LL, bit6 RA,
        if (leadoff & 0x200)  // RL
        {
            for (int i = 0; i < ECG_LEAD_NR - 1; i++)
            {
                leadData[i] = 0;
                leadOff[i] = true;
            }
        }
        if (leadoff & 0x100)  // LA
        {
            leadOff[ECG_LEAD_I] = true;
            leadOff[ECG_LEAD_III] = true;
            leadOff[ECG_LEAD_AVR] = true;
            leadOff[ECG_LEAD_AVL] = true;
            leadOff[ECG_LEAD_AVF] = true;

            leadData[ECG_LEAD_I] = 0;
            leadData[ECG_LEAD_III] = 0;
            leadData[ECG_LEAD_AVR] = 0;
            leadData[ECG_LEAD_AVL] = 0;
            leadData[ECG_LEAD_AVF] = 0;
        }
        if (leadoff & 0x80)  // LL
        {
            leadOff[ECG_LEAD_II] = true;
            leadOff[ECG_LEAD_III] = true;
            leadOff[ECG_LEAD_AVR] = true;
            leadOff[ECG_LEAD_AVL] = true;
            leadOff[ECG_LEAD_AVF] = true;

            leadData[ECG_LEAD_II] = 0;
            leadData[ECG_LEAD_III] = 0;
            leadData[ECG_LEAD_AVR] = 0;
            leadData[ECG_LEAD_AVL] = 0;
            leadData[ECG_LEAD_AVF] = 0;
        }
        if (leadoff & 0x40)  // RA
        {
            leadOff[ECG_LEAD_I] = true;
            leadOff[ECG_LEAD_II] = true;
            leadOff[ECG_LEAD_AVR] = true;
            leadOff[ECG_LEAD_AVL] = true;
            leadOff[ECG_LEAD_AVF] = true;

            leadData[ECG_LEAD_I] = 0;
            leadData[ECG_LEAD_II] = 0;
            leadData[ECG_LEAD_AVR] = 0;
            leadData[ECG_LEAD_AVL] = 0;
            leadData[ECG_LEAD_AVF] = 0;
        }
        if (leadoff & 0x20)                // bit5 V1
        {
            leadData[ECG_LEAD_V1] = 0;
            leadOff[ECG_LEAD_V1] = true;
        }
        if (leadoff & 0x10)                // bit4 V2
        {
            leadData[ECG_LEAD_V2] = 0;
            leadOff[ECG_LEAD_V2] = true;
        }
        if (leadoff & 0x08)                // bit3 V3
        {
            leadData[ECG_LEAD_V3] = 0;
            leadOff[ECG_LEAD_V3] = true;
        }
        if (leadoff & 0x04)                // bit2 V4
        {
            leadData[ECG_LEAD_V4] = 0;
            leadOff[ECG_LEAD_V4] = true;
        }
        if (leadoff & 0x02)               // bit1 V5
        {
            leadData[ECG_LEAD_V5] = 0;
            leadOff[ECG_LEAD_V5] = true;
        }
        if (leadoff & 0x01)               // bit0 V6
        {
            leadData[ECG_LEAD_V6] = 0;
            leadOff[ECG_LEAD_V6] = true;
        }
#else
        if (leadoff & 0x3C0)  // RL/LA/LL/RA
        {
            for (int i = 0; i < ECG_LEAD_NR - 1; i++)
            {
                leadData[i] = 0;
                leadOff[i] = true;
            }
        }
        else
        {
            if (leadoff & 0x20)                // bit5 V1
            {
                leadData[ECG_LEAD_V1] = 0;
                leadOff[ECG_LEAD_V1] = true;
            }
            if (leadoff & 0x10)                // bit4 V2
            {
                leadData[ECG_LEAD_V2] = 0;
                leadOff[ECG_LEAD_V2] = true;
            }
            if (leadoff & 0x08)                // bit3 V3
            {
                leadData[ECG_LEAD_V3] = 0;
                leadOff[ECG_LEAD_V3] = true;
            }
            if (leadoff & 0x04)                // bit2 V4
            {
                leadData[ECG_LEAD_V4] = 0;
                leadOff[ECG_LEAD_V4] = true;
            }
            if (leadoff & 0x02)               // bit1 V5
            {
                leadData[ECG_LEAD_V5] = 0;
                leadOff[ECG_LEAD_V5] = true;
            }
            if (leadoff & 0x01)               // bit0 V6
            {
                leadData[ECG_LEAD_V6] = 0;
                leadOff[ECG_LEAD_V6] = true;
            }
        }
#endif

        for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; i++)
        {
            ecgParam.setLeadOff((ECGLead)i, leadOff[i]);
        }

        if (ecgParam.getCalcLead() != ECG_LEAD_NR)
        {
            ecgParam.setOverLoad(overLoad);
        }
        ecgParam.updateWaveform(leadData, leadOff, ipaceMark, epaceMark, qrsTone);

        leadoff = 0;
        overLoad = false;
        ipaceMark = false;
        rMark = false;
        pdBlank = false;
        epaceMark = false;
        qrsTone = false;
    }
}

/**************************************************************************************************
 * 处理呼吸数据。
 *************************************************************************************************/
void E5Provider::_handleRESPRawData(const unsigned char *data, unsigned /*len*/)
{
    int resp = (data[2] << 8) | data[1];

    if (0 == respParam.getRespMonitoring())
    {
        resp = 0;
    }

    int flag = 0;
    if (resp & 0x8000)
    {
        flag |= INVALID_WAVE_FALG_BIT;
        resp = getRESPBaseLine();
        respParam.setLeadoff(true, _isRespFirstConnect);
    }
    else
    {
        respParam.setLeadoff(false);
        if (!_isRespFirstConnect)
        {
            _isRespFirstConnect = true;
        }
    }

    resp &= 0x7FFF;
    resp = ((resp & 0x4000) ? (-((~(resp - 1)) & 0x7FFF)) : resp);

    respParam.addWaveformData(resp, flag);
}

/**************************************************************************************************
 * 读取数据。
 *************************************************************************************************/
void E5Provider::handlePacket(unsigned char *data, int len)
{
#if 0
    static struct timeval startTime = {0, 0};
    static struct timeval endTime = {0, 0};
    static int dataCount = 0;
    if (startTime.tv_sec == 0)
    {
        gettimeofday(&startTime, NULL);
    }
    dataCount += len + 4;
    gettimeofday(&endTime, NULL);
    int timeElaspe = (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec);
    if (timeElaspe >= 1000000)
    {
        qdebug("dataRate: %d bytes/sec", dataCount * 1000 / (timeElaspe / 1000));
        dataCount = 0;
        startTime = endTime;
    }
#endif
    if (!isConnectedToParam)
    {
        return;
    }

    BLMProvider::handlePacket(data, len);

    switch (data[0])
    {
    case TE3_CYCLE_ACTIVE:
    case TE3_UPGRADE_ALIVE:
        feed();
        break;

    case TE3_RSP_VERSION:
        return;

    case TE3_NOTIFY_SYSTEM_STARTED:
    {
        unsigned char data = TE3_NOTIFY_SYSTEM_STARTED;
        sendCmd(TE3_CMD_ASK, &data, 1);
        break;
    }

    default:
        break;
    }

    switch (data[0])
    {
    case TE3_RSP_VERSION:
        break;

    case TE3_RSP_SYSTEM_TEST:  // 系统测试命令反馈
        break;

    case TE3_RSP_PACE_SYNC_STATUS:
        break;

    case TE3_RSP_ECG_DATA_SYNC:
        break;

    case TE3_RSP_ECG_LEAD_MODE:
        break;

    case TE3_RSP_FILTER_PARAMETER:
        break;

    case TE3_RSP_NOTCH_FILTER:
        break;

    case TE3_RSP_ECG_SAMPLE_CONFIG:
        break;

    case TE3_RSP_ECG_PATIENT_TYPE:
        enableVFCalcCtrl(true);
        break;

    case TE3_RSP_ECG_LEAD_CABLE_TYPE:
        ecgParam.handleECGLeadCabelType(data[1]);
        break;

    case TE3_RSP_RESP_APNEA_INTERVAL:
        break;

    case TE3_RSP_ECG_ENABLE_RAW_DATA:
        break;

    case TE3_RSP_SELFTEST_RESULT:
    {
        unsigned result = 0;
        if (0 == data[1])
        {
            result |= MAJOR_SAMPLE_MODULE_INIT_FAILED;
        }

        if (0 == data[2])
        {
            result |= MINOR_SAMPLE_MODULE_INIT_FAILED;
        }

        ecgParam.handleSelfTestResult(result);
        break;
    }

    case TE3_NOTIFY_RESP_INACCURATE:
    {
        respParam.rrInaccurate(static_cast<bool>(data[1]));
        break;
    }

    case TE3_NOTIFY_SYSTEM_STARTED:
    {
        ErrorLogItem *item = new ErrorLogItem();
        item->setName("E5 Start");
        errorLog.append(item);

        ecgParam.reset();
        respParam.reset();

        const unsigned char data = TE3_NOTIFY_SYSTEM_STARTED;
        sendCmd(TE3_CMD_ASK, &data, 1);
        break;
    }

    case TE3_NOTIFY_SYSTEM_TEST_RESULT:
        break;

    case TE3_NOTIFY_ECG_LEAD_CABEL_TYPE:
        ecgParam.handleECGLeadCabelType(data[1]);
        break;

    case TE3_NOTIFY_RESP_ALARM:
    {
#ifdef ENABLE_O2_APNEASTIMULATION
        respDupParam.setRespApneaStimulation(data[1]);
#endif
        AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_RESP);
        if (alarmSource)
        {
            alarmSource->setOneShotAlarm(RESP_ONESHOT_ALARM_APNEA, data[1]);
        }
        break;
    }

    case TE3_NOTIFY_VF_ALARM:
    {
        AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_ECG);
        if (alarmSource)
        {
            alarmSource->setOneShotAlarm(ECG_ONESHOT_ARR_VFIBVTAC, data[1]);
        }
        break;
    }

    case TE3_NOTIFY_ASYS_ALARM:
    {
        AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_ECG);
        if (alarmSource)
        {
            alarmSource->setOneShotAlarm(ECG_ONESHOT_ARR_ASYSTOLE, data[1]);
        }
        break;
    }
    case TE3_CYCLE_ACTIVE:
        feed();
        break;

    case TE3_CYCLE_ECG:  // 收到心电数据包。
        _handleECGRawData(data, len);
        break;

    case TE3_CYCLE_RESP:
        _handleRESPRawData(data, len);
        break;

    case TE3_CYCLE_RR:
    {
        short rr = (data[2] << 8) | data[1];
        rr = (rr == -1) ? InvData() : rr;
        if (0 == respParam.getRespMonitoring())
        {
            rr = InvData();
        }

        // set invalid rr when it does not support RESP
        if (!_isSupportRESP)
        {
            rr = InvData();
        }

        // 设置呼吸率的测量范围
        if (rr > respParam.getRRMeasureMaxRange() || rr < respParam.getRRMeasureMinRange())
        {
            rr = InvData();
        }

        respParam.setRR(rr);
        break;
    }

    case TE3_CYCLE_HR:
    {
        short hrValue = (data[2] << 8) | data[1];
        hrValue = (hrValue == -1) ? InvData() : hrValue;
        if (ecgParam.getCalcLead() != ECG_LEAD_NR)
        {
            ecgParam.updateHR(hrValue);
        }
        break;
    }

    case TE3_CYCLE_VFVT:
    {
        short vfvtValue = (data[2] << 8) | data[1];
        vfvtValue = (vfvtValue == -1) ? InvData() : vfvtValue;
        ecgParam.updateVFVT(vfvtValue);
        break;
    }

    case TE3_CYCLE_STORE:
    {
        rawDataCollector.collectData(RawDataCollector::ECG_DATA, data + 1, len -1);
        break;
    }

    case TE3_WARNING_ERROR_CODE:
    {
        QString errorStr("");
        unsigned num = 0;
        switch (data[1])
        {
        case 0x01:
            if (0 == data[2])
            {
                errorStr = "Major Collection Module Has No Data.\r\n";
            }
            else
            {
                errorStr = "Minor Collection Module Has No Data.\r\n";
            }
            break;

        case 0x02:
            num = ((data[5] << 24) | (data[4] << 16) | (data[3] << 8) | data[2]);
            errorStr = QString("Major Collection Module %1 Packet Loss.\r\n").arg(num);
            break;

        case 0x03:
            num = ((data[5] << 24) | (data[4] << 16) | (data[3] << 8) | data[2]);
            errorStr = QString("Minor Collection Module %1 Packet Loss.\r\n").arg(num);
            break;

        case 0x04:
            num = ((data[5] << 24) | (data[4] << 16) | (data[3] << 8) | data[2]);
            errorStr = QString("Major Collecting %1 Error Data Frames.\r\n").arg(num);
            break;

        case 0x05:
            num = ((data[5] << 24) | (data[4] << 16) | (data[3] << 8) | data[2]);
            errorStr = QString("Minor Collecting %1 Error Data Frames.\r\n").arg(num);
            break;

        case 0x06:
            if (0 == data[2])
            {
                errorStr = "Internal Flash Flag OK.\r\n";
            }
            else
            {
                errorStr = "Internal Flash Flag Error.\r\n";
            }

        default:
            break;
        }

        if (!errorStr.isEmpty())
        {
            ErrorLogItem *item = new CriticalFaultLogItem();
            item->setName(QString("E5 Error:0x%1").arg(data[1], 2, 16, QChar('0')));
            item->setLog(errorStr);
            item->setSubSystem(ErrorLogItem::SUB_SYS_E5);
            item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
            item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
            errorLog.append(item);
        }
        break;
    }
    }
}

/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void E5Provider::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    sendCmd(cmdId, data, len);
}

/**************************************************************************************************
 * 获取自检状态。
 *************************************************************************************************/
void E5Provider::getSelfTestStatus(void)
{
    sendCmd(TE3_CMD_GET_SELFTEST_RESULT, NULL, 0);
}

/**************************************************************************************************
 * 获取波形的采样速度。
 *************************************************************************************************/
int E5Provider::getWaveformSample(void)
{
    return _waveSampleRate;
}

/**************************************************************************************************
 * 设置波形的采样速度。
 *************************************************************************************************/
void E5Provider::setWaveformSample(int rate)
{
    _waveSampleRate = rate;
}

int E5Provider::getP05mV()
{
    return 408;
}

int E5Provider::getN05mV()
{
    return -408;
}

/**************************************************************************************************
 * 获取导联线类型
 *************************************************************************************************/
void E5Provider::getLeadCabelType()
{
    sendCmd(TE3_CMD_GET_LEAD_CABLE_TYPE, NULL, 0);
}

/**************************************************************************************************
 * 设置导联系统。
 *************************************************************************************************/
void E5Provider::setLeadSystem(ECGLeadMode leadSystem)
{
    unsigned char dataPayload = (unsigned char)leadSystem;
    sendCmd(TE3_CMD_SET_ECG_LEAD_MODE, &dataPayload, 1);
}

/**************************************************************************************************
 * 设置计算导联。
 *************************************************************************************************/
void E5Provider::setCalcLead(ECGLead lead)
{
    if (lead <= ECG_LEAD_V6)
    {
        // 协议顺序和导联定义顺序不一致
        unsigned char dataCalcMode = (unsigned char)lead;
        switch (lead)
        {
        case ECG_LEAD_AVR:
        case ECG_LEAD_AVL:
        case ECG_LEAD_AVF:
            dataCalcMode = 9 + lead - ECG_LEAD_AVR;
            break;

        case ECG_LEAD_V1:
        case ECG_LEAD_V2:
        case ECG_LEAD_V3:
        case ECG_LEAD_V4:
        case ECG_LEAD_V5:
        case ECG_LEAD_V6:
            dataCalcMode = 3 + lead - ECG_LEAD_V1;
            break;

        default:
            dataCalcMode = (unsigned char)lead;
            break;
        }

        sendCmd(TE3_CMD_SET_ECG_CALC_LEAD, &dataCalcMode, 1);
    }
}

/**************************************************************************************************
 * 设置病人类型。0表示成人，1表示小孩，2表示新生儿，3表示空
 *************************************************************************************************/
void E5Provider::setPatientType(unsigned char type)
{
    unsigned char cmd;
    cmd = type & 0xFF;
    if (cmd > 0x03)
    {
        cmd = 0;
    }

    sendCmd(TE3_CMD_SET_PATIENT_TYPE, &cmd, 1);
}

void E5Provider::setWorkMode(ECGWorkMode workMode)
{
    unsigned char data = workMode;
    sendCmd(TE3_CMD_SET_WORK_MODE_IN_MONITOR, &data, 1);
}

void E5Provider::setFilterMode(ECGFilterMode mode)
{
    ECGBandwidth band;
    switch (mode)
    {
    case ECG_FILTERMODE_SURGERY:
        band = ECG_BANDWIDTH_067_20HZ;
        break;
    case ECG_FILTERMODE_MONITOR:
        band = ECG_BANDWIDTH_067_40HZ;
        break;
    case ECG_FILTERMODE_ST:
        band = ECG_BANDWIDTH_0525_40HZ;
        break;
    case ECG_FILTERMODE_DIAGNOSTIC:
        band = ECG_BANDWIDTH_0525_150HZ;
        break;
    default:
        band = ECG_BANDWIDTH_067_40HZ;
        break;
    }

    ECGWorkMode workMode;
    if (mode == ECG_FILTERMODE_SURGERY || mode == ECG_FILTERMODE_MONITOR)
    {
        workMode = ECG_WORKMODE_MONITOR;
    }
    else
    {
        workMode = ECG_WORKMODE_DIAGNOSTIC;
    }
    setWorkMode(workMode);
    setBandwidth(band);
}

/**************************************************************************************************
 * 设置带宽。
 *************************************************************************************************/
void E5Provider::setBandwidth(ECGBandwidth bandwidth)
{
    unsigned char dataPayload = 0;

    switch (bandwidth)
    {
    case ECG_BANDWIDTH_067_20HZ:
        dataPayload = 0x12;
        break;

    case ECG_BANDWIDTH_067_40HZ:
        dataPayload = 0x13;
        break;

    case ECG_BANDWIDTH_0525_40HZ:
        dataPayload = 0x00;
        break;

    case ECG_BANDWIDTH_0525_150HZ:
        dataPayload = 0x01;
        break;

    default:
        return;
    }

    sendCmd(TE3_CMD_SET_FILTER_PARAMETER, &dataPayload, 1);
}

void E5Provider::setSelfLearn(bool onOff)
{
    unsigned char learn = onOff;
    Q_UNUSED(learn)
}

void E5Provider::setARRThreshold(ECGAlg::ARRPara parameter, short value)
{
    Q_UNUSED(parameter)
    Q_UNUSED(value)
}

/**************************************************************************************************
 * 起搏器设置。
 *************************************************************************************************/
void E5Provider::enablePacermaker(ECGPaceMode onoff)
{
    unsigned char enable = onoff;
    sendCmd(TE3_CMD_SET_PACEMARK_ONOFF, &enable, 1);
}

/**************************************************************************************************
 * 设置工频滤波。
 *************************************************************************************************/
void E5Provider::setNotchFilter(ECGNotchFilter notch)
{
    unsigned char data = notch;
    ECGFilterMode filter = ecgParam.getFilterMode();
    if (filter == ECG_FILTERMODE_SURGERY || filter == ECG_FILTERMODE_MONITOR)
    {
        data |= 0x10;
    }
    sendCmd(TE3_CMD_SET_NOTCH_FILTER, &data, 1);
}

/**************************************************************************************************
 * ST开关。
 *************************************************************************************************/
void E5Provider::enableSTAnalysis(bool onoff)
{
    Q_UNUSED(onoff)
}

/***************************************************************************************************
 * enable vf calc control
 **************************************************************************************************/
void E5Provider::enableVFCalcCtrl(bool enable)
{
    unsigned char data = enable;
    sendCmd(TE3_CMD_ENABLE_VF_CALC, &data, 1);
}

/**************************************************************************************************
 * ST点设置。
 *************************************************************************************************/
void E5Provider::setSTPoints(int iso, int st)
{
    Q_UNUSED(iso)
    Q_UNUSED(st)
}

void E5Provider::enableRawData(bool onoff)
{
    unsigned char data = onoff;
    sendCmd(TE3_CMD_ECG_ENABLE_RAW_DATA, &data, 1);
}

/**************************************************************************************************
 * 获取版本号。
 *************************************************************************************************/
void E5Provider::sendVersion(void)
{
    sendCmd(TE3_CMD_GET_VERSION, NULL, 0);
}

/**************************************************************************************************
 * 获取波形的采样速度。
 *************************************************************************************************/
int E5Provider::getRESPWaveformSample(void)
{
    return 125;
}

/**************************************************************************************************
 * 关闭窒息处理。
 *************************************************************************************************/
void E5Provider::disableApnea(void)
{
}

/**************************************************************************************************
 * 设置窒息时间。
 *************************************************************************************************/
void E5Provider::setApneaTime(ApneaAlarmTime t)
{
    unsigned char data = t;

    sendCmd(TE3_CMD_SET_RESP_APNEA_INTERVAL, &data, 1);
}

/**************************************************************************************************
 * 获取波形的采样速度。
 *************************************************************************************************/
void E5Provider::setWaveformZoom(RESPZoom zoom)
{
    zoom = zoom;
}

/**************************************************************************************************
 * 设置呼吸导联。
 *************************************************************************************************/
void E5Provider::setRESPCalcLead(RESPLead lead)
{
    unsigned char data = 0;
    switch (lead)
    {
    case RESP_LEAD_I:
        data = 1;
        break;

    case RESP_LEAD_II:
        data = 0;
        break;

    default:
        break;
    }

    sendCmd(TE3_CMD_SET_RESP_CALC_LEAD, &data, 1);
}

/**************************************************************************************************
 * 启用呼吸计算。
 *************************************************************************************************/
void E5Provider::enableRESPCalc(bool enable)
{
    unsigned char data = enable;
    sendCmd(TE3_CMD_SET_RESP_CALS_SWITCH, &data, 1);
}

/**************************************************************************************************
 * 连接中断。
 *************************************************************************************************/
void E5Provider::disconnected(void)
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_ECG);
    if (alarmSource)
    {
        alarmSource->clear();
        alarmSource->setOneShotAlarm(ECG_ONESHOT_ALARM_COMMUNICATION_STOP, true);
    }
    ecgParam.updateHR(InvData());

    QList<int> waveID;
    bool needFreshWave = false;
    for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; ++i)
    {
        ecgParam.setLeadOff((ECGLead)i, true);
        if (-1 != waveID.indexOf(ecgParam.leadToWaveID((ECGLead)i)))
        {
            needFreshWave = true;
        }
    }

    if (_isSupportRESP)
    {
        alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_RESP);
        if (alarmSource)
        {
            alarmSource->clear();
            alarmSource->setOneShotAlarm(RESP_ONESHOT_ALARM_COMMUNICATION_STOP, true);
        }
        respParam.setRR(InvData());
        if (-1 != waveID.indexOf(WAVE_RESP))
        {
            needFreshWave = true;
        }
    }
    ecgParam.setConnected(false);
    respParam.setConnected(false);
    Q_UNUSED(needFreshWave)
}

/**************************************************************************************************
 * 连接恢复。
 *************************************************************************************************/
void E5Provider::reconnected(void)
{
    AlarmOneShotIFace *ecgOneShotAlarm = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_ECG);
    if (ecgOneShotAlarm)
    {
        ecgOneShotAlarm->setOneShotAlarm(ECG_ONESHOT_ALARM_COMMUNICATION_STOP, false);
    }
    AlarmOneShotIFace *respOneShotAlarm = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_RESP);
    if (respOneShotAlarm)
    {
        respOneShotAlarm->setOneShotAlarm(RESP_ONESHOT_ALARM_COMMUNICATION_STOP, false);
    }

    QList<int> waveID;
    bool needFreshWave = false;
    for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; ++i)
    {
        ecgParam.setLeadOff((ECGLead)i, true);
        if (-1 != waveID.indexOf(ecgParam.leadToWaveID((ECGLead)i)))
        {
            needFreshWave = true;
        }
    }

    if (_isSupportRESP)
    {
        if (respOneShotAlarm)
        {
            respOneShotAlarm->clear();
            respOneShotAlarm->setOneShotAlarm(RESP_ONESHOT_ALARM_COMMUNICATION_STOP, false);
        }
        respParam.setRR(InvData());
        if (-1 != waveID.indexOf(WAVE_RESP))
        {
            needFreshWave = true;
        }
    }
    ecgParam.setConnected(true);
    respParam.setConnected(true);
    Q_UNUSED(needFreshWave)
}

void E5Provider::sendDisconnected()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_ECG);
    if (alarmSource && !alarmSource->isAlarmed(ECG_ONESHOT_ALARM_COMMUNICATION_STOP))
    {
        alarmSource->setOneShotAlarm(ECG_ONESHOT_ALARM_SEND_COMMUNICATION_STOP, true);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
E5Provider::E5Provider() : BLMProvider("BLM_E5"), ECGProviderIFace(), _waveSampleRate(WAVE_SAMPLE_RATE_250),
    _isFristConnect(false), _isRespFirstConnect(false), _isSupportRESP(systemManager.isSupport(CONFIG_RESP))
{
    UartAttrDesc portAttr(460800, 8, 'N', 1);
    initPort(portAttr);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
E5Provider::~E5Provider()
{
}
