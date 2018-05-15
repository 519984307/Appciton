#include "NIBPMonitorGetResultState.h"
#include "NIBPParam.h"

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPMonitorGetResultState::enter(void)
{
    // 测量的时限。
    setTimeOut();
    nibpParam.provider().getResult();
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPMonitorGetResultState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen)
{
    short sys(InvData()), dia(InvData()), map(InvData()), pr(InvData());
    NIBPOneShotType err;

    switch (event)
    {
    case NIBP_EVENT_MODULE_RESET:
        switchState(NIBP_MONITOR_STOPE_STATE);
        break;

    case NIBP_EVENT_MODULE_ERROR:
    case NIBP_EVENT_TIMEOUT:
        switchState(NIBP_MONITOR_ERROR_STATE);
        break;

    case NIBP_EVENT_TRIGGER_BUTTON:
        switchState(NIBP_MONITOR_STOPE_STATE);
        break;

    case NIBP_EVENT_TRIGGER_PATIENT_TYPE:
        switchState(NIBP_MONITOR_STOPE_STATE);
        break;

    case NIBP_EVENT_TRIGGER_MODEL:
        if (args[0] == 0x01)
        {
            if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
            {
                switchState(NIBP_MONITOR_STOPE_STATE);
            }
            else
            {
                nibpParam.setSTATMeasure(true);
                nibpCountdownTime.STATMeasureStart(); // 只测量5分钟。
            }
            break;
        }

        if (nibpParam.getSuperMeasurMode() == NIBP_MODE_AUTO)
        {
            nibpParam.switchToAuto();
        }
        else
        {
            nibpParam.switchToManual();
        }
        switchState(NIBP_MONITOR_SAFEWAITTIME_STATE);
        break;

    case NIBP_EVENT_MONITOR_STOP:
        switchState(NIBP_MONITOR_STOPE_STATE);
        break;

    case NIBP_EVENT_MONITOR_GET_RESULT:
        if (!nibpParam.analysisResult(args, argLen, sys, dia, map, pr, err))  // 不是测量结果。
        {
            return;
        }
        // 将结果传给NIBPParam处理。

        nibpParam.setResult(sys, dia, map, pr, err);

        debug("0x%02x, %d",args[1],err);

        switchState(NIBP_MONITOR_SAFEWAITTIME_STATE);
        break;
    default:
        break;
    }

}
/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPMonitorGetResultState::NIBPMonitorGetResultState() : NIBPState(NIBP_MONITOR_GET_RESULT_STATE)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPMonitorGetResultState::~NIBPMonitorGetResultState()
{

}