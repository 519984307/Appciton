#include "NIBPServiceManometer.h"
#include "NIBPParamService.h"
#include "NIBPManometer.h"
#include "NIBPRepair.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPServiceManometer::run(void)
{
    // 等待超时，返回待机模式。
    if (isTimeOut())
    {
        timeStop();
        nibpmanometer.unPacket(false);
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
void NIBPServiceManometer::triggerReturn()
{
    if(_isEnterSuccess && !nibprepair.getRepairError())
    {
        provider_service->service_Manometer(false);
        provider_service->service_PressureProtect(true);
        _isReturn = true;
    }
    else
    {
        nibprepair.returnMenu();
        nibpParamService.switchState(NIBP_Service_REPAIR);
    }
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceManometer::enter(void)
{
    // 测量的时限。
    provider_service->service_Manometer(true);
    provider_service->service_PressureProtect(false);
    setTimeOut();
}

void NIBPServiceManometer::connectError()
{
    timeStop();
    nibpmanometer.unPacket(false);
    _isEnterSuccess = false;
}

/**************************************************************************************************
 * 解包。
 *************************************************************************************************/
void NIBPServiceManometer::unPacket(unsigned char *packet, int /*len*/)
{
    if (packet[0] == TN3_RSP_MANOMETER)
    {
        timeStop();
        if(_isReturn)
        {
            _isReturn = false;
            if (!provider_service->isService_Manometer(packet))
            {
                return;
            }
            else
            {
                nibpmanometer.init();
                nibprepair.returnMenu();
                nibpParamService.switchState(NIBP_Service_REPAIR);
            }
        }
        else
        {
            if (!provider_service->isService_Manometer(packet))
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
    }
    if (packet[0] == TN3_SERVICE_PRESSURE)
    {
        int pressure;
        timeStop();
        pressure = provider_service->service_cuffPressure(packet);
        if (pressure != -1)
        {
            nibpmanometer.setCuffPressure(pressure);
            return;
        }
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceManometer::NIBPServiceManometer() : NIBPServiceState(NIBP_Service_MANOMETER)
{
    _isReturn = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceManometer::~NIBPServiceManometer()
{

}
