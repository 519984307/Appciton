#include <QApplication>
#include "ECG12LeadSoftkeyAction.h"
#include "WindowManager.h"
#include "ECGParam.h"
#include "ECG12LDataAcquire.h"
#include "ECG12LSnapShotReview.h"
#include "ECG12LeadManager.h"


/***************************************************************************************************
 * 所有的快捷按键定义。
 **************************************************************************************************/
static KeyActionDesc _12LeadKeys[] =
{
//    KeyActionDesc("", "12LeadCapture.png", ECG12LeadSoftkeyAction::ecg12LeadCapture),
//    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PAT_INFO),
//    KeyActionDesc("", "12LeadData.png", ECG12LeadSoftkeyAction::ecg12LeadReview),
//    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, Qt::black, Qt::black, Qt::black, false),
//    KeyActionDesc("", "12LeadExit.png", ECG12LeadSoftkeyAction::ecg12LeadExit),

    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PAT_INFO),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PAT_NEW),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PREVIOUS_PAGE),
    KeyActionDesc("", "DiagECG.png", ECG12LeadSoftkeyAction::ecg12LeaddiagECG),
    KeyActionDesc("", "12LeadCapture.png", ECG12LeadSoftkeyAction::ecg12LeadCapture),
    KeyActionDesc("", "12LeadData.png", ECG12LeadSoftkeyAction::ecg12LeadReview),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_ALARM_LIMIT),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_CODE_MARKER),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_RESCUE_DATA),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_CALCULATION),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NEXT_PAGE),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_MAIN_SETUP),
};

/***************************************************************************************************
 * 12导诊断。
 **************************************************************************************************/
void ECG12LeadSoftkeyAction::ecg12LeaddiagECG(bool isPressed)
{
    if (isPressed)
    {
        ecgParam.setDiagBandwidth();
        return;
    }

    ecgParam.restoreDiagBandwidth();
}

/***************************************************************************************************
 * 12导数据获取。
 **************************************************************************************************/
void ECG12LeadSoftkeyAction::ecg12LeadCapture(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    ecg12LeadManager.startCapture();
}

/***************************************************************************************************
 * 12导数据回顾。
 **************************************************************************************************/
void ECG12LeadSoftkeyAction::ecg12LeadReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    ecg12LeadManager.startReview();
}

/***************************************************************************************************
 * 退出12导回调。
 **************************************************************************************************/
void ECG12LeadSoftkeyAction::ecg12LeadExit(bool isPressed)
{
    if (isPressed|| ecg12LDataAcquire.isAcquiring() || ecg12LeadManager.isTransfer())
    {
        return;
    }

//    patientMenu.hide();
    ecg12LSnapShotReview.hide();
    ecg12LDataAcquire.hide();
    ecgParam.exit12Lead();

}

/***************************************************************************************************
 * 获取动作描述总个数。
 **************************************************************************************************/
int ECG12LeadSoftkeyAction::getActionDescNR(void)
{
    return sizeof(_12LeadKeys) / sizeof(KeyActionDesc);
}

/***************************************************************************************************
 * 获取动作描述句柄。
 **************************************************************************************************/
KeyActionDesc *ECG12LeadSoftkeyAction::getActionDesc(int index)
{
    if (index >= (int)(sizeof(_12LeadKeys) / sizeof(KeyActionDesc)))
    {
        return NULL;
    }

    if (_12LeadKeys[index].type < SOFT_BASE_KEY_NR)
    {
        return SoftkeyActionBase::getActionDesc(_12LeadKeys[index].type);
    }

    return &_12LeadKeys[index];
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
ECG12LeadSoftkeyAction::ECG12LeadSoftkeyAction() : SoftkeyActionBase(SOFTKEY_ACTION_12LEAD)
{

}

/***************************************************************************************************
 * 析构。
 **************************************************************************************************/
ECG12LeadSoftkeyAction::~ECG12LeadSoftkeyAction()
{

}
