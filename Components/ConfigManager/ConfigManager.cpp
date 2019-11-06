/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/12
 **/


#include "ConfigManager.h"
#include "PatientManager.h"
#include "IConfig.h"
#include "Debug.h"
#include "TimeDate.h"
#include <QFile>
#include <QDateTime>
#include "LayoutManager.h"

#define CONFIG_DIR "/usr/local/nPM/etc/"
#define USER_DEFINE_CONFIG_PREFIX "UserDefine"
#define MAX_CONFIG_COUNT 3

class ConfigManagerPrivate
{
public:
    ConfigManagerPrivate()
        : curConfig(NULL),
          isDisableWidgets(false)
    {
        patientConfig[0] = NULL;
        patientConfig[1] = NULL;
        patientConfig[2] = NULL;
    }

    ~ConfigManagerPrivate();

    QString getDefaultConfigFilepath(PatientType patType);

    // 3 config object for adult, ped, neo
    Config *patientConfig[3];
    Config *curConfig;

    bool isDisableWidgets;  //是否失能控件标志位 1:失能
};

ConfigManagerPrivate::~ConfigManagerPrivate()
{
}
//获得默认的配置文件路径
QString ConfigManagerPrivate::getDefaultConfigFilepath(PatientType patType)
{
    QString configFileName;
    QString configFileNameTemp;
    QString index = QString("ConfigManager|Default|%1").arg(PatientSymbol::convert(patType));

    systemConfig.getStrValue(index, configFileName);
    if (configFileName.isEmpty())
    {
        qdebug("Fail to get default config file for patient type");
        return configFileName;
    }

    if (configFileName.indexOf(".Original"))
    {
        configFileNameTemp = configFileName;
        configFileNameTemp.replace(".Original", "");
    }

    QFile myFile(QString("%1/%2").arg(CONFIG_DIR).arg(configFileNameTemp));
    if (!myFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        myFile.copy(QString("%1/%2").arg(CONFIG_DIR).arg(configFileName),
                    QString("%1/%2").arg(CONFIG_DIR).arg(configFileNameTemp));
    }

    return QString("%1%2").arg(CONFIG_DIR).arg(configFileNameTemp);
}


ConfigManager::~ConfigManager()
{
}

ConfigManager &ConfigManager::getInstance()
{
    static ConfigManager *instance = NULL;

    if (instance == NULL)
    {
        instance = new ConfigManager();
        ConfigManagerInterface *old = registerConfigManager(instance);
        if (old)
        {
            delete old;
        }
    }

    return *instance;
}

Config *ConfigManager::getConfig(PatientType patType)
{
    if (patType > PATIENT_TYPE_NEO)
    {
        patType = PATIENT_TYPE_ADULT;
    }

    if (d_ptr->patientConfig[patType] == NULL)
    {
        // config object is not created yet, create it
        d_ptr->patientConfig[patType] = new Config(d_ptr->getDefaultConfigFilepath(patType));
    }

    return d_ptr->patientConfig[patType];
}

Config &ConfigManager::getCurConfig()
{
    d_ptr->curConfig = getConfig(patientManager.getType());
    return *d_ptr->curConfig;
}

QList<ConfigManager::UserDefineConfigInfo> ConfigManager::getUserDefineConfigInfos()
{
    int count = 0;
    QList<ConfigManager::UserDefineConfigInfo> infos;
    systemConfig.getNumAttr("ConfigManager|UserDefine", "count", count);
    for (int i = 0; i < count; i++)
    {
        QString index = QString("ConfigManager|UserDefine|Config%1").arg(i + 1);
        ConfigManager::UserDefineConfigInfo conf;
        systemConfig.getStrAttr(index, "name", conf.name);
        systemConfig.getStrValue(index, conf.fileName);
        systemConfig.getStrAttr(index, "patType", conf.patType);
        infos.append(conf);
    }
    return infos;
}

