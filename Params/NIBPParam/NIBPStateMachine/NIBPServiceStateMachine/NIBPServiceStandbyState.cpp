#include "NIBPServiceStandbyState.h"
#include "NIBPServiceStateDefine.h"
#include "NIBPParam.h"
#include "NIBPRepairMenuManager.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPServiceStandbyState::run(void)
{

}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceStandbyState::enter(void)
{
    timeStop();
    nibpRepairMenuManager.returnMenu();
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPServiceStandbyState::handleNIBPEvent(NIBPEvent event, const unsigned char */*args*/, int /*argLen*/)
{
    switch (event)
    {
    case NIBP_EVENT_MODULE_RESET:
        nibpRepairMenuManager.unPacket(false);
        switchState(NIBP_SERVICE_ERROR_STATE);
        break;

    case NIBP_EVENT_TIMEOUT:
    {
        nibpRepairMenuManager.unPacket(false);
        IMessageBox messbox(trs("Warn"), trs("NIBPDirectiveTimeout"), false);
        messbox.setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
        messbox.setYesBtnTxt(trs("SupervisorOK"));
        messbox.exec();
    }
        break;

    case NIBP_EVENT_SERVICE_REPAIR_ENTER_SUCCESS:
        nibpRepairMenuManager.unPacket(true);
        break;

    case NIBP_EVENT_SERVICE_REPAIR_ENTER_FAIL:
        nibpRepairMenuManager.unPacket(false);
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceStandbyState::NIBPServiceStandbyState() : NIBPState(NIBP_SERVICE_STANDBY_STATE)
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceStandbyState::~NIBPServiceStandbyState()
{

}
