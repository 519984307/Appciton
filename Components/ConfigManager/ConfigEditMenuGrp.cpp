#include "ConfigEditMenuGrp.h"
#include "ConfigEditAlarmLimitMenu.h"
#include "ConfigEditGeneralMenu.h"
#include "ConfigEditEcgMenu.h"
#include "ConfigEditRespMenu.h"
#include "ConfigEditNIBPMenu.h"
#include "ConfigEditSpO2Menu.h"
#include "ConfigEditCO2Menu.h"
#include "ConfigEditCOMenu.h"
#include "ConfigEditPAMenu.h"

class ConfigEditMenuGrpPrivate
{
public:
    ConfigEditMenuGrpPrivate()
        :isSubmenuInitial(false),
          curEditConfig(NULL)
    {

    }

    bool isSubmenuInitial;
    Config *curEditConfig;
    QString configName;
};

ConfigEditMenuGrp::ConfigEditMenuGrp()
    :MenuGroup("ConfigEdit"),
      d_ptr(new ConfigEditMenuGrpPrivate())
{

}

ConfigEditMenuGrp &ConfigEditMenuGrp::getInstance()
{
    static ConfigEditMenuGrp *instance = NULL;
    if(instance == NULL)
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
    if(d_ptr->isSubmenuInitial)
        return;

    addSubMenu(new ConfigEditGeneralMenu());
    addSubMenu(new ConfigEditAlarmLimitMenu());
    addSubMenu(new ConfigEditEcgMenu());
    addSubMenu(new ConfigEditRespMenu());
    addSubMenu(new ConfigEditNIBPMenu());
    addSubMenu(new ConfigEditSpO2Menu());
    addSubMenu(new ConfigEditCO2Menu());
    addSubMenu(new ConfigEditCOMenu());
    addSubMenu(new ConfigEditPAMenu());
    //addSubMenu(new ConfigEditTEMPMenu());
    addReturnMenu();

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
