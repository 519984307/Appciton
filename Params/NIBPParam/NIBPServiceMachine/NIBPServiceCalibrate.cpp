#include "NIBPServiceCalibrate.h"
#include "NIBPParamService.h"
#include "NIBPCalibrate.h"
#include "NIBPRepair.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPServiceCalibrate::run(void)
{
    // 等待超时，返回待机模式。
    if (isTimeOut())
    {
        timeStop();
        nibpcalibrate.unPacket(false);
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
void NIBPServiceCalibrate::triggerReturn()
{
    if (_isEnterSuccess && !nibprepair.getRepairError())
    {
        provider_service->service_Calibrate(false);
        _isReturn = true;
    }
    else
    {
        nibprepair.returnMenu();
        nibpParamService.switchState(NIBP_Service_REPAIR);
    }
}

/**************************************************************************************************
 * 发送校准点值。
 *************************************************************************************************/
void NIBPServiceCalibrate::calibratedPoint(int point, int value)
{
    if (point == 0)
    {
        provider_service->service_Pressurepoint(0, 0);
        setTimeOut(5000);
    }

    if (point == 1)
    {
        provider_service->service_Pressurepoint(1, value);
        setTimeOut(5000);
    }
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceCalibrate::enter(void)
{
    // 测量的时限。
    provider_service->service_Calibrate(true);
    setTimeOut();
}

void NIBPServiceCalibrate::connectError()
{
    timeStop();
    nibpcalibrate.unPacket(false);
    _isEnterSuccess = false;
}

/**************************************************************************************************
 * 解包。
 *************************************************************************************************/
void NIBPServiceCalibrate::unPacket(unsigned char *packet, int /*len*/)
{
    if (packet[0] == TN3_RSP_CALIBRATE)
    {
        timeStop();
        if (_isReturn)
        {
            _isReturn = false;
            if (!provider_service->isService_Calibrate(packet))
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
            if (!provider_service->isService_Calibrate(packet))
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

    }

    if (packet[0] == TN3_RSP_PRESSURE_POINT)
    {
        timeStop();
        if (!provider_service->isService_Pressurepoint(packet))
        {
            nibpcalibrate.unPacketPressure(false);
        }
        else
        {
            nibpcalibrate.unPacketPressure(true);
        }
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceCalibrate::NIBPServiceCalibrate() : NIBPServiceState(NIBP_Service_CALIBRATE)
{
    _isReturn = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceCalibrate::~NIBPServiceCalibrate()
{

}
