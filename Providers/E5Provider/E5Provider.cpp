/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/9
 **/

#include "E5Provider.h"
#include "ECGParam.h"
#include "RESPParam.h"
#include "PatientDefine.h"
#include "ECGAlg2SoftInterface.h"
#include "Uart.h"
#include "ECGParam.h"
#include "ECGAlarm.h"
#include "RESPAlarm.h"
#include "SystemManager.h"
#include <Debug.h>
#include <QTextStream>
#include <QFile>
#include "IConfig.h"
#include "RawDataCollector.h"

enum E5RecvPacketType
{
    E5_RSP_VERSION              = 0x11,         // version response

    E5_RSP_SELFLEARN_ONOFF      = 0x13,         // get selflearn onoff
    E5_RSP_LEAD_MODE            = 0x17,         // get lead mode
    E5_RSP_CALC_MODE            = 0x19,         // get calculation mode
    E5_RSP_NOTCH_TYPE           = 0x1B,         // get notch type
    E5_RSP_CALC_LEAD            = 0x1F,         // get calculation lead
    E5_RSP_PACE_ONOFF           = 0x21,         // get pace onoff
    E5_RSP_PATIENT_TYPE         = 0x23,         // get patient type

    E5_RSP_ST_TEST              = 0x33,         // get ST test
    E5_RSP_ST_ISO_POS           = 0x35,         // get ST ISO position
    E5_RSP_ST_J_POS             = 0x37,         // get ST J position
    E5_RSP_ST_STANDJ_POS        = 0x39,         // get ST ST and J position

    E5_RSP_RESP_APNEA_TIME      = 0x4F,         // response of set resp apnea time
    E5_RSP_RESP_CALC_LEAD       = 0x51,         // response of set resp calculate lead
    E5_RSP_RESP_CALC_SWITCH     = 0x61,         // response of set resp calculate switch

    E5_RSP_SELFTEST_RESULT      = 0x7F,         // selftest result

    E5_NOTIFY_SYSTEM_START      = 0x80,         // module start notification
    E5_NOTIFY_ST_RESULT         = 0x90,         // ST result
    E5_NOTIFY_ST_WAVE_RESULT    = 0x91,         // ST wave result
    E5_NOTIFY_ASYS_ALARM        = 0xA1,         // notify asys alarm status
    E5_PERIODIC_ALIVE           = 0xB0,         // module alive packet, receive frequency will be 1Hz
    E5_PERIODIC_TXT_DATA        = 0xB9,         // store TXT data
    E5_PERIODIC_ECG_DATA        = 0xBA,         // ecg data
    E5_PERIODIC_HR              = 0xBB,         // HR, -1 means invalid
    E5_PERIODIC_RESP_DATA       = 0xD0,         // resp data
    E5_PERIODIC_RR              = 0xD1,         // respiration rate
    E5_NOTIFY_RESP_ALARM        = 0xD2,         // notify resp alarm status
    E5_PERIODIC_TEMP_DATA       = 0xE0,         // temp data
    E5_STATUS_ERROR_OR_WARN     = 0xF0,         // error or warning
};

enum E5SendPacketType
{
    E5_CMD_GET_VERSION          = 0x10,         // get version

    E5_CMD_SET_SELFLEARN_ONOFF  = 0x12,         // set selflearn onoff
    E5_CMD_SET_LEAD_MODE        = 0x16,         // set lead mode
    E5_CMD_SET_CALC_MODE        = 0x18,         // set calculation mode
    E5_CMD_SET_NOTCH_TYPE       = 0x1A,         // set notch type
    E5_CMD_SET_CALC_LEAD        = 0x1E,         // set calculation lead
    E5_CMD_SET_PACE_ONOFF       = 0x20,         // set pace onoff
    E5_CMD_SET_PATIENT_TYPE     = 0x22,         // set patient type

