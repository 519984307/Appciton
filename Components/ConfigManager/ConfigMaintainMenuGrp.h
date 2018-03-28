#pragma once
#include "MenuGroup.h"

class ConfigMaintainMenuGrp : public MenuGroup
{
    Q_OBJECT
public:
    /**
     * @brief getInstance singleton
     * @return
     */
    static ConfigMaintainMenuGrp &getInstance();

    /**
     * @brief addAllSubMenus intialize the sub menus
     */
    void initializeSubMenu();

private:
    ConfigMaintainMenuGrp();
    bool _isSubmenuInitial;
};
#define configMaintainMenuGrp (ConfigMaintainMenuGrp::getInstance())
