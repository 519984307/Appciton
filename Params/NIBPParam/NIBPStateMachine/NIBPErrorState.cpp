#include "NIBPErrorState.h"
#include "NIBPParam.h"
#include "NIBPAlarm.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPErrorState::run(void)
{
    // 模块错误,不可恢复
    if (nibpParam.isErrorDisable())
    {
        return;
    }

    // 等待超时，返回待机模式。
    if (isTimeOut() && nibpParam.isConnected())
    {
        //恢复STAT按钮
        nibpParam.setSTATMeasure(false);
        //防止在错误状态时间内，产生的倒计时测量触发
        nibpCountdownTime.setAutoMeasureTimeout(false);
        nibpParam.switchState(NIBP_STATE_STANDBY);
        nibpParam.setText(InvStr());
    }
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPErrorState::enter(void)
{
    provider->stopMeasure();

    //通信中断在超时中处理
    //停止的触发时间
    nibpParam.createSnapshot(NIBP_ONESHOT_ABORT);

    // 进入此状态最少持续5s
    setTimeOut(5 * 1000);

    nibpParam.setAdditionalMeasure(false);

    nibpParam.setText(trs("NIBPModule") + "\n" + trs("NIBPDisable"));
    nibpParam.setModelText("");
    nibpParam.clearResult();

    //
    nibpCountdownTime.STATMeasureStop();
    nibpParam.setSTATMeasure(false);
    nibpParam.setSTATClose(false);
    //自动测量模式中的手动触发关闭
    nibpParam.setAutoMeasure(false);
    //安全间隔结束，临时状态被关闭
    nibpParam.setSTATOpenTemp(false);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPErrorState::NIBPErrorState() : NIBPState(NIBP_STATE_ERROR)
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPErrorState::~NIBPErrorState()
{

}
