#include "NIBPServicePressureControlState.h"
#include "NIBPServiceStateDefine.h"
#include "NIBPParam.h"
#include "NIBPPressureControl.h"
#include "NIBPRepairMenuManager.h"

/**************************************************************************************************
 * 充气压力指令。
 *************************************************************************************************/
void NIBPServicePressureControlState::servicePressureInflate(int Value)
{
    nibpParam.provider().servicePressureinflate(Value);
}

/**************************************************************************************************
 * 放气指令。
 *************************************************************************************************/
void NIBPServicePressureControlState::servicePressureDeflate(void)
{
    nibpParam.provider().servicePressuredeflate();
}

/**************************************************************************************************
 * 设置病人类型。
 *************************************************************************************************/
void NIBPServicePressureControlState::servicePatientType(unsigned char type)
{
    nibpParam.provider().setPatientType(type);
}

/**************************************************************************************************
 * 过压保护。
 *************************************************************************************************/
void NIBPServicePressureControlState::servicePressureProtect(bool enter)
{
    nibpParam.provider().servicePressureProtect(enter);
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServicePressureControlState::enter(void)
{
    // 测量的时限。
    nibpParam.provider().servicePressurecontrol(true);
    setTimeOut();
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPServicePressureControlState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int /*argLen*/)
{
    switch (event)
    {
    case NIBP_EVENT_MODULE_RESET:
    case NIBP_EVENT_MODULE_ERROR:
        nibppressurecontrol.unPacket(false);
        _isEnterSuccess = false;
        switchState(NIBP_SERVICE_ERROR_STATE);
        break;

    case NIBP_EVENT_TIMEOUT:
    {
        nibppressurecontrol.unPacket(false);
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

    case NIBP_EVENT_SERVICE_PRESSURECONTROL_ENTER:
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
                nibppressurecontrol.unPacket(false);
                _isEnterSuccess = false;
                IMessageBox messbox(trs("Warn"), trs("NIBPModuleEnterFail"), false);
                messbox.setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
                messbox.setYesBtnTxt(trs("SupervisorOK"));
                messbox.exec();
            }
            else
            {
                nibppressurecontrol.unPacket(true);
                _isEnterSuccess = true;
            }
        }
        break;

    case NIBP_EVENT_SERVICE_STATE_CHANGE:
        if (args[0] == 0x01)
        {
            nibpParam.provider().servicePressurecontrol(true);
            nibppressurecontrol.btnSwitch(true);
            setTimeOut();
        }
        break;

    case NIBP_EVENT_SERVICE_PRESSURECONTROL_DEFLATE:
        nibppressurecontrol.btnSwitch(true);
        break;

    case NIBP_EVENT_CURRENT_PRESSURE:
    {
        int pressure;
        pressure = (args[1]<<8)+args[0];
        if (pressure != -1)
        {
            nibppressurecontrol.setCuffPressure(pressure);
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
NIBPServicePressureControlState::NIBPServicePressureControlState() :
    NIBPState(NIBP_SERVICE_PRESSURECONTROL_STATE)
{
    _isReturn = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServicePressureControlState::~NIBPServicePressureControlState()
{

}
