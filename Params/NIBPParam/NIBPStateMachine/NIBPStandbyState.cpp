#include "NIBPStandbyState.h"
#include "NIBPParam.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPStandbyState::run(void)
{
}

void NIBPStandbyState::enter()
{
    if (nibpParam.getMeasurMode() == NIBP_MODE_AUTO)
    {
        nibpParam.setAutoMeasure(false);
        nibpParam.setModelText(trs("NIBPAUTO") + ":" +
                               trs(NIBPSymbol::convert((NIBPIntervalTime)nibpParam.getAutoInterval())));
    }
    else if (nibpParam.getMeasurMode() == NIBP_MODE_MANUAL)
    {
        nibpParam.setModelText(trs("NIBPManual"));
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPStandbyState::NIBPStandbyState() : NIBPState(NIBP_STATE_STANDBY)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPStandbyState::~NIBPStandbyState()
{

}
