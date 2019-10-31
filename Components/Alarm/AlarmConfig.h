/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#pragma once
#include <QObject>
#include "UnitManager.h"
#include "ParamDefine.h"
#include <QPair>
#include <QHash>
#include "PatientDefine.h"
#include "AlarmDefine.h"

struct LimitAlarmConfig
{
    LimitAlarmConfig()
        : highLimit(0), maxHighLimit(0), minHighLimit(0),
          lowLimit(0), maxLowLimit(0), minLowLimit(0),
          scale(1), step(1)
    {}

    bool operator == (const LimitAlarmConfig &other) const
    {
        return this->highLimit == other.highLimit
               && this->lowLimit == other.lowLimit;
    }

    int highLimit;
    int maxHighLimit;
    int minHighLimit;
    int lowLimit;
    int maxLowLimit;
    int minLowLimit;
    int scale;
    int step;
};

struct LimitAlarmControl
{
    LimitAlarmControl()
        : priority(ALARM_PRIO_PROMPT), enable(false) {}

    bool operator == (const LimitAlarmControl &other) const
    {
        return  this->enable == other.enable
                && this->priority == other.priority;
    }

    AlarmPriority priority;
    bool enable;
};

class AlarmConfig : public QObject
{
    Q_OBJECT
public:
    static AlarmConfig &getInstance();

    // check whether limit alarm is enable
    bool isLimitAlarmEnable(SubParamID subParamId);

    /**
     * @brief hasLimitAlarmDisable 是否有参数报警被关闭
     * @return
     */
    bool hasLimitAlarmDisable();

    // set limit alarm enable or not
    void setLimitAlarmEnable(SubParamID subParamId, bool enable);

    // get the limit alarm priority
    AlarmPriority getLimitAlarmPriority(SubParamID subParamId);

    // set the limit alarm priority
    void setLimitAlarmPriority(SubParamID subParamId, AlarmPriority prio);

    // get the alarm config
    LimitAlarmConfig getLimitAlarmConfig(SubParamID subParamId, UnitType unit);


    // set the alarm config
    void setLimitAlarmConfig(SubParamID subParamId, UnitType unit, const LimitAlarmConfig &config);

    // utility function to convert the high limit to string
    static QString getHighLimitStr(const LimitAlarmConfig &config);

    // utility function to convert the low limit to string
    static QString getLowLimitStr(const LimitAlarmConfig &config);

    /**
     * @brief clearLimitAlarmInfo  清除报警限信息
     */
    void clearLimitAlarmInfo();

signals:
    void alarmOff(SubParamID subParamId);

private slots:
    void onPatientTypeChange(PatientType type);


private:
    typedef QPair<SubParamID, UnitType> AlarmConfigKey;
    typedef QHash<AlarmConfigKey, LimitAlarmConfig> LimitAlarmConfigCache;
    typedef QHash<SubParamID, LimitAlarmControl> LimitAlarmControlCache;

    LimitAlarmConfigCache _configCache;
    LimitAlarmControlCache _controlCache;
    AlarmConfig();
};

#define alarmConfig AlarmConfig::getInstance()

