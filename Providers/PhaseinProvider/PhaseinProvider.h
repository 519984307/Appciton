/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/26
 **/

#pragma once
#include "Provider.h"
#include "AGProviderIFace.h"
#include <QFile>
#include <QTextStream>

enum  // 数据包类型。
{
    INSP_VALS = 0x00,
    EXP_VALS = 0x01,
    MOM_VALS = 0x02,
    GEN_VALS = 0x03,
    SENSOR_REGS = 0x04,
    CONFIG_DATA = 0x05,
    SERVICE_DATA = 0x06,
};

enum  // 设置命令。
{
    CMD_SET_MODE = 0x00,        // 设置工作模式
    CMD_SET_APNE_TIME = 0x01,   // 设置窒息时间
    CMD_SET_AGENT_ID = 0x02,    // 设置主麻醉类型ID
    CMD_SET_O2 = 0x04,          // 设置氧气补偿的浓度
    CMD_SET_N20 = 0x05,         // 设置笑气补偿的浓度
    CMD_ZERO_CAL = 0x06,        // 模块校零命令。
};

struct  AGProviderStatus
{
    // --------------------------------------------------------------------------------------------
    // ------------------------------ STS (Status summary) ----------------------------------------
    // ---------------------------------- 未使用 ---------------------------------------------------
    bool breathDetect;          // BDET,       1 = Breath detected (cleared after transmission).
    bool noBreath;              // NO_BREATHS, 1 = No breaths detected within selected time.
    bool o2Replace;             // O2_REPL,    1 = Replace O2 sensor.
    bool checkAdapt;            // CHK_ADAPT,  1 = Mainstream: Check adapter.
    //             1 = Sidestream: Check sampling line.
    bool unspecAcc;             // UNSPEC_ACC, 1 = At least one parameter is out of range,
    //             or zero calibration of IR measurement is required.
    //             Validity of data may be questionable.
    bool sensorErr;             // SENS_ERR,   sensor err.
    bool o2Calib;               // O2_CALIB,   1 = Mainstream: Room air O2 calibration required
    //             1 = Sidestream: O2 span calibration required

    // --------------------------------------------------------------------------------------------
    // -------------------------------- GenVals (ID 0x03) -----------------------------------------
    // --------------------------------------------------------------------------------------------
    unsigned char respRate;                 // Respiratory rate.
    int lastBrethTime;                      // Time since last breath.
    unsigned char primaryAgent;             // Primary agent ID.   (x)
    unsigned char secondaryAgent;           // Secondary agent ID. (x)
    unsigned short atmPress;                // 500--1300hPa, 1 std atm = 1013.25 hPa

    // --------------------------------------------------------------------------------------------
    // --------------------------- Sensor register(ID 0x04) ---------------------------------------
    // --------------------------------------------------------------------------------------------
    // Sensor mode register (byte 0)
    unsigned char workMode;    // MODE, 0:self test, 1:sleep, 2:measurement

    // O2 compensation conc (byte 1)
    unsigned char o2CompensationConc;   // 0--115

    // Sensor error register (byte 2)
    bool softwareErr;          // SW_ERR, Software error. Restart sensor.
    bool hardwareErr;          // SW_ERR, Hardware error. Replace sensor.
    bool motorSpeedOut;        // MFAIL, Motor speed out of bounds.
    bool uncalibrated;         // UNCAL, Factory calibration lost/missing.
    bool o2Error;              // O2_ERR, 1 = Mainstream: N/A
    //         1 = Sidestream: O2 sensor error

    // Adapter status register (byte 3)
    bool replaceAdapt;          // REPL_ADAPT, 1 = Mainstream: Replace adapter (IR signal low).
    //             1 = Sidestream: Sampling line clogged.
    bool noAdapt;               // NO_ADAPT,   1 = Mainstream: No adapter. Connect adapter(IR signal high).
    //             1 = Sidestream: No sampling line.
    bool o2Clogged;             // O2_CLG,     1 = Mainstream: O 2 port failure (clogged or plugged)
    //             1 = Sidestream: N/A

    // Data valid register (byte 4)
    bool co2OR;                // CO2_OR,  1 = CO2 outside specified accuracy range.
    bool n2oOR;                // N2O_OR,  1 = N2O outside specified accuracy range.
    bool axOR;                 // AX_OR,  1 = At least one agent outside specified accuracy range.
    bool o2OR;                 // O2_OR,  1 = O 2 outside specified accuracy range.
    bool tempOR;               // TEMP_OR, 1 = Internal temperature outside operating range.
    bool pressOR;              // PRESS_OR, 1 = Ambient pressure outside operating range.
    bool zeroRequired;         // ZERO_REQ, 1 = Inaccurate zero reference. Zeroing required .
    bool agentUnreliable;      // AGENT_ID_UNRELIABLE, 1 = Agent identification and concentrations are unreliable.

