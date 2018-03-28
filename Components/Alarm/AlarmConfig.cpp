#include "AlarmConfig.h"
#include "PatientManager.h"
#include "ParamInfo.h"
#include "IConfig.h"

AlarmConfig &AlarmConfig::getInstance()
{
    static AlarmConfig *instance = NULL;
    if(instance == NULL)
    {
        instance = new AlarmConfig();
    }
    return *instance;
}

bool AlarmConfig::isLimitAlarmEnable(SubParamID subParamId)
{
    LimitAlarmControlCache::iterator iter = _controlCache.find(subParamId);
    if(iter == _controlCache.end())
    {
        LimitAlarmControl ctrl;

        //load data from config file
        QString prefix = "AlarmSource|" + patientManager.getTypeStr() + "|";
        prefix += paramInfo.getSubParamName(subParamId, true);
        int v = 0;
        superRunConfig.getNumAttr(prefix, "Enable", v);
        ctrl.enable = v;
        v = 0;
        superRunConfig.getNumAttr(prefix, "Prio", v);
        ctrl.priority = (AlarmPriority)v;
        iter = _controlCache.insert(subParamId, ctrl);
    }
    return iter.value().enable;
}

void AlarmConfig::setLimitAlarmEnable(SubParamID subParamId, bool enable)
{
    if(isLimitAlarmEnable(subParamId) == enable)
    {
        return;
    }

    //update cache, the cache must be found here
    LimitAlarmControlCache::iterator iter = _controlCache.find(subParamId);
    iter->enable = enable;

    QString prefix = "AlarmSource|" + patientManager.getTypeStr() + "|";
    prefix += paramInfo.getSubParamName(subParamId, true);

    //save to config file
    int val = enable;
    superRunConfig.setNumAttr(prefix, "Enable", val);
}

AlarmPriority AlarmConfig::getLimitAlarmPriority(SubParamID subParamId)
{
    LimitAlarmControlCache::iterator iter = _controlCache.find(subParamId);
    if(iter == _controlCache.end())
    {
        LimitAlarmControl ctrl;

        //load data from config file
        QString prefix = "AlarmSource|" + patientManager.getTypeStr() + "|";
        prefix += paramInfo.getSubParamName(subParamId, true);
        int v = 0;
        superRunConfig.getNumAttr(prefix, "Enable", v);
        ctrl.enable = v;
        v = 0;
        superRunConfig.getNumAttr(prefix, "Prio", v);
        ctrl.priority = (AlarmPriority)v;
        iter = _controlCache.insert(subParamId, ctrl);
    }
    return iter.value().priority;
}

void AlarmConfig::setLimitAlarmPriority(SubParamID subParamId, AlarmPriority prio)
{
    if(getLimitAlarmPriority(subParamId) == prio)
    {
        return;
    }

    //update cache, the cache must be found here
    LimitAlarmControlCache::iterator iter = _controlCache.find(subParamId);
    iter->priority = prio;

    QString prefix = "AlarmSource|" + patientManager.getTypeStr() + "|";
    prefix += paramInfo.getSubParamName(subParamId, true);

    //save to config file
    int val = prio;
    superRunConfig.setNumAttr(prefix, "Prio", val);
}

LimitAlarmConfig AlarmConfig::getLimitAlarmConfig(SubParamID subParamId, UnitType unit)
{
    AlarmConfigKey key(subParamId, unit);
    LimitAlarmConfigCache::iterator iter = _configCache.find(key);
    if(iter == _configCache.end())
    {
        LimitAlarmConfig config;

        //load data from config file
        QString prefix = "AlarmSource|" + patientManager.getTypeStr() + "|";
        prefix += paramInfo.getSubParamName(subParamId, true);
        prefix += "|";
        prefix += Unit::getSymbol(unit);
        prefix += "|";
        int v = 0;
        QString highPrefix = prefix + "High";
        superRunConfig.getNumValue(highPrefix, v);
        config.highLimit = v;

        v = 0;
        superRunConfig.getNumAttr(highPrefix, "Min", v);
        config.minHighLimit = v;

        v = 0;
        superRunConfig.getNumAttr(highPrefix, "Max", v);
        config.maxHighLimit = v;

        v = 0;
        QString lowPrefix = prefix + "Low";
        superRunConfig.getNumValue(lowPrefix, v);
        config.lowLimit = v;

        v = 0;
        superRunConfig.getNumAttr(lowPrefix, "Min", v);
        config.minLowLimit = v;

        v = 0;
        superRunConfig.getNumAttr(lowPrefix, "Max", v);
        config.maxLowLimit = v;

        v = 1;
        superRunConfig.getNumValue(prefix + "Scale", v);
        config.scale = v;

        v = 0;
        superRunConfig.getNumValue(prefix + "Step", v);
        config.step = v;

        iter = _configCache.insert(key, config);
    }

    return iter.value();
}

void AlarmConfig::setLimitAlarmConfig(SubParamID subParamId, UnitType unit, const LimitAlarmConfig &config)
{
    AlarmConfigKey key(subParamId, unit);
    LimitAlarmConfig curConfig = getLimitAlarmConfig(subParamId, unit);
    if(curConfig == config)
    {
        return;
    }

    QString prefix = "AlarmSource|" + patientManager.getTypeStr() + "|";
    prefix += paramInfo.getSubParamName(subParamId, true);

    int val = 0;

    prefix += "|";
    prefix += Unit::getSymbol(unit);
    if(curConfig.highLimit != config.highLimit)
    {
        val = config.highLimit;
        superRunConfig.setNumValue(prefix + "|High", val);
    }

    if(curConfig.lowLimit != config.lowLimit)
    {
        val = config.lowLimit;
        superRunConfig.setNumValue(prefix + "|Low", val);
    }

    //TODO: update limit of other unit??

    _configCache.insert(key, config);

}

QString AlarmConfig::getLowLimitStr(const LimitAlarmConfig &config)
{
    if(config.scale == 1)
    {
        return QString::number(config.lowLimit);
    }
    else
    {
        return QString("%1.%2").arg(config.lowLimit / config.scale)
                .arg(config.lowLimit % config.scale);
    }
}

void AlarmConfig::onPatientTypeChange(PatientType type)
{
    Q_UNUSED(type)
    //need to reload config when patient type changed
    _configCache.clear();
    _controlCache.clear();
}

QString AlarmConfig::getHighLimitStr(const LimitAlarmConfig &config)
{
    if(config.scale == 1)
    {
        return QString::number(config.highLimit);
    }
    else
    {
        return QString("%1.%2").arg(config.highLimit / config.scale)
                .arg(config.highLimit % config.scale);
    }
}

AlarmConfig::AlarmConfig()
    :QObject()
{
    connect(&patientManager, SIGNAL(signalPatientType(PatientType)), this, SLOT(onPatientTypeChange(PatientType)));
}
