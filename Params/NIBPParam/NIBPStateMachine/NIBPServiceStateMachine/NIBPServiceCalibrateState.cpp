#include "NIBPParam.h"
#include "NIBPServiceCalibrateState.h"
#include "NIBPServiceStateDefine.h"
#include "NIBPCalibrate.h"
#include "NIBPRepairMenuManager.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPServiceCalibrateState::run(void)
{
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceCalibrateState::enter(void)
{
    // 测量的时限。
    nibpParam.provider().serviceCalibrate(true);
    setTimeOut();
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPServiceCalibrateState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen)
{
    switch (event)
    {
    case NIBP_EVENT_MODULE_RESET:
    case NIBP_EVENT_MODULE_ERROR:
        timeStop();
        nibpcalibrate.unPacket(false);
        _isEnterSuccess = false;
        switchState(NIBP_SERVICE_ERROR_STATE);
        break;

    case NIBP_EVENT_TIMEOUT:
    {
        nibpcalibrate.unPacket(false);
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
            switchState(NIBP_SERVICE_STANDBY_STATE);
        }
        break;

    case NIBP_EVENT_SERVICE_CALIBRATE_ENTER:
        timeStop();
        if (_isReturn)
        {
            _isReturn = false;
            if (args[0] != 0x00)
            {
                return;
            }
            else
            {
                nibpRepairMenuManager.returnMenu();
                // 转换到测量状态。
                switchState(NIBP_SERVICE_STANDBY_STATE);
            }
        }
        else
        {
            if (args[0] != 0x00)
            {
                nibpcalibrate.unPacket(false);
                nibpcalibrate.setText(trs("NIBPCalibrateModelEnterFail"));
                _isEnterSuccess = false;
                IMessageBox messbox(trs("Warn"), trs("NIBPModuleEnterFail"), false);
                messbox.setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
                messbox.setYesBtnTxt(trs("SupervisorOK"));
                messbox.exec();
            }
            else
            {
                nibpcalibrate.unPacket(true);
                _isEnterSuccess = true;
            }
        }
        break;

    case NIBP_EVENT_SERVICE_CALIBRATE_CMD_PRESSURE_POINT:
        setTimeOut();
        nibpParam.provider().servicePressurepoint(args,argLen);
        break;

    case NIBP_EVENT_SERVICE_CALIBRATE_RSP_PRESSURE_POINT:
        timeStop();
        if (args[0] != 0x01)
        {
            nibpcalibrate.unPacketPressure(false);
        }
        else
        {
            nibpcalibrate.unPacketPressure(true);
        }
    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceCalibrateState::NIBPServiceCalibrateState() : NIBPState(NIBP_SERVICE_CALIBRATE_STATE)
{
    _isReturn = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceCalibrateState::~NIBPServiceCalibrateState()
{

}
