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
#include "ConfigEditAlarmLimitMenu.h"
#include "ConfigEditGeneralMenu.h"
#include "ConfigEditEcgMenu.h"
#include "ConfigEditRespMenu.h"
#include "ConfigEditNIBPMenu.h"
#include "ConfigEditSpO2Menu.h"
#include "ConfigEditCO2Menu.h"
#include "ConfigEditCOMenu.h"
#include "ConfigEditIBPMenu.h"
#include "SupervisorCodeMarker.h"
#include "ConfigEditDisplayMenu.h"
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
#include "ConfigEditTEMPMenu.h"

class ConfigEditMenuWindowPrivate
{
public:
    ConfigEditMenuWindowPrivate()
        : isSubmenuInitial(false),
          curEditConfig(NULL)
    {
        configName.clear();
        subMenuMap.clear();
    }

    bool isSubmenuInitial;
    Config *curEditConfig;
    QString configName;
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

    subMenu = new ConfigEditRespMenuContent(config);
    addMenuContent(subMenu);
    d_ptr->subMenuMap["ConfigEditRespMenu"] = subMenu;

    subMenu = new ConfigEditTEMPMenu(config);
    addMenuContent(subMenu);
    d_ptr->subMenuMap["ConfigEditTEMPMenu"] = subMenu;

    subMenu = new ConfigEditSpO2MenuContent(config);
    addMenuContent(subMenu);
    d_ptr->subMenuMap["ConfigEditSpO2Menu"] = subMenu;

    subMenu = new ConfigEditNIBPMenuContent(config);
    addMenuContent(subMenu);
    d_ptr->subMenuMap["ConfigEditNIBPMenu"] = subMenu;

    subMenu = new ConfigEditCOMenuContent(config);
    addMenuContent(subMenu);
    d_ptr->subMenuMap["ConfigEditCOMenu"] = subMenu;

    subMenu = new ConfigEditCO2MenuContent(config);
    addMenuContent(subMenu);
    d_ptr->subMenuMap["ConfigEditCO2Menu"] = subMenu;

    subMenu = new ConfigEditIBPMenuContent(config);
    addMenuContent(subMenu);
    d_ptr->subMenuMap["ConfigEditIBPMenu"] = subMenu;

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

void ConfigEditMenuWindow::setCurrentEditConfig(Config *config)
{
    d_ptr->curEditConfig = config;
}

Config *ConfigEditMenuWindow::getCurrentEditConfig() const
{
    return d_ptr->curEditConfig;
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

