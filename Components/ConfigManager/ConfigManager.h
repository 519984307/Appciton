#pragma once

#include <QObject>
#include <QScopedPointer>
#include "Config.h"
#include "PatientDefine.h"
#include <QList>

class ConfigManagerPrivate;
class ConfigManager : public QObject
{
    Q_OBJECT
public:

    struct UserDefineConfigInfo {
      QString name;         //name of the this config
      QString fileName;     //xml filename
    };

    ~ConfigManager();

    /**
     * @brief getInstance signleton
     * @return
     */
    static ConfigManager &getInstance();

    /**
     * @brief getConfig get a config object of the specific patient type
     * @param patType the paient type, adult, ped or neo
     * @return pointer to the specific patient type config object
     */
    Config *getConfig(PatientType patType);

    /**
     * @brief getCurConfig get the reference of the current running config
     * @return  reference to the current running config
     */
    Config &getCurConfig();

    /**
     * @brief getUserConfigInfos get user define config info
     * @return a list of the user define config info
     */
    QList<UserDefineConfigInfo> getUserDefineConfigInfos();

    /**
     * @brief saveUserDefineConfig the user define config
     * @param configName config name
     * @param configObj pointer to the config object
     * @return save successfully or not
     */
    bool saveUserDefineConfig(const QString & configName, Config * configObj);

    /**
     * @brief saveUserConfigInfo save the user config info to the system config
     * @param infos
     * @note the old user config info will be remove
     */
    void saveUserConfigInfo(const QList<ConfigManager::UserDefineConfigInfo> infos);

    /**
     * @brief factoryConfigFilename get the factory config file name for specific patient type
     * @param patType patient type
     * @return config file name
     */
    QString factoryConfigFilename(PatientType patType);

    /**
     * @brief runningConfigFilename get the running config file name for specific patient type
     * @param patType patient type
     * @return  config file name
     */
    QString runningConfigFilename(PatientType patType);

signals:
    /**
     * @brief configChange
     * @note emit when the current running confg object switch to other config
     *       object. Usually happens after select patient type
     *
     */
    void configChanged();
    
    /**
     * @brief userDefineConfigChanged
     * @note 
     */
    void userDefineConfigChanged();

private slots:
    void _onPatientTypeChange(PatientType type);

private:
    ConfigManager();
    //智能指针
    QScopedPointer<ConfigManagerPrivate> d_ptr;
};

#define configManager (ConfigManager::getInstance())
