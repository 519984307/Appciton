#include "NIBPStartingState.h"
#include "NIBPParam.h"
#include "PatientManager.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPStartingState::run(void)
{
//    if (!_isWaiting)
//    {
//        return;
//    }

    // 等待超时，返回待机模式。
    if (isTimeOut())
    {
//        nibpParam.connectedTimeout();
        nibpParam.switchState(NIBP_STATE_ERROR);
    }
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPStartingState::enter(void)
{
    // 启动定时器，不能无限期地等待下去。
    // 将定时器上移，相当于在每次测量时初始化定时器；
    setTimeOut();

    //清除显示的数据。
    nibpParam.invResultData();
    nibpParam.setText(InvStr());

    //清除标志位
    nibpParam.setSwitchFlagTime(false);
    nibpParam.setSwitchFlagType(false);

//    provider->setPatientType((unsigned char)patientManager.getType());
//    provider->setInitPressure(nibpParam.getInitPressure(patientManager.getType()));

    provider->startMeasure(patientManager.getType());

    // 如果不需要启动应答则立即切换到测量状态。
    if (!provider->needStartACK())
    {
        nibpParam.switchState(NIBP_STATE_MEASURING);
        return;
    }
}

/**************************************************************************************************
 * 解包。
 *************************************************************************************************/
void NIBPStartingState::unPacket(unsigned char *packet, int /*len*/)
{
    if(NULL == provider)
        return;
    if (provider->isStopACK(packet))
    {
        // 停止测量。
        nibpParam.switchState(NIBP_STATE_STOPING);
        return;
    }

    // 是否为启动应答包，如果不是直接返回。
    if (!provider->isStartACK(packet))
    {
        return;
    }

    // 获得应答，切换到测量状态。
    nibpParam.switchState(NIBP_STATE_MEASURING);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPStartingState::NIBPStartingState() : NIBPState(NIBP_STATE_STARTING)
{
//    _isWaiting = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPStartingState::~NIBPStartingState()
{

}
