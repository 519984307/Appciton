#include "NIBPServiceRepair.h"
#include "NIBPParamService.h"
#include "NIBPRepair.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPServiceRepair::run(void)
{
    // 等待超时，返回待机模式。
    if (isTimeOut())
    {
        timeStop();
        nibprepair.unPacket(false);
        IMessageBox messbox(trs("Warn"), trs("NIBPDirectiveTimeout"), false);
        messbox.setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
        messbox.setYesBtnTxt(trs("SupervisorOK"));
        messbox.exec();
    }
}

void NIBPServiceRepair::connectError()
{
    timeStop();
    nibprepair.unPacket(false);
}

/**************************************************************************************************
 * 进入模式指令。
 *************************************************************************************************/
void NIBPServiceRepair::triggerEnter()
{
    if (provider_service == NULL)
    {
        return;
    }

    provider_service->service_Enter(true);
    setTimeOut();
}

/**************************************************************************************************
 * 退出模式指令。
 *************************************************************************************************/
void NIBPServiceRepair::triggerReturn()
{
    //停止定时器，只发送，不处理结果，
    //当第二次进入时，如果因为还处于服务模式无法进入，则显示错误；
    timeStop();
    provider_service->service_Enter(false);
}

/**************************************************************************************************
 * 解包。
 *************************************************************************************************/
void NIBPServiceRepair::unPacket(unsigned char *packet, int /*len*/)
{
    if (packet[0] == TN3_RSP_ENTER_SERVICE)
    {
        timeStop();
        if (!provider_service->isService_Enter(packet))
        {
            nibprepair.unPacket(false);
        }
        else
        {
            nibprepair.unPacket(true);
        }
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceRepair::NIBPServiceRepair() : NIBPServiceState(NIBP_Service_REPAIR)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceRepair::~NIBPServiceRepair()
{

}