    E5_CMD_SET_ST_TEST          = 0x32,         // set ST test
    E5_CMD_SET_ST_ISO_POS       = 0x34,         // set ST ISO position
    E5_CMD_SET_ST_J_POS         = 0x36,         // set ST J position
    E5_CMD_SET_ST_STANDJ_POS    = 0x38,         // set ST ST and J position

    E5_CMD_SET_RESP_APNEA_TIME  = 0x4E,         // set resp apnea time
    E5_CMD_SET_RESP_CALC_LEAD   = 0x50,         // set resp calculate lead
    E5_CMD_SET_RESP_CALC_SWITCH = 0x60,         // set resp calculate switch

    E5_CMD_GET_SELFTEST_RESULT  = 0x7E,         // get system test result

    E5_CMD_UPGRADE              = 0xF6,         // enter upgrade mode
};

class E5ProviderPrivate
{
public:
    explicit E5ProviderPrivate(E5Provider *const q_ptr)
        : q_ptr(q_ptr),
          lastLeadOff(1),
          support12Lead(false),
          ecgLeadMode(ECG_LEAD_MODE_5),
          respApneaTime(APNEA_ALARM_TIME_OFF),
          resplead(RESP_LEAD_II),
          enableRespCalc(false)
    {
        qMemSet(selftestResult, 0, sizeof(selftestResult));
    }

    void handleEcgRawData(unsigned char *data, int len);
    void handleRESPRawData(unsigned char *data, int len);

    E5Provider *const q_ptr;
    short lastLeadOff;
    bool support12Lead;
    char selftestResult[8];
    ECGLeadMode ecgLeadMode;
    ApneaAlarmTime respApneaTime;
    RESPLead resplead;
    bool enableRespCalc;
};

/* parse the data from the module and pass it to the algorithm interface */
void E5ProviderPrivate::handleEcgRawData(unsigned char *data, int len)
{
    Q_ASSERT(len == 524);

    for (int n = 0; n < 20; n++)
    {
        int leadData[ECG_LEAD_NR] = {0};
        bool leadOFFStatus[ECG_LEAD_NR] = {0};
        short leadoff;
        bool qrsflag;
        bool paceflag;
        bool overload;
        leadoff = ((data[4 + n * 26] & 0x3) << 8) | data[5 + n * 26];
        paceflag = data[4 + n * 26] & 0x10;
        qrsflag = data[4 + n * 26] & 0x40;
        overload = data[4 + n * 26] & 0x20;

        for (int j = 0; j < ECG_LEAD_NR; j++)
        {
            short v = data[6 + n * 26 + j * 2] | (data[7 + n * 26 + j * 2] << 8);
            leadData[j] = v;
        }

        if (ecgParam.getLeadMode() == ECG_LEAD_MODE_5)
        {
            leadoff &= 0xFFE0;
        }

        if (ecgParam.getLeadMode() == ECG_LEAD_MODE_3)
        {
            leadoff &= 0xFFC0;
        }

        if (leadoff & 0x3C0) // RL/LA/LL/RA
        {
            // all lead will be off if any limb lead is off
            for (int j = 0; j < ECG_LEAD_NR; j++)
            {
                leadData[j] = 0;
                leadOFFStatus[j] = true;
            }
        }
        else
        {
            if (leadoff & 0x20)                // bit5 V1
            {
                leadData[ECG_LEAD_V1] = 0;
                leadOFFStatus[ECG_LEAD_V1] = true;
            }
            if (leadoff & 0x10)                // bit4 V2
            {
                leadData[ECG_LEAD_V2] = 0;
                leadOFFStatus[ECG_LEAD_V2] = true;
            }
            if (leadoff & 0x08)                // bit3 V3
            {
                leadData[ECG_LEAD_V3] = 0;
                leadOFFStatus[ECG_LEAD_V3] = true;
            }
            if (leadoff & 0x04)                // bit2 V4
            {
                leadData[ECG_LEAD_V4] = 0;
                leadOFFStatus[ECG_LEAD_V4] = true;
            }
            if (leadoff & 0x02)               // bit1 V5
            {
                leadData[ECG_LEAD_V5] = 0;
                leadOFFStatus[ECG_LEAD_V5] = true;
            }
            if (leadoff & 0x01)               // bit0 V6
            {
                leadData[ECG_LEAD_V6] = 0;
                leadOFFStatus[ECG_LEAD_V6] = true;
            }
        }

        if (lastLeadOff != leadoff)
        {
            lastLeadOff = leadoff;

            for (int i = 0; i < ECG_LEAD_NR; i++)
            {
                ecgParam.setLeadOff((ECGLead)i, leadOFFStatus[i]);
            }
        }

        ecgParam.setOverLoad(overload);
        ecgParam.updateWaveform(leadData, leadOFFStatus, paceflag, false, qrsflag);
    }
}

