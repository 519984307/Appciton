#include "NIBPServiceErrorState.h"
#include "NIBPParam.h"
#include "NIBPAlarm.h"
#include "NIBPRepairMenuManager.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceErrorState::enter(void)
{
    nibpRepairMenuManager.setMonitorState(NIBP_MONITOR_ERROR_STATE);
    nibpRepairMenuManager.warnShow(true);
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPServiceErrorState::handleNIBPEvent(NIBPEvent /*event*/, const unsigned char */*args*/, int /*argLen*/)
{

}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceErrorState::NIBPServiceErrorState() : NIBPState(NIBP_SERVICE_ERROR_STATE)
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceErrorState::~NIBPServiceErrorState()
{

}
