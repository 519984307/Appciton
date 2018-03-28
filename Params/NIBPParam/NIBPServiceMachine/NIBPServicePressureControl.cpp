#include "NIBPServicePressureControl.h"
#include "NIBPParamService.h"
#include "NIBPPressureControl.h"
#include "NIBPRepair.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPServicePressureControl::run(void)
{
    // 等待超时，返回待机模式。
    if (isTimeOut())
    {
        timeStop();
        nibppressurecontrol.unPacket(false);
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
void NIBPServicePressureControl::triggerReturn()
{
    if(_isEnterSuccess && !nibprepair.getRepairError())
    {
        provider_service->service_Pressurecontrol(false);
        _isReturn = true;
    }
    else
    {
        nibprepair.returnMenu();
        nibpParamService.switchState(NIBP_Service_REPAIR);
    }
}

/**************************************************************************************************
 * 充气压力指令。
 *************************************************************************************************/
void NIBPServicePressureControl::servicePressureInflate(int Value)
{
    provider_service->service_Pressureinflate(Value);
}

/**************************************************************************************************
 * 放气指令。
 *************************************************************************************************/
void NIBPServicePressureControl::servicePressureDeflate(void)
{
    provider_service->service_Pressuredeflate();
}

/**************************************************************************************************
 * 设置病人类型。
 *************************************************************************************************/
void NIBPServicePressureControl::servicePatientType(unsigned char type)
{
    provider_service->setPatientType(type);
}

/**************************************************************************************************
 * 过压保护。
 *************************************************************************************************/
void NIBPServicePressureControl::servicePressureProtect(bool enter)
{
    provider_service->service_PressureProtect(enter);
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServicePressureControl::enter(void)
{
    // 测量的时限。
    provider_service->service_Pressurecontrol(true);
    setTimeOut();
}

void NIBPServicePressureControl::connectError()
{
    timeStop();
    nibppressurecontrol.unPacket(false);
    _isEnterSuccess = false;
}

/**************************************************************************************************
 * 解包。
 *************************************************************************************************/
void NIBPServicePressureControl::unPacket(unsigned char *packet, int /*len*/)
{
    if (packet[0] == TN3_RSP_PRESSURE_CONTROL)
    {
        timeStop();
        if (_isReturn)
        {
            _isReturn = false;
            if (!provider_service->isService_Pressurecontrol(packet))
            {
                return;
            }
            else
            {
                nibprepair.returnMenu();
                // 转换到测量状态。
                nibpParamService.switchState(NIBP_Service_REPAIR);
            }
        }
        else
        {
            if (!provider_service->isService_Pressurecontrol(packet))
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
    }

    if ((packet[0] == TN3_STATE_CHANGE) && (packet[1] == 0x01))
    {
        timeStop();
        provider_service->service_Pressurecontrol(true);
        nibppressurecontrol.btnSwitch(true);
        setTimeOut();
    }

    if (packet[0] == TN3_RSP_PRESSURE_DEFLATE)
    {
        timeStop();
        if (!provider_service->isService_Pressuredeflate(packet))
        {
            return;
        }
        else
        {
            nibppressurecontrol.btnSwitch(true);
        }
    }

    if (packet[0] == TN3_SERVICE_PRESSURE)
    {
        timeStop();
        int pressure;
        pressure = provider_service->service_cuffPressure(packet);
        if (pressure != -1)
        {
            nibppressurecontrol.setCuffPressure(pressure);
            return;
        }
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServicePressureControl::NIBPServicePressureControl() :
    NIBPServiceState(NIBP_Service_PRESSURECONTROL)
{
    _isReturn = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServicePressureControl::~NIBPServicePressureControl()
{

}
