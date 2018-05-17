#include "UserMaintainManager.h"
#include "LanguageManager.h"
#include "WindowManager.h"

UserMaintainManager *UserMaintainManager::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
UserMaintainManager::UserMaintainManager() : MenuGroup(trs("UserMaintainModifyConfig"))
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
UserMaintainManager::~UserMaintainManager()
{

}