void E5ProviderPrivate::handleRESPRawData(unsigned char *data, int /*len*/)
{
    bool leadOff = data[0] & 0x80;
    int resp;
    int flag = 0;
    if (leadOff)
    {
        flag |= INVALID_WAVE_FALG_BIT;
        resp = q_ptr->getRESPBaseLine();
        respParam.setLeadoff(true);
    }
    else
    {
        short waveValue = ((data[0] & 0x7F) << 8) | data[1];

        if (waveValue & 0x4000)
        {
            // negative value
            waveValue |= 0x8000;
        }
        resp = waveValue;
        respParam.setLeadoff(false);
    }
    respParam.addWaveformData(resp, flag);
}

E5Provider::E5Provider()
    : BLMProvider("BLM_E5"),
      d_ptr(new E5ProviderPrivate(this))
{
    // UartAttrDesc portAttr(460800, 8, 'N', 1, 0, FlOW_CTRL_HARD);
    UartAttrDesc portAttr(230400, 8, 'N', 1);
    initPort(portAttr);

    // set lead mode
    int leadMode;
    currentConfig.getNumValue("ECG|LeadMode", leadMode);
    setLeadSystem(static_cast<ECGLeadMode>(leadMode));

    // set calculation lead
    int calcLead;
    currentConfig.getNumValue("ECG|CalcLead", calcLead);
    setCalcLead(static_cast<ECGLead>(calcLead));

    // get selftest result
    sendCmd(E5_CMD_GET_SELFTEST_RESULT, NULL, 0);
}

E5Provider::~E5Provider()
{
}

bool E5Provider::attachParam(Param &param)
{
    bool ret = false;
    if (param.getParamID() == PARAM_ECG)
    {
        // TODO: fix the relation between param and provider, should use the abstrace class to
        //      set the provider, in other word, setprovider should be virtual.
        ecgParam.setProvider(this);
        ret = true;
    }
    else if (param.getParamID() == PARAM_RESP)
    {
        respParam.setProvider(this);
        ret = true;
    }
    if (ret)
    {
        Provider::attachParam(param);
    }
    return ret;
}

