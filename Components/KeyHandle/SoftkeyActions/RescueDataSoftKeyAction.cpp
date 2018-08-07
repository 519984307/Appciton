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
#include "TrendDataWidget.h"
#include "TrendTableWindow.h"
#include "TrendGraphWidget.h"
#include "RescueDataExportWidget.h"
#include "IMessageBox.h"
#include "DataStorageDirManager.h"
#include "SoftKeyManager.h"
#include "MenuGroup.h"
#include "MenuManager.h"
#include "EventReviewWindow.h"
#include "EventWindow.h"
#include "OxyCRGEventWidget.h"
#include "OxyCRGEventWindow.h"
#include "HistoryDataReviewWidget.h"
#include "TrendGraphWindow.h"

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

    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PAT_INFO),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PAT_NEW),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PREVIOUS_PAGE),
    KeyActionDesc("", "Trend.png",   RescueDataSoftKeyAction::trendReview),
    KeyActionDesc("", "Summary.png", RescueDataSoftKeyAction::summaryReview),
    KeyActionDesc("", "Export.png",  RescueDataSoftKeyAction::exportData),
    KeyActionDesc("", "Delete.png",  RescueDataSoftKeyAction::clearData),
    KeyActionDesc("", "Trend.png", RescueDataSoftKeyAction::eventReview),
    KeyActionDesc("", "Trend.png", RescueDataSoftKeyAction::oxyCRGEventReview),
    KeyActionDesc("", "Export.png",   RescueDataSoftKeyAction::historyReview),
    KeyActionDesc("", "SoftkeyArrow.png", RescueDataSoftKeyAction::exit),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_MAIN_SETUP),
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

//    bool isVisible = trendDataWidget.isVisible();
//    while (NULL != QApplication::activeModalWidget())
//    {
//        QApplication::activeModalWidget()->hide();
//        menuManager.close();
//    }

//    if (isVisible)
//    {
//        return;
//    }

//    QRect r = windowManager.getMenuArea();
//    int x = r.x() + (r.width() - menuManager.getSubmenuWidth()) / 2;
//    int y = r.y() + (r.height() - menuManager.getSubmenuHeight());
//    trendDataWidget.setHistoryData(false);
//    trendDataWidget.autoShow(x, y);
    windowManager.showWindow(&trendTableWindow);
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

//    bool isVisible = trendGraphWidget.isVisible();
//    while (NULL != QApplication::activeModalWidget())
//    {
//        QApplication::activeModalWidget()->hide();
//        menuManager.close();
//    }

//    if (isVisible)
//    {
//        return;
//    }

//    QRect r = windowManager.getMenuArea();
//    int x = r.x() + (r.width() - menuManager.getSubmenuWidth()) / 2;
//    int y = r.y() + (r.height() - menuManager.getSubmenuHeight());
//    trendGraphWidget.setHistoryData(false);
//    trendGraphWidget.autoShow(x, y);
    windowManager.showWindow(&trendGraphWindow);
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

//    bool isVisible = rescueDataExportWidget.isVisible();
//    while (NULL != QApplication::activeModalWidget())
//    {
//        QApplication::activeModalWidget()->hide();
//        menuManager.close();
//    }

//    if (isVisible)
//    {
//        return;
//    }

//    QRect r = windowManager.getMenuArea();
//    int x = r.x() + (r.width() - menuManager.getSubmenuWidth()) / 2;
//    int y = r.y() + (r.height() - menuManager.getSubmenuHeight());
//    rescueDataExportWidget.autoShow(x, y);
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

    SoftKeyActionType type = softkeyManager.uFaceTypeToSoftKeyType(windowManager.getUFaceType());
    softkeyManager.setContent(type, true);
}

void RescueDataSoftKeyAction::eventReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

//    bool isVisible = eventReviewWindow.isVisible();
//    while (NULL != QApplication::activeModalWidget())
//    {
//        QApplication::activeModalWidget()->hide();
//        menuManager.close();
//    }

//    if (isVisible)
//    {
//        return;
//    }

//    QRect r = windowManager.getMenuArea();
//    int x = r.x() + (r.width() - menuManager.getSubmenuWidth()) / 2;
//    int y = r.y() + (r.height() - menuManager.getSubmenuHeight());
//    // summaryRescueDataWidget.autoShow(x, y);
//    eventReviewWindow.setHistoryData(false);
//    eventReviewWindow.autoShow(x, y);
    windowManager.showWindow(&eventWindow);
}

void RescueDataSoftKeyAction::oxyCRGEventReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

//    bool isVisible = oxyCRGEventWidget.isVisible();
//    while (NULL != QApplication::activeModalWidget())
//    {
//        QApplication::activeModalWidget()->hide();
//        menuManager.close();
//    }

//    if (isVisible)
//    {
//        return;
//    }

//    QRect r = windowManager.getMenuArea();
//    int x = r.x() + (r.width() - menuManager.getSubmenuWidth()) / 2;
//    int y = r.y() + (r.height() - menuManager.getSubmenuHeight());
//    // summaryRescueDataWidget.autoShow(x, y);
//    oxyCRGEventWidget.setHistoryData(false);
//    oxyCRGEventWidget.autoShow(x, y);
    windowManager.showWindow(&oxyCRGEventWindow);
}

void RescueDataSoftKeyAction::historyReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    bool isVisible = historyDataReviewWidget.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
        menuManager.close();
    }

    if (isVisible)
    {
        return;
    }

    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - menuManager.getSubmenuWidth()) / 2;
    int y = r.y() + (r.height() - menuManager.getSubmenuHeight());
    historyDataReviewWidget.autoShow(x, y);
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
