#include <QApplication>
#include "RescueDataSoftKeyAction.h"
#include "WindowManager.h"
#include "TrendDataWidget.h"
#include "TrendGraphWidget.h"
#include "RescueDataExportWidget.h"
#include "IMessageBox.h"
#include "DataStorageDirManager.h"
#include "SoftKeyManager.h"
#include "MenuGroup.h"
#include "MenuManager.h"
#include "EventReviewWindow.h"
#include "OxyCRGEventWidget.h"
#include "HistoryDataReviewWidget.h"

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

    bool isVisible = trendDataWidget.isVisible();
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
    trendDataWidget.isHistoryData(false);
    trendDataWidget.autoShow(x, y);
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

//    bool isVisible = summaryRescueDataWidget.isVisible();
    bool isVisible = trendGraphWidget.isVisible();
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
    //summaryRescueDataWidget.autoShow(x, y);
    trendGraphWidget.isHistoryData(false);
    trendGraphWidget.autoShow(x, y);
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

    bool isVisible = rescueDataExportWidget.isVisible();
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
    rescueDataExportWidget.autoShow(x, y);
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

    bool isVisible = eventReviewWindow.isVisible();
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
    //summaryRescueDataWidget.autoShow(x, y);
    eventReviewWindow.isHistoryData(false);
    eventReviewWindow.autoShow(x, y);

}

void RescueDataSoftKeyAction::oxyCRGEventReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    bool isVisible = oxyCRGEventWidget.isVisible();
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
    //summaryRescueDataWidget.autoShow(x, y);
    oxyCRGEventWidget.isHistoryData(false);
    oxyCRGEventWidget.autoShow(x, y);
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
    if (index >= (int)(sizeof(_rescueDataKeys) / sizeof(KeyActionDesc)))
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
