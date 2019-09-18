/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/16
 **/
#include "ConfigEditMenuWindow.h"
#include "ConfigEditCO2MenuContent.h"
#include "ConfigEditCOMenuContent.h"
#include "ConfigEditEcgMenuContent.h"
#include "ConfigEditGeneralMenuContent.h"
#include "ConfigEditNIBPMenuContent.h"
#include "ConfigEditRespMenuContent.h"
#include "ConfigEditIBPMenuContent.h"
#include "ConfigEditDisplayMenuContent.h"
#include "ConfigEditSpO2MenuContent.h"
#include "ConfigEditCodeMarkerMenuContent.h"
#include "ConfigEditAlarmLimitMenuContent.h"
#include "UserConfigEditMenuContent.h"
#include "ConfigEditTEMPMenuContent.h"
#include "SystemManager.h"

class ConfigEditMenuWindowPrivate
{
public:
    ConfigEditMenuWindowPrivate()
        : isSubmenuInitial(false)
        , curEditConfig(NULL)
        , configName("")
        , patType(PATIENT_TYPE_ADULT)
    {
    }

    bool isSubmenuInitial;
    Config *curEditConfig;
    QString configName;
    PatientType patType;
    QMap <QString, MenuContent *> subMenuMap;
};

ConfigEditMenuWindow::ConfigEditMenuWindow()
    : MenuWindow(),
      d_ptr(new ConfigEditMenuWindowPrivate())
{
}

ConfigEditMenuWindow::~ConfigEditMenuWindow()
{
}

void ConfigEditMenuWindow::initializeSubMenu()
{
    if (d_ptr->isSubmenuInitial)
    {
        return;
    }

    d_ptr->subMenuMap.clear();

    MenuContent *subMenu;

    Config *config = getCurrentEditConfig();
    subMenu = new ConfigEditGeneralMenuContent(this);
    addMenuContent(subMenu);
    d_ptr->subMenuMap["ConfigEditGeneralMenu"] = subMenu;

    subMenu = new ConfigEditAlarmLimitMenuContent(config);
    addMenuContent(subMenu);
    d_ptr->subMenuMap["ConfigEditAlarmLimitMenu"] = subMenu;

    subMenu = new ConfigEditECGMenuContent(config);
    addMenuContent(subMenu);
    d_ptr->subMenuMap["ConfigEditEcgMenu"] = subMenu;

    if (systemManager.isSupport(CONFIG_RESP))
    {
        subMenu = new ConfigEditRespMenuContent(config);
        addMenuContent(subMenu);
        d_ptr->subMenuMap["ConfigEditRespMenu"] = subMenu;
    }

    if (systemManager.isSupport(CONFIG_TEMP))
    {
        subMenu = new ConfigEditTEMPMenuContent(config);
        addMenuContent(subMenu);
        d_ptr->subMenuMap["ConfigEditTEMPMenu"] = subMenu;
    }

    if (systemManager.isSupport(CONFIG_SPO2))
    {
        subMenu = new ConfigEditSpO2MenuContent(config);
        addMenuContent(subMenu);
        d_ptr->subMenuMap["ConfigEditSpO2Menu"] = subMenu;
    }

    if (systemManager.isSupport(CONFIG_NIBP))
    {
        subMenu = new ConfigEditNIBPMenuContent(config, getCurrentConfigType());
        addMenuContent(subMenu);
        d_ptr->subMenuMap["ConfigEditNIBPMenu"] = subMenu;
    }

    if (systemManager.isSupport(CONFIG_CO))
    {
        subMenu = new ConfigEditCOMenuContent(config);
        addMenuContent(subMenu);
        d_ptr->subMenuMap["ConfigEditCOMenu"] = subMenu;
    }

    if (systemManager.isSupport(CONFIG_CO2))
    {
        subMenu = new ConfigEditCO2MenuContent(config);
        addMenuContent(subMenu);
        d_ptr->subMenuMap["ConfigEditCO2Menu"] = subMenu;
    }

    if (systemManager.isSupport(CONFIG_IBP))
    {
        subMenu = new ConfigEditIBPMenuContent(config);
        addMenuContent(subMenu);
        d_ptr->subMenuMap["ConfigEditIBPMenu"] = subMenu;
    }

    subMenu = new ConfigEditCodeMarkerMenuContent(config);
    addMenuContent(subMenu);
    d_ptr->subMenuMap["ConfigEditCodeMarkerMenu"] = subMenu;

    QStringList colorList;
    QString color;
    config->getStrValue("Display|AllColors", color);
    colorList = color.split(',', QString::KeepEmptyParts);

    subMenu = new ConfigEditDisplayMenuContent(colorList, config);
    addMenuContent(subMenu);

    d_ptr->subMenuMap["ConfigEditDisplayMenu"] = subMenu;


    d_ptr->isSubmenuInitial = true;
}

void ConfigEditMenuWindow::setCurrentEditConfigInfo(Config *config, PatientType type)
{
    d_ptr->curEditConfig = config;
    d_ptr->patType = type;
}

Config *ConfigEditMenuWindow::getCurrentEditConfig() const
{
    return d_ptr->curEditConfig;
}

PatientType ConfigEditMenuWindow::getCurrentConfigType() const
{
    return d_ptr->patType;
}

void ConfigEditMenuWindow::setCurrentEditConfigName(const QString &name)
{
    d_ptr->configName = name;
}

QString ConfigEditMenuWindow::getCurrentEditConfigName() const
{
    return d_ptr->configName;
}

QMap <QString, MenuContent *> ConfigEditMenuWindow::getCurrentEditConfigItem() const
{
    return d_ptr->subMenuMap;
}