    // No-breaths timeout value (byte 5)
    unsigned char apneaTime;   // apnea time: 20-60 s

    // --------------------------------------------------------------------------------------------
    // --------------------------- Configuration data(ID 0x05) ---------------------------------------
    // --------------------------------------------------------------------------------------------
    // Sensor configuration register 0 (byte 0)
    bool o2Config;              // O2_CFG
    bool co2Config;             // CO2_CFG
    bool n2oConfig;             // N2O_CFG
    bool halConfig;             // HAL_CFG
    bool enfConfig;             // ENF_CFG
    bool isoConfig;             // ISO_CFG
    bool sevConfig;             // SEV_CFG
    bool desConfig;             // DES_CFG

    // Sensor hardware revision (byte 1)
    int hardwareVersion;        // coded as two BCD characters assembled into a byte.

    // Sensor software revision (byte 2 and 3)
    int softwareVersion;        // coded as four BCD characters assembled into a word.

    // Sensor configuration register 1 (byte 4)
    bool axIDConfig;            // ID_CFG, agent id option fitted, auto agent id transmit
    bool n2oCompensationAvaiable;   // N2O_COMP, 1 = Host N2O compensation available.
    // When this bit is set, it is possible to set the N2O concentration
    // from host using the command SetN2O.
    int o2SensorType;           // O2_SENSOR_TYPE, 0:galvanic o2, 1:servomex pm1116(isa only),
    // 2,3:reserved
    bool scientificConfig;      // SCIENTIFIC_CFG, Always scientific mode
    bool sidestreamConfig;      // SS_CFG, Sidestream model

    // Communication protocol version (byte 5)
    int commProtocolVersion;    // communication protocol rev

    // --------------------------------------------------------------------------------------------
    // ------------------------ Service data (ID 0x06) layouts ------------------------------------
    // --------------------------------------------------------------------------------------------
    // Sensor s/n (byte 0, 1 and 4)
    int sensorSN;               // sensor serial number

    // Service status register (byte 2). NOTES: service status!! important!!
    bool zeroDisable;           // ZERO_DISAB
    bool zeroInProgress;        // ZERO_IN_PROGRESS
    bool spanError;             // SPAN_ERR, 1 = Mainstream: N/A
    //           1 = Sidestream: Latest span calibration command failed
    bool spanCalibInProgress;   // SPAN_CALB_IN_PROGRESS, 1 = Mainstream: N/A
    //                        1 = Sidestream: Span calibration and
    //                            validation in progress
    char irO2Delay;             // 0--15

    // (byte 3)
    unsigned char n2oCompensationConc;  // compensation concentration

    // (byte 5)
    int cuvetteAdjuestPress;  // only for sidestream maintenance.
};

class PhaseinProvider: public Provider, public AGProviderIFace
{
public:    // Provider interface
    bool attachParam(Param *param);
    void dataArrived(void);

    // send command.
    void sendCmd(unsigned char dataId, const unsigned char data);

    virtual void sendVersion(void) {}
    virtual void disconnected(void);          // 模块连接断开时回调，之类实现。
    virtual void reconnected(void);          // 模块连接恢复时回调，之类实现。

public:    // AGProviderIFace interface
    // set work mode.
    virtual void setWorkMode(AGWorkMode);

    // set no-breaths timeout.
    virtual void setApneaTimeout(ApneaAlarmTime);

    // set primary agent ID.
    virtual void setPrimaryID(AGAnaestheticType);

    // set O2 compensation.
    virtual void setO2Compensation(int comp);

    // set N2O compensation.
    virtual void setN2OCompensation(int comp);

    // zero calibration.
    virtual void zeroCalibration(void);

    // get waveform sample
    virtual int getN2OWaveformSample(void)
    {
        return 50;
    }
    virtual int getAA1WaveformSample(void)
    {
        return 50;
    }
    virtual int getAA2WaveformSample(void)
    {
        return 50;
    }
    virtual int getO2WaveformSample(void)
    {
        return 50;
    }

    // get base line.
    virtual int getN2OBaseLine(void)
    {
        return 0;
    }
    virtual int getAA1BaseLine(void)
    {
        return 0;
    }
    virtual int getAA2BaseLine(void)
    {
        return 0;
    }
    virtual int getO2BaseLine(void)
    {
        return 0;
    }

    // get max waveform value.
    virtual int getN2OMaxWaveform(void)
    {
        return 11500;
    }
    virtual int getAA1MaxWaveform(void)
    {
        return 2500;
    }
    virtual int getAA2MaxWaveform(void)
    {
        return 2500;
    }
    virtual int getO2MaxWaveform(void)
    {
        return 11500;
    }

public:
    PhaseinProvider();
    ~PhaseinProvider();

private:
    void _unpacket(const unsigned char packet[]);
    static const int _packetLen = 21;      // 数据包长度。
    static const int _maxPacketLen = 5;     // max send packet len
    AGProviderStatus _status;
};
