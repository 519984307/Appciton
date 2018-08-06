/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include "ConfigEditMenuGrp.h"
#include "ConfigEditAlarmLimitMenu.h"
#include "ConfigEditGeneralMenu.h"
#include "ConfigEditEcgMenu.h"
#include "ConfigEditRespMenu.h"
#include "ConfigEditNIBPMenu.h"
#include "ConfigEditSpO2Menu.h"
#include "ConfigEditCO2Menu.h"
#include "ConfigEditCOMenu.h"
#include "ConfigEditIBPMenu.h"
#include "IBPMenu.h"
#include "ECGMenu.h"
#include "SupervisorCodeMarker.h"
#include "ConfigEditDisplayMenu.h"

class ConfigEditMenuGrpPrivate
{
public:
    ConfigEditMenuGrpPrivate()
        : isSubmenuInitial(false),
          curEditConfig(NULL)
    {
    }

    bool isSubmenuInitial;
    Config *curEditConfig;
    QString configName;
    QMap <QString, SubMenu *> subMenuMap;
};

ConfigEditMenuGrp::ConfigEditMenuGrp()
    : MenuGroup("ConfigEdit"),
      d_ptr(new ConfigEditMenuGrpPrivate())
{
}

ConfigEditMenuGrp &ConfigEditMenuGrp::getInstance()
{
    static ConfigEditMenuGrp *instance = NULL;
    if (instance == NULL)
    {
        instance = new ConfigEditMenuGrp();
    }
    return *instance;
}

ConfigEditMenuGrp::~ConfigEditMenuGrp()
{
}

void ConfigEditMenuGrp::initializeSubMenu()
{
    if (d_ptr->isSubmenuInitial)
    {
        return;
    }

    d_ptr->subMenuMap.clear();

    SubMenu *subMenu = new ConfigEditGeneralMenu();
    addSubMenu(subMenu);
    d_ptr->subMenuMap["ConfigEditGeneralMenu"] = subMenu;

    subMenu = new ConfigEditAlarmLimitMenu();
    addSubMenu(subMenu);
    d_ptr->subMenuMap["ConfigEditAlarmLimitMenu"] = subMenu;

    subMenu = new ConfigEditEcgMenu();
    addSubMenu(subMenu);
    d_ptr->subMenuMap["ConfigEditEcgMenu"] = subMenu;

    subMenu = new ConfigEditRespMenu();
    addSubMenu(subMenu);
    d_ptr->subMenuMap["ConfigEditRespMenu"] = subMenu;

    subMenu = new ConfigEditSpO2Menu();
    addSubMenu(subMenu);
    d_ptr->subMenuMap["ConfigEditSpO2Menu"] = subMenu;

    subMenu = new ConfigEditNIBPMenu();
    addSubMenu(subMenu);
    d_ptr->subMenuMap["ConfigEditNIBPMenu"] = subMenu;

    subMenu = new ConfigCOMenu();
    addSubMenu(subMenu);
    d_ptr->subMenuMap["ConfigCOMenu"] = subMenu;

    subMenu = new ConfigCO2Menu();
    addSubMenu(subMenu);
    d_ptr->subMenuMap["ConfigCO2Menu"] = subMenu;

    subMenu = new ConfigIBPMenu();
    addSubMenu(subMenu);
    d_ptr->subMenuMap["ConfigIBPMenu"] = subMenu;

    subMenu = new SupervisorCodeMarker();
    addSubMenu(subMenu);
    d_ptr->subMenuMap["SupervisorCodeMarker"] = subMenu;

    subMenu = new ConfigEditDisplayMenu();
    addSubMenu(subMenu);
    d_ptr->subMenuMap["ConfigEditDisplayMenu"] = subMenu;

//    addSubMenu(new ConfigEditEcgMenu());
//    addSubMenu(new ConfigEditRespMenu());
//    addSubMenu(new ConfigEditSpO2Menu());
//    addSubMenu(new ConfigEditNIBPMenu());
//    addSubMenu(new ConfigCOMenu());
//    addSubMenu(new ConfigCO2Menu());
//    //addSubMenu(new ConfigEditUnitSetupMenu());
//    addSubMenu(new ConfigIBPMenu());
//    addSubMenu(new SupervisorCodeMarker());
//    addSubMenu(new ConfigEditDisplayMenu());
//    addSubMenu(new COMenu());
//    addSubMenu(new SPO2Menu());
//    addSubMenu(new ECGMenu());
//    addSubMenu(new AGMenu());
//    addSubMenu(new IBPMenu);
//    addSubMenu(new CO2Menu());

//    addSubMenu(new ConfigEditTEMPMenu());

    d_ptr->isSubmenuInitial = true;
}

void ConfigEditMenuGrp::setCurrentEditConfig(Config *config)
{
    d_ptr->curEditConfig = config;
}

Config *ConfigEditMenuGrp::getCurrentEditConfig() const
{
    return d_ptr->curEditConfig;
}

void ConfigEditMenuGrp::setCurrentEditConfigName(const QString &name)
{
    d_ptr->configName = name;
}

QString ConfigEditMenuGrp::getCurrentEditConfigName() const
{
    return d_ptr->configName;
}

QMap <QString, SubMenu *> ConfigEditMenuGrp::getCurrentEditConfigItem() const
{
    return d_ptr->subMenuMap;
}
