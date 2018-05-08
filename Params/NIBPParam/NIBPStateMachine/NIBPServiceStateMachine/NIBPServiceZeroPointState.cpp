#include "NIBPServiceZeroPointState.h"
#include "NIBPServiceStateDefine.h"
#include "NIBPParam.h"
#include "NIBPZeroPoint.h"
#include "NIBPRepairMenuManager.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPServiceZeroPointState::run(void)
{

}

/**************************************************************************************************
 * 退出模式指令。
 *************************************************************************************************/
void NIBPServiceZeroPointState::triggerReturn()
{
    if (_isEnterSuccess && !nibpRepairMenuManager.getRepairError())
    {
        nibpParam.provider().serviceCalibrateZero(false);
        _isReturn = true;
    }
    else
    {
        switchState(NIBP_SERVICE_STANDBY_STATE);
    }
}

/**************************************************************************************************
 * 关闭气阀控制。
 *************************************************************************************************/
void NIBPServiceZeroPointState::serviceCloseValve(bool enter)
{
    nibpParam.provider().serviceValve(enter);
}

/**************************************************************************************************
 * 校零指令。
 *************************************************************************************************/
void NIBPServiceZeroPointState::servicePressureZero(void)
{
    nibpParam.provider().servicePressureZero();
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceZeroPointState::enter(void)
{
    // 测量的时限。
    nibpParam.provider().serviceCalibrateZero(true);
    _isReturn = false;
    setTimeOut();
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPServiceZeroPointState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int /*argLen*/)
{
    switch (event)
    {
    case NIBP_EVENT_MODULE_RESET:
    case NIBP_EVENT_MODULE_ERROR:
        nibpzeropoint.unPacket(false);
        _isEnterSuccess = false;
        switchState(NIBP_SERVICE_ERROR_STATE);
        break;

    case NIBP_EVENT_TIMEOUT:
        {
        nibpzeropoint.unPacket(false);
        _isEnterSuccess = false;
        IMessageBox messbox(trs("Warn"), trs("NIBPDirectiveTimeout"), false);
        messbox.setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
        messbox.setYesBtnTxt(trs("SupervisorOK"));
        messbox.exec();
    }
        break;

    case NIBP_EVENT_SERVICE_REPAIR_RETURN:
        timeStop();
        if (_isEnterSuccess && !nibpRepairMenuManager.getRepairError())
        {
            nibpParam.provider().serviceCalibrate(false);
            _isReturn = true;
        }
        else
        {
            nibpRepairMenuManager.returnMenu();
            switchState(NIBP_SERVICE_STANDBY_STATE);
        }
        break;

    case NIBP_EVENT_SERVICE_CALIBRATE_ZERO_ENTER:
        if (_isReturn)
        {
            _isReturn = false;
            if (args[0] != 0x00)
            {
                return;
            }
            else
            {
                nibpzeropoint.init();
                nibpRepairMenuManager.returnMenu();
                // 转换到测量状态。
                switchState(NIBP_SERVICE_STANDBY_STATE);
            }
        }
        else
        {
            if (args[0] != 0x00)
            {
                nibpzeropoint.unPacket(false);
                _isEnterSuccess = false;
                IMessageBox messbox(trs("Warn"), trs("NIBPModuleEnterFail"), false);
                messbox.setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
                messbox.setYesBtnTxt(trs("SupervisorOK"));
                messbox.exec();
            }
            else
            {
                nibpzeropoint.unPacket(true);
                _isEnterSuccess = true;
            }
        }
        break;

    case NIBP_EVENT_SERVICE_PRESSURECONTROL_VALVE:
        nibpzeropoint.startSwitch(false);
        break;

    case NIBP_EVENT_SERVICE_STATE_CHANGE:
        if (args[0] == 0x01)
        {
            nibpParam.provider().serviceCalibrateZero(true);
            nibpzeropoint.startSwitch(true);
            setTimeOut();
        }
        break;

    case NIBP_EVENT_CURRENT_PRESSURE:
    {
        int pressure;
        pressure = (args[1]<<8)+args[0];
        if (pressure != -1)
        {
            nibpzeropoint.setCuffPressure(pressure);
            return;
        }
    }
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceZeroPointState::NIBPServiceZeroPointState() : NIBPState(NIBP_SERVICE_CALIBRATE_ZERO_STATE)
{
    _isReturn = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceZeroPointState::~NIBPServiceZeroPointState()
{

}
