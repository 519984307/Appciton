#include <QApplication>
#include "CalculateSoftkeyAction.h"
#include "WindowManager.h"
#include "SoftKeyManager.h"
#include "MenuGroup.h"
#include "MenuManager.h"
#include "DataStorageDirManager.h"
#include "DoseCalculationWidget.h"
#include "DoseCalculationManager.h"
#include "HemodynamicWidget.h"
#include "ECG12LDataAcquire.h"
#include "ECG12LeadManager.h"

/***************************************************************************************************
 * 所有的快捷按键定义。
 **************************************************************************************************/
static KeyActionDesc _calculationDataKeys[] =
{
//    KeyActionDesc("", "Trend.png",   CalculateSoftkeyAction::trendReview),
//    KeyActionDesc("", "Summary.png", CalculateSoftkeyAction::summaryReview),
//    KeyActionDesc("", "Export.png",  CalculateSoftkeyAction::exportData),
//    KeyActionDesc("", "Delete.png",  CalculateSoftkeyAction::clearData),
//    KeyActionDesc("", "SoftkeyArrow.png", CalculateSoftkeyAction::exit),

    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PAT_INFO),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PAT_NEW),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PREVIOUS_PAGE),
    KeyActionDesc("", "dosecalculation.png",CalculateSoftkeyAction::doseCalculation),
    KeyActionDesc("", "blood.png",CalculateSoftkeyAction::Hemodynamic),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "SoftkeyArrow.png", CalculateSoftkeyAction::exit),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_MAIN_SETUP),
};

/***************************************************************************************************
 * 药物计算回调。
 **************************************************************************************************/
void CalculateSoftkeyAction::doseCalculation(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    if (ecg12LDataAcquire.isAcquiring() || ecg12LeadManager.isTransfer())
    {
        return;
    }

    bool isVisible = doseCalculationWidget.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
        menuManager.close();
    }

    if (isVisible)
    {
        return;
    }

//    QRect r = windowManager.getMenuArea();
//    int x = r.x() + (r.width() - patientMenu.width()) / 2;
//    int y = r.y() + (r.height() - patientMenu.height());
//    patientMenu.autoShow(x, y);
//    //    patientMenu.autoShow();
    doseCalculationWidget.autoShow();
}

/***************************************************************************************************
 * 血液动力学回调。
 **************************************************************************************************/
void CalculateSoftkeyAction::Hemodynamic(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    if (ecg12LDataAcquire.isAcquiring() || ecg12LeadManager.isTransfer())
    {
        return;
    }

    bool isVisible = hemodynamicWidget.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
        menuManager.close();
    }

    if (isVisible)
    {
        return;
    }

//    QRect r = windowManager.getMenuArea();
//    int x = r.x() + (r.width() - patientMenu.width()) / 2;
//    int y = r.y() + (r.height() - patientMenu.height());
//    patientMenu.autoShow(x, y);
//    //    patientMenu.autoShow();
    hemodynamicWidget.autoShow();
}

void CalculateSoftkeyAction::exit(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    SoftKeyActionType type = softkeyManager.uFaceTypeToSoftKeyType(windowManager.getUFaceType());
    softkeyManager.setContent(type, true);
}

int CalculateSoftkeyAction::getActionDescNR(void)
{
    return sizeof(_calculationDataKeys) / sizeof(KeyActionDesc);
}

/***************************************************************************************************
 * 获取动作描述句柄。
 **************************************************************************************************/
KeyActionDesc *CalculateSoftkeyAction::getActionDesc(int index)
{
    if (index >= (int)(sizeof(_calculationDataKeys) / sizeof(KeyActionDesc)))
    {
        return NULL;
    }

    if (_calculationDataKeys[index].type < SOFT_BASE_KEY_NR)
    {
        return SoftkeyActionBase::getActionDesc(_calculationDataKeys[index].type);
    }

    return &_calculationDataKeys[index];
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
CalculateSoftkeyAction::CalculateSoftkeyAction() : SoftkeyActionBase(SOFTKEY_ACTION_CALCULATE)
{

}

/***************************************************************************************************
 * 析构。
 **************************************************************************************************/
CalculateSoftkeyAction::~CalculateSoftkeyAction()
{

}
