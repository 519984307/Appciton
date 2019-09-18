/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/30
 **/

#pragma once

#include <QObject>
#include <QScopedPointer>
#include <QList>
#include "ConfigManagerInterface.h"

class ConfigManagerPrivate;
class ConfigManager : public QObject, public ConfigManagerInterface
{
    Q_OBJECT
public:
    struct UserDefineConfigInfo {
      QString name;         // name of the this config
      QString fileName;     // xml filename
      QString patType;         // patient type
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
     * @param type patient type
     * @return save successfully or not
     */
    bool saveUserDefineConfig(const QString & configName, Config * configObj, const PatientType &type);

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

    /**
     * @brief isReadOnly 获取控件的预备失能
     * @return 1:预备失能
     */
    bool isReadOnly()const;

    /**
     * @brief setWidgetIfOnlyShown 设置控件状态 失能或使能
     * @param status 1, 0
     */
    void setWidgetIfOnlyShown(bool status);

    /**
     * @brief getWidgetStatus
     * @return
     */
    bool getWidgetsPreStatus() const;

    /**
     * @brief setWidgetStatus
     * @param status
     */
    void setWidgetStatus(bool status);

    /**
     * @brief getUserDefineConfigMaxLen 获得允许用户定义最大配置数量
     * @return
     */
    int getUserDefineConfigMaxLen();

    /**
     * @brief hasExistConfig 是否已经存在同名同类型配置表
     * @param name
     * @param type
     * @return
     */
    bool hasExistConfig(const QString &name, PatientType type);

    /**
     * @brief loadConfig 加载相应的配置（只能加载预设的配置）
     * @param type　病人类型
     */
    void loadConfig(PatientType type);

    /**
     * @brief getOriginalConfig 获取当前文件的原始文件（包括.original.xml和userDefine.xml）
     * @param type
     * @return
     */
    QString getOriginalConfig(PatientType type);

    /**
     * @brief setOriginalConfig 设置当前文件的原始文件（包括.original.xml和userDefine.xml）
     * @param type
     * @param path
     */
    void setOriginalConfig(PatientType type, QString path);
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

    /**
     * @brief configUpdated  发送配置更新信号
     */
    void configUpdated(void);

private slots:
    void _onPatientTypeChange(PatientType type);

private:
    ConfigManager();
    // 智能指针
    QScopedPointer<ConfigManagerPrivate> d_ptr;
};

#define configManager (ConfigManager::getInstance())
#define currentConfig (ConfigManager::getInstance().getCurConfig())
