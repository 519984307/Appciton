/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/26
 **/

#include <QApplication>
#include "RescueDataSoftKeyAction.h"
#include "WindowManager.h"
#include "TrendTableWindow.h"
#include "RescueDataExportWidget.h"
#include "IMessageBox.h"
#include "DataStorageDirManager.h"
#include "SoftKeyManager.h"
#include "MenuGroup.h"
#include "MenuManager.h"
#include "EventWindow.h"
#include "OxyCRGEventWindow.h"
#include "TrendGraphWindow.h"
#include "HistoryDataReviewWindow.h"
#include "LayoutManager.h"

/***************************************************************************************************
 * 所有的快捷按键定义。
 **************************************************************************************************/
static KeyActionDesc _rescueDataKeys[] =
{
//    KeyActionDesc("", "Trend.png",   RescueDataSoftKeyAction::trendReview),
//    KeyActionDesc("", "Summary.png", RescueDataSoftKeyAction::summaryReview),
//    KeyActionDesc("", "Export.png",  RescueDataSoftKeyAction::exportData),
//    KeyActionDesc("", "Delete.png",  RescueDataSoftKeyAction::clearData),
//    KeyActionDesc("", "SoftkeyArrow.png", RescueDataSoftKeyAction::exit),

    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PAT_INFO),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PAT_NEW),
    KeyActionDesc("", "TrendTable", "Summary.png",   RescueDataSoftKeyAction::trendReview),
    KeyActionDesc("", "TrendGraph", "Summary.png", RescueDataSoftKeyAction::summaryReview),
    KeyActionDesc("", "ExportImport", "Export.png",  RescueDataSoftKeyAction::exportData),
    KeyActionDesc("", "Delete", "Delete.png",  RescueDataSoftKeyAction::clearData),
    KeyActionDesc("", "EventReview", "Summary.png", RescueDataSoftKeyAction::eventReview),
    KeyActionDesc("", "OxyCRGEventReview", "Summary.png", RescueDataSoftKeyAction::oxyCRGEventReview),
    KeyActionDesc("", "HistoryTrend", "Export.png",   RescueDataSoftKeyAction::historyReview),
    KeyActionDesc("", "", "SoftkeyArrow.png", RescueDataSoftKeyAction::exit),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_MAIN_SETUP)
};

/***************************************************************************************************
 * 趋势回顾回调。
 **************************************************************************************************/
void RescueDataSoftKeyAction::trendReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    TrendTableWindow::getInstance()->setHistoryData(false);
    windowManager.showWindow(TrendTableWindow::getInstance(),
                             WindowManager::ShowBehaviorCloseIfVisiable | WindowManager::ShowBehaviorCloseOthers);
}

/***************************************************************************************************
 * summary回顾回调。
 **************************************************************************************************/
void RescueDataSoftKeyAction::summaryReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    TrendGraphWindow::getInstance()->setHistoryData(false);
    windowManager.showWindow(TrendGraphWindow::getInstance(),
                             WindowManager::ShowBehaviorCloseIfVisiable | WindowManager::ShowBehaviorCloseOthers);
}

/***************************************************************************************************
 * 数据导出回调。
 **************************************************************************************************/
void RescueDataSoftKeyAction::exportData(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
}

/***************************************************************************************************
 * 进入营救数据回调。
 **************************************************************************************************/
void RescueDataSoftKeyAction::clearData(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    softkeyManager.setContent(SOFTKEY_ACTION_DEL_RESCUE_DATA);
}

/***************************************************************************************************
 * 退出营救数据回调。
 **************************************************************************************************/
void RescueDataSoftKeyAction::exit(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    SoftKeyActionType type = softkeyManager.uFaceTypeToSoftKeyType(layoutManager.getUFaceType());
    softkeyManager.setContent(type);
}

void RescueDataSoftKeyAction::eventReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    EventWindow::getInstance()->setHistoryData(false);
    windowManager.showWindow(EventWindow::getInstance(), WindowManager::ShowBehaviorCloseIfVisiable
                             | WindowManager::ShowBehaviorCloseOthers);
}

void RescueDataSoftKeyAction::oxyCRGEventReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    OxyCRGEventWindow::getInstance()->setHistoryData(false);
    windowManager.showWindow(OxyCRGEventWindow::getInstance(), WindowManager::ShowBehaviorCloseIfVisiable
                             | WindowManager::ShowBehaviorCloseOthers);
}

void RescueDataSoftKeyAction::historyReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    windowManager.showWindow(HistoryDataReviewWindow::getInstance(), WindowManager::ShowBehaviorCloseIfVisiable
                             | WindowManager::ShowBehaviorCloseOthers);
}

/***************************************************************************************************
 * 获取动作描述总个数。
 **************************************************************************************************/
int RescueDataSoftKeyAction::getActionDescNR(void)
{
    return sizeof(_rescueDataKeys) / sizeof(KeyActionDesc);
}

/***************************************************************************************************
 * 获取动作描述句柄。
 **************************************************************************************************/
KeyActionDesc *RescueDataSoftKeyAction::getActionDesc(int index)
{
    if (index >= static_cast<int>(sizeof(_rescueDataKeys) / sizeof(KeyActionDesc)))
    {
        return NULL;
    }

    if (_rescueDataKeys[index].type < SOFT_BASE_KEY_NR)
    {
        return SoftkeyActionBase::getActionDesc(_rescueDataKeys[index].type);
    }

    return &_rescueDataKeys[index];
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
RescueDataSoftKeyAction::RescueDataSoftKeyAction() : SoftkeyActionBase(SOFTKEY_ACTION_RESCUE_DATA)
{
}

/***************************************************************************************************
 * 析构。
 **************************************************************************************************/
RescueDataSoftKeyAction::~RescueDataSoftKeyAction()
{
}
