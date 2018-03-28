#pragma once
#include "MenuGroup.h"

class PublicMenuManager : public MenuGroup
{
public:
    static PublicMenuManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new PublicMenuManager();
        }
        return *_selfObj;
    }
    static PublicMenuManager *_selfObj;

    ~PublicMenuManager();

private:
    PublicMenuManager();
};
#define publicMenuManager (PublicMenuManager::construction())
#define deletePublicMenuManager() (delete PublicMenuManager::_selfObj)
