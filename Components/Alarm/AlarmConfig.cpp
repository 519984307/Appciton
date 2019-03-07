/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/


#include "AlarmConfig.h"
#include "PatientManager.h"
#include "ParamInfo.h"
#include "IConfig.h"
#include "ConfigManager.h"
#include "SystemManager.h"
#include "ParamInfo.h"

AlarmConfig &AlarmConfig::getInstance()
{
    static AlarmConfig *instance = NULL;
    if (instance == NULL)
    {
        instance = new AlarmConfig();
    }
    return *instance;
}

bool AlarmConfig::isLimitAlarmEnable(SubParamID subParamId)
{
    LimitAlarmControlCache::iterator iter = _controlCache.find(subParamId);
    if (iter == _controlCache.end())
    {
        LimitAlarmControl ctrl;

        // load data from config file
        QString prefix = "AlarmSource|";
        prefix += paramInfo.getSubParamName(subParamId, true);
        int v = 0;
        currentConfig.getNumAttr(prefix, "Enable", v);
        ctrl.enable = v;
        v = 0;
        currentConfig.getNumAttr(prefix, "Prio", v);
        ctrl.priority = (AlarmPriority)v;
        iter = _controlCache.insert(subParamId, ctrl);
    }
    return iter.value().enable;
}

bool AlarmConfig::hasLimitAlarmDisable()
{
    LimitAlarmControlCache::iterator iter = _controlCache.begin();
    for (; iter != _controlCache.end(); ++iter)
    {
        if (iter.value().enable == false && systemManager.isSupport(paramInfo.getParamID(iter.key())))
        {
            return true;
        }
    }
    return false;
}

void AlarmConfig::setLimitAlarmEnable(SubParamID subParamId, bool enable)
{
    if (isLimitAlarmEnable(subParamId) == enable)
    {
        return;
    }

    // update cache, the cache must be found here
    LimitAlarmControlCache::iterator iter = _controlCache.find(subParamId);
    iter->enable = enable;

    QString prefix = "AlarmSource|";
    prefix += paramInfo.getSubParamName(subParamId, true);

    // save to config file
    int val = enable;
    currentConfig.setNumAttr(prefix, "Enable", val);

    emit alarmOff(subParamId);
}

AlarmPriority AlarmConfig::getLimitAlarmPriority(SubParamID subParamId)
{
    LimitAlarmControlCache::iterator iter = _controlCache.find(subParamId);
    if (iter == _controlCache.end())
    {
        LimitAlarmControl ctrl;

        // load data from config file
        QString prefix = "AlarmSource|";
        prefix += paramInfo.getSubParamName(subParamId, true);
        int v = 0;
        currentConfig.getNumAttr(prefix, "Enable", v);
        ctrl.enable = v;
        v = 0;
        currentConfig.getNumAttr(prefix, "Prio", v);
        ctrl.priority = (AlarmPriority)v;
        iter = _controlCache.insert(subParamId, ctrl);
    }
    return iter.value().priority;
}

void AlarmConfig::setLimitAlarmPriority(SubParamID subParamId, AlarmPriority prio)
{
    if (getLimitAlarmPriority(subParamId) == prio)
    {
        return;
    }

    // update cache, the cache must be found here
    LimitAlarmControlCache::iterator iter = _controlCache.find(subParamId);
    iter->priority = prio;

    QString prefix = "AlarmSource|";
    prefix += paramInfo.getSubParamName(subParamId, true);

    // save to config file
    int val = prio;
    currentConfig.setNumAttr(prefix, "Prio", val);
}

LimitAlarmConfig AlarmConfig::getLimitAlarmConfig(SubParamID subParamId, UnitType unit)
{
    AlarmConfigKey key(subParamId, unit);
    LimitAlarmConfigCache::iterator iter = _configCache.find(key);
    if (iter == _configCache.end())
    {
        LimitAlarmConfig config;

        // load data from config file
        QString prefix = "AlarmSource|";
        prefix += paramInfo.getSubParamName(subParamId, true);
        prefix += "|";
        prefix += Unit::getSymbol(unit);
        prefix += "|";
        int v = 0;
        QString highPrefix = prefix + "High";
        currentConfig.getNumValue(highPrefix, v);
        config.highLimit = v;

        v = 0;
        currentConfig.getNumAttr(highPrefix, "Min", v);
        config.minHighLimit = v;

        v = 0;
        currentConfig.getNumAttr(highPrefix, "Max", v);
        config.maxHighLimit = v;

        v = 0;
        QString lowPrefix = prefix + "Low";
        currentConfig.getNumValue(lowPrefix, v);
        config.lowLimit = v;

        v = 0;
        currentConfig.getNumAttr(lowPrefix, "Min", v);
        config.minLowLimit = v;

        v = 0;
        currentConfig.getNumAttr(lowPrefix, "Max", v);
        config.maxLowLimit = v;

        v = 1;
        currentConfig.getNumValue(prefix + "Scale", v);
        config.scale = v;

        v = 0;
        currentConfig.getNumValue(prefix + "Step", v);
        config.step = v;

        iter = _configCache.insert(key, config);
    }

    return iter.value();
}

