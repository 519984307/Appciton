#include <QApplication>
#include "DelRescueDataSoftkeyAction.h"
#include "WindowManager.h"
#include "TrendDataWidget.h"
#include "SummaryRescueDataWidget.h"
#include "RescueDataExportWidget.h"
#include "IMessageBox.h"
#include "DataStorageDirManager.h"
#include "SoftKeyManager.h"
#include "MenuGroup.h"
#include "RescueDataDeleteWidget.h"
#include "MenuManager.h"

IMessageBox *DelRescueDataSoftkeyAction::_messageBox = NULL;
/***************************************************************************************************
 * 所有的快捷按键定义。
 **************************************************************************************************/
static KeyActionDesc _delRescueDataKeys[] =
{
//    KeyActionDesc("", "delAll.png", DelRescueDataSoftkeyAction::delAll),
//    KeyActionDesc("", "delCase.png", DelRescueDataSoftkeyAction::delCase),
//    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, Qt::black, Qt::black, Qt::black, false),
//    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, Qt::black, Qt::black, Qt::black, false),
//    KeyActionDesc("", "SoftkeyArrow.png", DelRescueDataSoftkeyAction::exit),

    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PAT_INFO),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PAT_NEW),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PREVIOUS_PAGE),
    KeyActionDesc("", "delAll.png", DelRescueDataSoftkeyAction::delAll),
    KeyActionDesc("", "delCase.png", DelRescueDataSoftkeyAction::delCase),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "SoftkeyArrow.png", DelRescueDataSoftkeyAction::exit),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_MAIN_SETUP),
};

/***************************************************************************************************
 * 删除所有数据。
 **************************************************************************************************/
void DelRescueDataSoftkeyAction::delAll(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    if (NULL == _messageBox)
    {
        _messageBox = new IMessageBox(trs("Prompt"), trs("ClearAllRescueData"));
    }

    bool isVisible = _messageBox->isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
    }

    if (isVisible)
    {
        return;
    }

    if (1 == _messageBox->exec())
    {
        dataStorageDirManager.deleteAllData();
    }
}

/***************************************************************************************************
 * 选择删除。
 **************************************************************************************************/
void DelRescueDataSoftkeyAction::delCase(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    bool isVisible = rescueDataDeleteWidget.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
    }

    if (isVisible)
    {
        return;
    }

    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - menuManager.width()) / 2;
    int y = r.y() + (r.height() - menuManager.height());
    rescueDataDeleteWidget.autoShow(x, y);
}

/***************************************************************************************************
 * 退出营救数据回调。
 **************************************************************************************************/
void DelRescueDataSoftkeyAction::exit(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    softkeyManager.setContent(SOFTKEY_ACTION_RESCUE_DATA);
}

/***************************************************************************************************
 * 获取动作描述总个数。
 **************************************************************************************************/
int DelRescueDataSoftkeyAction::getActionDescNR(void)
{
    return sizeof(_delRescueDataKeys) / sizeof(KeyActionDesc);
}

/***************************************************************************************************
 * 获取动作描述句柄。
 **************************************************************************************************/
KeyActionDesc *DelRescueDataSoftkeyAction::getActionDesc(int index)
{
    if (index >= (int)(sizeof(_delRescueDataKeys) / sizeof(KeyActionDesc)))
    {
        return NULL;
    }

    if (_delRescueDataKeys[index].type < SOFT_BASE_KEY_NR)
    {
        return SoftkeyActionBase::getActionDesc(_delRescueDataKeys[index].type);
    }

    return &_delRescueDataKeys[index];
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
DelRescueDataSoftkeyAction::DelRescueDataSoftkeyAction() :
    SoftkeyActionBase(SOFTKEY_ACTION_DEL_RESCUE_DATA)
{

}

/***************************************************************************************************
 * 析构。
 **************************************************************************************************/
DelRescueDataSoftkeyAction::~DelRescueDataSoftkeyAction()
{
    if (NULL != _messageBox)
    {
        delete _messageBox;
        _messageBox = NULL;
    }
}


