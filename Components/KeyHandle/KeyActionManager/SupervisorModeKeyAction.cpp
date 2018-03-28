#include "SupervisorModeKeyAction.h"
#include "ComboListPopup.h"
#include "PrintManager.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SupervisorModeKeyAction::SupervisorModeKeyAction()
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SupervisorModeKeyAction::~SupervisorModeKeyAction()
{

}

/**************************************************************************************************
 * F7释放
 *************************************************************************************************/
void SupervisorModeKeyAction::keyF7Released(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    //supervisor中关闭下拉弹出框
    if (ComboListPopup::current())
    {
        ComboListPopup::current()->close();
    }
}

void SupervisorModeKeyAction::keyF8Pressed(bool multiBtnPress)
{
    if (multiBtnPress)
    {
        return;
    }

    if (printManager.isPrinting())
    {
        printManager.abort();
        return;
    }
}

