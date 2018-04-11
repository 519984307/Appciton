#include "SupervisorMenuManager.h"
#include "LanguageManager.h"
#include "WindowManager.h"

SupervisorMenuManager *SupervisorMenuManager::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SupervisorMenuManager::SupervisorMenuManager() : MenuGroup(trs("SupervisorModifyConfig"))
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SupervisorMenuManager::~SupervisorMenuManager()
{

}
