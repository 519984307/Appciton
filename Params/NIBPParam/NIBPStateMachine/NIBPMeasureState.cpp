#include "NIBPMeasureState.h"
#include "NIBPParam.h"
#include "NIBPAlarm.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPMeasureState::run(void)
{
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
void NIBPMeasureState::enter(void)
{
    setTimeOut();
    nibpParam.setSnapshotFlag(true);
}

/**************************************************************************************************
 * 解包。
 *************************************************************************************************/
void NIBPMeasureState::unPacket(unsigned char *packet, int /*len*/)
{
    setTimeOut(10000);//需要下移

    if (provider->isStopACK(packet))
    {
        // 停止测量。
        nibpParam.switchState(NIBP_STATE_STOPING);
        return;
    }

    // 获取压力数据。
    short pressure = provider->cuffPressure(packet);
    if (pressure != -1)
    {
        nibpParam.setCuffPressure(pressure);
        return;
    }

    // 是否收到异常数据。
    NIBPOneShotType error = provider->isMeasureError(packet);
    if (error != NIBP_ONESHOT_NONE)
    {
        nibpOneShotAlarm.setOneShotAlarm(error, true);
    }

    // 是否为测量结束。
    if (provider->isMeasureDone(packet))
    {
        //provider->stopMeasure();//不隐去会导致连续测量无法继续
        nibpParam.switchState(NIBP_STATE_GET_RESULT);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPMeasureState::NIBPMeasureState() : NIBPState(NIBP_STATE_MEASURING)
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPMeasureState::~NIBPMeasureState()
{

}