void E5Provider::handlePacket(unsigned char *data, int len)
{
    if (!isConnectedToParam)
    {
        return;
    }

    BLMProvider::handlePacket(data, len);

    if (!isConnected)
    {
        ecgParam.setConnected(true);
        respParam.setConnected(true);
    }

    switch (data[0])
    {
    case E5_RSP_VERSION:
        break;
    case E5_RSP_SELFLEARN_ONOFF:
        break;
    case E5_RSP_SELFTEST_RESULT:
        qMemCopy(d_ptr->selftestResult, data + 1, sizeof(d_ptr->selftestResult));
        break;
    case E5_RSP_LEAD_MODE:
        break;
    case E5_RSP_CALC_MODE:
        break;
    case E5_RSP_CALC_LEAD:
        break;
    case E5_RSP_NOTCH_TYPE:
        break;
    case E5_RSP_PATIENT_TYPE:
        break;
    case E5_RSP_PACE_ONOFF:
        break;
    case E5_RSP_RESP_CALC_LEAD:
        break;
    case E5_RSP_RESP_APNEA_TIME:
        break;
    case E5_RSP_RESP_CALC_SWITCH:
        break;
    case E5_NOTIFY_SYSTEM_START:
    {
        // module restart, perfrom initialize

        // get selftest result
        sendCmd(E5_CMD_GET_SELFTEST_RESULT, NULL, 0);

        // set lead mode again
//        setLeadSystem(d_ptr->ecgLeadMode);

        // resp setting
        setRESPCalcLead(d_ptr->resplead);
        // resp apnea time
        setApneaTime(d_ptr->respApneaTime);
        // resp calculation
        enableRESPCalc(d_ptr->enableRespCalc);
    }
    break;
    case E5_NOTIFY_RESP_ALARM:
        // apnea alarm status
        if (data[1])
        {
            respOneShotAlarm.setOneShotAlarm(RESP_ONESHOT_ALARM_APNEA, true);
        }
        else
        {
            respOneShotAlarm.setOneShotAlarm(RESP_ONESHOT_ALARM_APNEA, false);
        }
        break;
    case E5_PERIODIC_ALIVE:
        feed();
        break;
    case E5_PERIODIC_ECG_DATA:
        rawDataCollector.collectData(RawDataCollector::ECG_DATA, data + 1, len -1);
        d_ptr->handleEcgRawData(data + 1, len - 1);
        break;
    case E5_PERIODIC_HR:
    {
        short hr = data[1] + (data[2] << 8);
        ecgParam.updateHR(hr);
    }
    break;
    case E5_PERIODIC_RESP_DATA:
        d_ptr->handleRESPRawData(data + 1, len - 1);
        break;
    case E5_PERIODIC_RR:
    {
        short rr = data[1] + (data[2] << 8);
        if (rr == -1)
        {
            rr = InvData();
        }
        respParam.setRR(rr);
    }
    break;
    case E5_STATUS_ERROR_OR_WARN:
        break;
    default:
        qdebug("unknown packet type 0x%02x", data[0]);
        break;
    }
}

void E5Provider::sendVersion()
{
    sendCmd(E5_CMD_GET_VERSION, NULL, 0);
}

/* module disconnect */
void E5Provider::disconnected()
{
    ecgOneShotAlarm.clear();
    respOneShotAlarm.clear();
    ecgOneShotAlarm.setOneShotAlarm(ECG_ONESHOT_ALARM_COMMUNICATION_STOP, true);
    respOneShotAlarm.setOneShotAlarm(RESP_ONESHOT_ALARM_COMMUNICATION_STOP, true);
    for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; i++)
    {
        ecgParam.setLeadOff((ECGLead)i, true);
    }
    respParam.setLeadoff(true);

    systemManager.setPoweronTestResult(E5_MODULE_SELFTEST_RESULT, SELFTEST_FAILED);
    ecgParam.setConnected(false);
    respParam.setConnected(false);
}


void E5Provider::reconnected()
{
    ecgOneShotAlarm.setOneShotAlarm(ECG_ONESHOT_ALARM_COMMUNICATION_STOP, false);
    respOneShotAlarm.setOneShotAlarm(RESP_ONESHOT_ALARM_COMMUNICATION_STOP, false);
    ecgParam.setConnected(true);
    respParam.setConnected(true);
}

int E5Provider::getWaveformSample()
{
    return 250;
}

void E5Provider::setWaveformSample(int rate)
{
    Q_UNUSED(rate)
}

void E5Provider::get05mV(int &p05mv, int &n05mv)
{
    // about 1.224 uV per unit
    p05mv = 408;
    n05mv = -408;
}

void E5Provider::getLeadCabelType()
{
    // TODO
}

