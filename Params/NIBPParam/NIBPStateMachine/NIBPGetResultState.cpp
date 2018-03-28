#include "NIBPGetResultState.h"
#include "NIBPParam.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPGetResultState::run(void)
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
void NIBPGetResultState::enter(void)
{
    // 测量的时限。
    setTimeOut();
    provider->getResult();
}
/**************************************************************************************************
 * 解包。
 *************************************************************************************************/
void NIBPGetResultState::unPacket(unsigned char *packet, int /*len*/)
{
    if (provider->isStopACK(packet))
    {
        // 停止测量。
        nibpParam.switchState(NIBP_STATE_STOPING);
        return;
    }

    short sys(InvData()), dia(InvData()), map(InvData()), pr(InvData());
    NIBPOneShotType err;

    if (!provider->isResult(packet, sys, dia, map, pr, err))  // 不是测量结果。
    {
        return;
    }
    // 将结果传给NIBPParam处理。

    nibpParam.setResult(sys, dia, map, pr, err);

    debug("0x%02x, %d",packet[1],err);

    nibpParam.switchState(NIBP_STATE_SAFEWAITTIME);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPGetResultState::NIBPGetResultState() : NIBPState(NIBP_STATE_GET_RESULT)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPGetResultState::~NIBPGetResultState()
{

}