ParamRulerConfig AlarmConfig::getParamRulerConfig(SubParamID subParamId, UnitType unit)
{
    ParamRulerConfig config;

    // load data from config file
    QString prefix = "TrendGraph|Ruler|";
    prefix += paramInfo.getSubParamName(subParamId, true);
    prefix += "|";
    prefix += Unit::getSymbol(unit);
    prefix += "|";
    int v = 0;
    QString highPrefix = prefix + "High";
    systemConfig.getNumValue(highPrefix, v);
    config.upRuler = v;

    v = 0;
    systemConfig.getNumAttr(highPrefix, "Min", v);
    config.minUpRuler = v;

    v = 0;
    systemConfig.getNumAttr(highPrefix, "Max", v);
    config.maxUpRuler = v;

    v = 0;
    QString lowPrefix = prefix + "Low";
    systemConfig.getNumValue(lowPrefix, v);
    config.downRuler = v;

    v = 0;
    systemConfig.getNumAttr(lowPrefix, "Min", v);
    config.minDownRuler = v;

    v = 0;
    systemConfig.getNumAttr(lowPrefix, "Max", v);
    config.maxDownRuler = v;

    v = 1;
    systemConfig.getNumValue(prefix + "Scale", v);
    config.scale = v;

    return config;
}

void AlarmConfig::setParamRulerConfig(SubParamID subParamID, UnitType unit, int low, int high)
{
    // load data from config file
    QString prefix = "TrendGraph|Ruler|";
    prefix += paramInfo.getSubParamName(subParamID, true);
    prefix += "|";
    prefix += Unit::getSymbol(unit);
    prefix += "|";
    QString highPrefix = prefix + "High";
    systemConfig.setNumValue(highPrefix, high);

    QString lowPrefix = prefix + "Low";
    systemConfig.setNumValue(lowPrefix, low);
}

void AlarmConfig::setLimitAlarmConfig(SubParamID subParamId, UnitType unit, const LimitAlarmConfig &config)
{
    AlarmConfigKey key(subParamId, unit);
    LimitAlarmConfig curConfig = getLimitAlarmConfig(subParamId, unit);
    if (curConfig == config)
    {
        return;
    }

    QString prefix = "AlarmSource|";
    prefix += paramInfo.getSubParamName(subParamId, true);

    int val = 0;

    prefix += "|";
    prefix += Unit::getSymbol(unit);
    if (curConfig.highLimit != config.highLimit)
    {
        val = config.highLimit;
        currentConfig.setNumValue(prefix + "|High", val);
    }

    if (curConfig.lowLimit != config.lowLimit)
    {
        val = config.lowLimit;
        currentConfig.setNumValue(prefix + "|Low", val);
    }

    // TODO: update limit of other unit??

    _configCache.insert(key, config);
}

QString AlarmConfig::getLowLimitStr(const LimitAlarmConfig &config)
{
    if (config.scale == 1)
    {
        return QString::number(config.lowLimit);
    }
    else
    {
        return QString("%1.%2").number(config.lowLimit / config.scale , 'f' , 3)
               .number(config.lowLimit % config.scale , 'f' , 3);
    }
}

void AlarmConfig::clearLimitAlarmInfo()
{
    _configCache.clear();
    _controlCache.clear();
}

void AlarmConfig::onPatientTypeChange(PatientType type)
{
    Q_UNUSED(type)
    // need to reload config when patient type changed
    _configCache.clear();
    _controlCache.clear();
}

QString AlarmConfig::getHighLimitStr(const LimitAlarmConfig &config)
{
    if (config.scale == 1)
    {
        return QString::number(config.highLimit);
    }
    else
    {
        return QString("%1.%2").number(config.highLimit / config.scale , 'f' , 3)
               .number(config.highLimit % config.scale , 'f' , 3);
    }
}

AlarmConfig::AlarmConfig()
    : QObject()
{
    connect(&patientManager, SIGNAL(signalPatientType(PatientType)), this, SLOT(onPatientTypeChange(PatientType)));
}
