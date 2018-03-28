#pragma once
#include "MenuGroup.h"

class ServiceMenuManager : public MenuGroup
{
public:
    static ServiceMenuManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ServiceMenuManager();
        }
        return *_selfObj;
    }
    static ServiceMenuManager *_selfObj;

    ~ServiceMenuManager();

private:
    ServiceMenuManager();
};
#define serviceMenuManager (ServiceMenuManager::construction())
#define deleteServiceMenuManager() (delete ServiceMenuManager::_selfObj)
