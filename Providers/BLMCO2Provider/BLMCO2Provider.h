/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/21
 **/

#pragma once
#include "Provider.h"
#include "CO2ProviderIFace.h"
#include "BLMProvider.h"

struct CO2ProviderStatus
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
    int lastBrethTime;          // Time since last breath.
    char primaryAgent;          // Primary agent ID.   (x)
    char secondaryAgent;        // Secondary agent ID. (x)
    short atmPress;             // 500--1300hPa, 1 std atm = 1013.25 hPa

    // --------------------------------------------------------------------------------------------
    // --------------------------- Sensor register(ID 0x04) ---------------------------------------
    // --------------------------------------------------------------------------------------------
    // Sensor mode register (byte 0)
    unsigned char workMode;    // MODE, 0:self test, 1:sleep, 2:measurement

    // O2 compensation conc (byte 1)
    unsigned char o2CompensationConc;  // 0--115

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
    // --------------------------- Sensor register(ID 0x05) ---------------------------------------
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
    int hardwaveVersion;        // coded as two BCD characters assembled into a byte.

    // Sensor software revision (byte 2 and 3)
    int softwaveVersion;        // coded as four BCD characters assembled into a word.

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

class BLMCO2Provider: public Provider, public CO2ProviderIFace
{
    Q_OBJECT
public:     // Provider的接口。
    virtual bool attachParam(Param *param);
    virtual void dataArrived(void);
    /* reimplment */
    virtual void dataArrived(unsigned char *data, unsigned int length);
    virtual void sendVersion(void) {}

public:     // CO2ProviderIFace 的接口。
    // 校零。
    virtual void zeroCalibration(void);

    // 设置窒息超时时间。
    virtual void setApneaTimeout(ApneaAlarmTime t);

    // 设置N2O补偿。
    virtual void setN2OCompensation(int comp);

    // 设置O2补偿。
    virtual void setO2Compensation(int comp);

    // 获取波形的采样速度。
    virtual int getCO2WaveformSample(void);

    // 获取波形基线
    virtual int getCO2BaseLine(void) {return 0;}

    // 获取最大的波形值。
    virtual int getCO2MaxWaveform(void);

    // 设置工作模式
    virtual void setWorkMode(CO2WorkMode mode);

    // 进入升级模式
    virtual void enterUpgradeMode();

    virtual void sendCalibrateData(int value);

    /**
     * @brief setUpgradeIface set the upgrade iface to handle the upgrade packet
     * @note set iface to NULL to stop upgrade
     * @param iface the upgrade iface
     */
    void setUpgradeIface(BLMProviderUpgradeIface *iface);

    // 发送升级命令
    bool sendUpgradeCmd(unsigned char cmdId, const unsigned char *data, unsigned int len);

    // 构造与析构。
    explicit BLMCO2Provider(const QString &name);
    ~BLMCO2Provider();

    /**
     * @brief isConnectModel 是否连接了CO2模块
     * @return
     */
    bool isConnectModel();

protected:
    virtual void disconnected(void);
    virtual void reconnected(void);

private slots:
    void connectTimeOut();

private:
    void _unpacket(const unsigned char packet[]);

    // 协议数据校验
    bool _checkUpgradePacketValid(const unsigned char *data, unsigned int len);

    // 读取升级相关数据数据导RingBuff中
    void _readUpgradeData(void);

    // 发送升级数据
    bool _sendUpgradeData(const unsigned char *data, unsigned int len);

    static const int _packetLen = 21;      // 数据包长度。
    CO2ProviderStatus _status;
    short _etco2Value;  // 缓存EtCO2的值。
    short _fico2Value;  // 缓存FiCO2的值。

    BLMProviderUpgradeIface *upgradeIface;
    bool _isLastSOHPaired;  // 遗留在ringBuff最后一个数据（该数据为SOH）是否已经剃掉了多余的SOH。

    QTimer connectTmr;
    bool co2ModelConnect;
    bool isPlugin;              // 是否为插件模块
};
