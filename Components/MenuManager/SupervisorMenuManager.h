#pragma once
#include "MenuGroup.h"

class SupervisorMenuManager : public MenuGroup
{
public:
    static SupervisorMenuManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SupervisorMenuManager();
        }
        return *_selfObj;
    }
    static SupervisorMenuManager *_selfObj;

    ~SupervisorMenuManager();

private:
    SupervisorMenuManager();
};
#define supervisorMenuManager (SupervisorMenuManager::construction())
#define deleteSupervisorMenuManager() (delete SupervisorMenuManager::_selfObj)