void ConfigManager::saveUserConfigInfo(const QList<ConfigManager::UserDefineConfigInfo> infos)
{
    // remove old nodes first
    int count = 0;
    systemConfig.getNumAttr("ConfigManager|UserDefine", "count", count);
    for (int i = 0; i < count; i++)
    {
        QString index = QString("ConfigManager|UserDefine|Config%1").arg(i + 1);
        systemConfig.removeNode(index);
    }

    // add new nodes
    count = infos.count();
    QMap<QString, QString> attrs;
    for (int i = 0; i < count; i++)
    {
        QString index = QString("ConfigManager|UserDefine");
        attrs["name"] = infos.at(i).name;
        attrs["patType"] = infos.at(i).patType;
        systemConfig.addNode(index,
                             QString("Config%1").arg(i + 1),
                             infos.at(i).fileName,
                             attrs);
    }
    systemConfig.setNumAttr("ConfigManager|UserDefine", "count", count);

    // update default config
    QString name;
    QString type;
    QString filename;
    for (int i = PATIENT_TYPE_ADULT; i <= PATIENT_TYPE_NEO; i++)
    {
        QString index = QString("ConfigManager|Default|%1").arg(PatientSymbol::convert((PatientType)i));
        systemConfig.getStrAttr(index, "type", type);
        if (type.toUpper() == "USER")
        {
            systemConfig.getStrAttr(index, "name", name);
            systemConfig.getStrValue(index, filename);

            bool found = false;
            for (int j = 0; j < infos.size(); j++)
            {
                if (infos.at(j).name == name && infos.at(j).fileName == filename)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                // set back to facroty default
                systemConfig.setStrAttr(index, "type", "Factory");
                systemConfig.setStrAttr(index, "name", "");
                systemConfig.setStrValue(index, factoryConfigFilename((PatientType)i));
            }
        }
    }
    emit userDefineConfigChanged();
    systemConfig.requestSave();
}

QString ConfigManager::factoryConfigFilename(PatientType patType)
{
    switch (patType)
    {
    case PATIENT_TYPE_PED:
        return "PedConfig.Original.xml";
    case PATIENT_TYPE_NEO:
        return "NeoConfig.Original.xml";
    default:
        return "AdultConfig.Original.xml";
    }
}

QString ConfigManager::runningConfigFilename(PatientType patType)
{
    switch (patType)
    {
    case PATIENT_TYPE_PED:
        return "PedConfig.xml";
    case PATIENT_TYPE_NEO:
        return "NeoConfig.xml";
    default:
        return "AdultConfig.xml";
    }
}

bool ConfigManager::getWidgetsPreStatus()const
{
    return false;
}

void ConfigManager::setWidgetStatus(bool status)
{
    Q_UNUSED(status)
}

int ConfigManager::getUserDefineConfigMaxLen()
{
    return MAX_CONFIG_COUNT;
}

bool ConfigManager::hasExistConfig(const QString &name, PatientType type)
{
    bool flag = false;
    QList<ConfigManager::UserDefineConfigInfo> infos = getUserDefineConfigInfos();
    for (int i = 0; i < infos.count(); i++)
    {
        if (name == infos.at(i).name && PatientSymbol::convert(type) == infos.at(i).patType)
        {
            flag = true;
            break;
        }
    }
    return flag;
}

void ConfigManager::loadConfig(PatientType type)
{
    // 更新配置
    QString filePath = d_ptr->getDefaultConfigFilepath(type);
    currentConfig.setCurrentFilePath(filePath);
    layoutManager.updateLayoutWidgetsConfig();
}

QString ConfigManager::getOriginalConfig(PatientType type)
{
    QString findPath = QString("%1|%2").arg("ConfigManager|Original|").arg(PatientSymbol::convert(type));
    QString path;
    systemConfig.getStrValue(findPath, path);
    return QString(CFG_PATH) + path;
}

void ConfigManager::setOriginalConfig(PatientType type, QString path)
{
    QString findPath = QString("%1|%2").arg("ConfigManager|Original|").arg(PatientSymbol::convert(type));
    systemConfig.setStrValue(findPath, path);
}

bool ConfigManager::isReadOnly()const
{
    return d_ptr->isDisableWidgets;
}

void ConfigManager::setWidgetIfOnlyShown(bool status)
{
    d_ptr->isDisableWidgets = status;
}

bool ConfigManager::saveUserDefineConfig(const QString &configName, Config *configObj, const PatientType &type)
{
    QList<ConfigManager::UserDefineConfigInfo> infos = getUserDefineConfigInfos();

    QChar name = 'A';
    if (type == PATIENT_TYPE_NEO)
    {
        name = 'N';
    }
    else if (type == PATIENT_TYPE_PED)
    {
        name = 'P';
    }
    QString filename = QString("%1-%2-%3.xml").arg(name).arg(USER_DEFINE_CONFIG_PREFIX)
                            .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));

    QString filePath = QString("%1%2").arg(CONFIG_DIR).arg(filename);
    if (!configObj->saveToFile(filePath))
    {
        return false;
    }
    ConfigManager::UserDefineConfigInfo info;
    info.fileName = filename;
    info.name = configName;
    info.patType = PatientSymbol::convert(type);        // 保存病人类型信息
    infos.append(info);
    saveUserConfigInfo(infos);
    return true;
}

void ConfigManager::_onPatientTypeChange(PatientType type)
{
    d_ptr->curConfig  = getConfig(type);
    emit configChanged();
}

ConfigManager::ConfigManager()
    : d_ptr(new ConfigManagerPrivate())
{
    // 在这里添加配置管理的初始化代码，该对象还要在其他处调用
}