void E5Provider::setLeadSystem(ECGLeadMode leadSystem)
{
    d_ptr->ecgLeadMode = leadSystem;
    unsigned char dataPayload = (unsigned char) leadSystem;
    sendCmd(E5_CMD_SET_LEAD_MODE, &dataPayload, 1);
}

void E5Provider::setCalcLead(ECGLead lead)
{
    unsigned char calcLead = lead;
    sendCmd(E5_CMD_SET_CALC_LEAD, &calcLead, 1);
}

void E5Provider::setPatientType(unsigned char type)
{
    unsigned char patientType = type;
    sendCmd(E5_CMD_SET_PATIENT_TYPE, &patientType, 1);
}

void E5Provider::setBandwidth(ECGBandwidth bandwidth)
{
    ECGFilterMode mode;
    switch (bandwidth)
    {
    case ECG_BANDWIDTH_067_20HZ:
        mode = ECG_FILTERMODE_SURGERY;
        break;
    case ECG_BANDWIDTH_067_40HZ:
        mode = ECG_FILTERMODE_MONITOR;
        break;
    case ECG_BANDWIDTH_0525_40HZ:
        mode = ECG_FILTERMODE_ST;
        break;
    case ECG_BANDWIDTH_0525_150HZ:
        mode = ECG_FILTERMODE_DIAGNOSTIC;
        break;
    default:
        mode = ECG_FILTERMODE_MONITOR;
        break;
    }

    unsigned char filterMode = mode;
    sendCmd(E5_CMD_SET_CALC_MODE, &filterMode, 1);
}

void E5Provider::setFilterMode(ECGFilterMode mode)
{
    unsigned char filterMode = mode;
    sendCmd(E5_CMD_SET_CALC_MODE, &filterMode, 1);
}

void E5Provider::enablePacermaker(ECGPaceMode onoff)
{
    unsigned char pacemaker = onoff;
    sendCmd(E5_CMD_SET_PACE_ONOFF, &pacemaker, 1);
}

void E5Provider::setNotchFilter(ECGNotchFilter notch)
{
    unsigned char notchChar = notch;
    sendCmd(E5_CMD_SET_NOTCH_TYPE, &notchChar, 1);
}

void E5Provider::enableSTAnalysis(bool onoff)
{
    Q_UNUSED(onoff)
    // TODO
}

void E5Provider::setSTPoints(int /*iso*/, int /*st*/)
{
    // TODO
}

void E5Provider::setSelfLearn(bool onOff)
{
    unsigned char learn = onOff;
    sendCmd(E5_CMD_SET_SELFLEARN_ONOFF, &learn, 1);
}

void E5Provider::setARRThreshold(ECGAlg::ARRPara parameter, short value)
{
    Q_UNUSED(parameter)
    Q_UNUSED(value)
}

int E5Provider::getRESPWaveformSample()
{
    return 125;
}

void E5Provider::disableApnea()
{
    d_ptr->respApneaTime = APNEA_ALARM_TIME_OFF;
    unsigned char apneaTime = d_ptr->respApneaTime;
    sendCmd(E5_CMD_SET_RESP_APNEA_TIME, &apneaTime, 1);
}

void E5Provider::setApneaTime(ApneaAlarmTime t)
{
    d_ptr->respApneaTime = t;
    unsigned char apneaTime = t;
    sendCmd(E5_CMD_SET_RESP_APNEA_TIME, &apneaTime, 1);
}

void E5Provider::setWaveformZoom(RESPZoom /*zoom*/)
{
    // TODO
}

void E5Provider::setRESPCalcLead(RESPLead lead)
{
    d_ptr->resplead = lead;
    unsigned char l = lead;
    sendCmd(E5_CMD_SET_RESP_CALC_LEAD, &l, 1);
}

void E5Provider::enableRESPCalc(bool enable)
{
    d_ptr->enableRespCalc = enable;
//    unsigned char onOff = enable ? 1 : 0;
//    sendCmd(E5_CMD_ENABLE_RESP_CALC, &onOff, 1);
}

