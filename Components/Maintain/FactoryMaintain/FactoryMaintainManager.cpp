#include "FactoryMaintainManager.h"
#include "LanguageManager.h"
#include "WindowManager.h"

FactoryMaintainManager *FactoryMaintainManager::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
FactoryMaintainManager::FactoryMaintainManager() : MenuGroup(trs("FactoryMaintainModifyConfig"))
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
FactoryMaintainManager::~FactoryMaintainManager()
{

}
