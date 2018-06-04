#pragma once
#include "MenuGroup.h"

class FactoryMaintainManager : public MenuGroup
{
public:
    static FactoryMaintainManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new FactoryMaintainManager();
        }
        return *_selfObj;
    }
    static FactoryMaintainManager *_selfObj;
    ~FactoryMaintainManager();
private:
    FactoryMaintainManager();
};
#define factoryMaintainManager (FactoryMaintainManager::construction())
#define deleteFactoryMaintainManager() (delete FactoryMaintainManager::_selfObj)
