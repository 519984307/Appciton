/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/9
 **/



#include <QApplication>
#include "DelRescueDataSoftkeyAction.h"
#include "WindowManager.h"
#include "RescueDataExportWidget.h"
#include "MessageBox.h"
#include "DataStorageDirManager.h"
#include "SoftKeyManager.h"
#include "MenuGroup.h"
#include "MenuManager.h"
#include "RescueDataDeleteWindow.h"
#include "LanguageManager.h"

MessageBox *DelRescueDataSoftkeyAction::_messageBox = NULL;
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

    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PAT_INFO),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PAT_NEW),
    KeyActionDesc("", "DeleteAll", "delAll.png", DelRescueDataSoftkeyAction::delAll),
    KeyActionDesc("", "DeleteCase", "delCase.png", DelRescueDataSoftkeyAction::delCase),
    KeyActionDesc("", "", "SoftkeyArrow.png", DelRescueDataSoftkeyAction::exit),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_MAIN_SETUP)
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
        _messageBox = new MessageBox(trs("Prompt"), trs("ClearAllRescueData"));
    }

    bool isVisible = _messageBox->isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
        menuManager.close();
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

    bool isVisible = rescueDataDeleteWindow.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
        menuManager.close();
    }

    if (isVisible)
    {
        return;
    }
    windowManager.showWindow(&rescueDataDeleteWindow , WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorCloseOthers);
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
    if (index >= static_cast<int>(sizeof(_delRescueDataKeys) / sizeof(KeyActionDesc)))
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


