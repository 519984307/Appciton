#include "NIBPStopState.h"
#include "NIBPParam.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPStopState::run(void)
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
void NIBPStopState::enter(void)
{
    //通信中断在超时中处理
    //停止的触发时间
    nibpParam.createSnapshot(NIBP_ONESHOT_ABORT);

    // 启动定时器，不能无限期地等待下去。
    setTimeOut();

    provider->stopMeasure();

    nibpParam.setAdditionalMeasure(false);

    if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
    {
        nibpCountdownTime.STATMeasureStop();
        nibpParam.setSTATMeasure(false);               //退出STAT
        nibpParam.setSTATClose(true);                  //置STAT关闭标志
        nibpParam.setModelText(trs("STATSTOPPED"));
    }
    else
    {
        if (nibpParam.isAutoMeasure())
        {
            nibpParam.setModelText(trs("NIBPManual"));
        }
    }

    // 如果不需要启动应答则立即切换到测量状态。
    if (!provider->needStopACK())
    {
        nibpParam.setText(trs("NIBPREADING") + "\n" + trs("NIBPSTOPPED"));
        nibpParam.clearResult();
        nibpParam.switchState(NIBP_STATE_SAFEWAITTIME);
        return;
    }
}

/**************************************************************************************************
 * 解包。
 *************************************************************************************************/
void NIBPStopState::unPacket(unsigned char *packet, int /*len*/)
{
    // 是否为启动应答包，如果不是直接返回。
    if (!provider->isStopACK(packet))
    {
        return;
    }
    nibpParam.setText(trs("NIBPREADING") + "\n" + trs("NIBPSTOPPED"));
    nibpParam.clearResult();

    nibpParam.switchState(NIBP_STATE_SAFEWAITTIME);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPStopState::NIBPStopState() : NIBPState(NIBP_STATE_STOPING)
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPStopState::~NIBPStopState()
{

}
