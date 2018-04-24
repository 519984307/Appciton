#include "ConfigMaintainMenuGrp.h"
#include "SelectDefaultConfigMenu.h"
#include "UserConfigEditMenu.h"
#include "ConfigExportImportMenu.h"
#include "MenuGroup.h"

ConfigMaintainMenuGrp::ConfigMaintainMenuGrp()
    :MenuGroup("ConfigMaintance"),
      _isSubmenuInitial(false)
{
}

ConfigMaintainMenuGrp &ConfigMaintainMenuGrp::getInstance()
{
    static ConfigMaintainMenuGrp *instance = NULL;
    if(instance == NULL)
    {
        instance = new ConfigMaintainMenuGrp();
    }

    return *instance;
}

void ConfigMaintainMenuGrp::initializeSubMenu()
{
    if(_isSubmenuInitial)
        return;

    addSubMenu(new SelectDefaultConfigMenu());
    addSubMenu(new UserConfigEditMenu());
    addSubMenu(new ConfigExportImportMenu());
    _isSubmenuInitial = true;
}
