#include "NIBPServiceZeroPoint.h"
#include "NIBPParamService.h"
#include "NIBPZeroPoint.h"
#include "NIBPRepair.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPServiceZeroPoint::run(void)
{
    // 等待超时，返回待机模式。
    if (isTimeOut())
    {
        timeStop();
        nibpzeropoint.unPacket(false);
        _isEnterSuccess = false;
        IMessageBox messbox(trs("Warn"), trs("NIBPDirectiveTimeout"), false);
        messbox.setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
        messbox.setYesBtnTxt(trs("SupervisorOK"));
        messbox.exec();
    }
}

/**************************************************************************************************
 * 退出模式指令。
 *************************************************************************************************/
void NIBPServiceZeroPoint::triggerReturn()
{
    if (_isEnterSuccess && !nibprepair.getRepairError())
    {
        provider_service->service_CalibrateZero(false);
        _isReturn = true;
    }
    else
    {
        nibprepair.returnMenu();
        nibpParamService.switchState(NIBP_Service_REPAIR);
    }
}

/**************************************************************************************************
 * 关闭气阀控制。
 *************************************************************************************************/
void NIBPServiceZeroPoint::serviceCloseValve(bool enter)
{
    provider_service->service_Valve(enter);
}

/**************************************************************************************************
 * 校零指令。
 *************************************************************************************************/
void NIBPServiceZeroPoint::servicePressureZero(void)
{
    provider_service->service_PressureZero();
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceZeroPoint::enter(void)
{
    // 测量的时限。
    provider_service->service_CalibrateZero(true);
    _isReturn = false;
    setTimeOut();
}

void NIBPServiceZeroPoint::connectError(void)
{
    timeStop();
    nibpzeropoint.unPacket(false);
    _isEnterSuccess = false;
}

/**************************************************************************************************
 * 解包。
 *************************************************************************************************/
void NIBPServiceZeroPoint::unPacket(unsigned char *packet, int /*len*/)
{
    if (packet[0] == TN3_RSP_CALIBRATE_Zero)
    {
        timeStop();
        if (_isReturn)
        {
            _isReturn = false;
            if (!provider_service->isService_CalibrateZero(packet))
            {
                return;
            }
            else
            {
                nibpzeropoint.init();
                nibprepair.returnMenu();
                // 转换到测量状态。
                nibpParamService.switchState(NIBP_Service_REPAIR);
            }
        }
        else
        {
            if (!provider_service->isService_CalibrateZero(packet))
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
    }

    if (packet[0] == TN3_RSP_Valve)
    {
        timeStop();
        if (!provider_service->isService_Valve(packet))
        {
            return;
        }
        else
        {
            nibpzeropoint.startSwitch(false);
        }

    }

    if ((packet[0] == TN3_STATE_CHANGE) && (packet[1] == 0x01))
    {
        timeStop();
        provider_service->service_CalibrateZero(true);
        nibpzeropoint.startSwitch(true);
        setTimeOut();
    }

    if (packet[0] == TN3_SERVICE_PRESSURE)
    {
        timeStop();
        int pressure;
        pressure = provider_service->service_cuffPressure(packet);
        if (pressure != -1)
        {
            nibpzeropoint.setCuffPressure(pressure);
            return;
        }
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceZeroPoint::NIBPServiceZeroPoint() : NIBPServiceState(NIBP_Service_CALIBRATE_ZERO)
{
    _isReturn = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceZeroPoint::~NIBPServiceZeroPoint()
{

}
