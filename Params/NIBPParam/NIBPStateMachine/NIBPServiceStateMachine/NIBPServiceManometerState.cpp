#include "NIBPServiceManometerState.h"
#include "NIBPServiceStateDefine.h"
#include "NIBPParam.h"
#include "NIBPManometer.h"
#include "NIBPRepairMenuManager.h"

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceManometerState::enter(void)
{
    // 测量的时限。
    nibpParam.provider().serviceManometer(true);
    nibpParam.provider().servicePressureProtect(false);
    setTimeOut();
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPServiceManometerState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int /*argLen*/)
{
    switch (event)
    {
    case NIBP_EVENT_MODULE_RESET:
    case NIBP_EVENT_MODULE_ERROR:
        nibpmanometer.unPacket(false);
        _isEnterSuccess = false;
        switchState(NIBP_SERVICE_ERROR_STATE);
        break;

    case NIBP_EVENT_TIMEOUT:
    {
        nibpmanometer.unPacket(false);
        _isEnterSuccess = false;
        IMessageBox messbox(trs("Warn"), trs("NIBPDirectiveTimeout"), false);
        messbox.setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
        messbox.setYesBtnTxt(trs("SupervisorOK"));
        messbox.exec();
    }
        break;

    case NIBP_EVENT_SERVICE_REPAIR_RETURN:
        if (_isEnterSuccess && !nibpRepairMenuManager.getRepairError())
        {
            nibpParam.provider().serviceCalibrate(false);
            _isReturn = true;
            setTimeOut();
        }
        else
        {
            switchState(NIBP_SERVICE_STANDBY_STATE);
        }
        break;

    case NIBP_EVENT_SERVICE_MANOMETER_ENTER:
        timeStop();
        if(_isReturn)
        {
            _isReturn = false;
            if (args[0] != 0x00)
            {
                return;
            }
            else
            {
                nibpmanometer.init();
                nibpRepairMenuManager.returnMenu();
                switchState(NIBP_SERVICE_STANDBY_STATE);
            }
        }
        else
        {
            if (args[0] != 0x00)
            {
                nibpmanometer.unPacket(false);
                _isEnterSuccess = false;
                IMessageBox messbox(trs("Warn"), trs("NIBPModuleEnterFail"), false);
                messbox.setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
                messbox.setYesBtnTxt(trs("SupervisorOK"));
                messbox.exec();
            }
            else
            {
                nibpmanometer.unPacket(true);
                _isEnterSuccess = true;
            }
        }
        break;

    case NIBP_EVENT_CURRENT_PRESSURE:
    {
        int pressure;
        pressure = (args[1]<<8)+args[0];
        if (pressure != -1)
        {
            nibpParam.setCuffPressure(pressure);
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
NIBPServiceManometerState::NIBPServiceManometerState() : NIBPState(NIBP_SERVICE_MANOMETER_STATE)
{
    _isReturn = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceManometerState::~NIBPServiceManometerState()
{

}
