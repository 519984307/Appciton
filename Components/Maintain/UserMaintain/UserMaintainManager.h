#pragma once
#include "MenuGroup.h"

class UserMaintainManager : public MenuGroup
{
public:
    static UserMaintainManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new UserMaintainManager();
        }
        return *_selfObj;
    }
    static UserMaintainManager *_selfObj;

    ~UserMaintainManager();

private:
    UserMaintainManager();
};
#define userMaintainManager (UserMaintainManager::construction())
#define deleteUserMaintainManager() (delete UserMaintainManager::_selfObj)
