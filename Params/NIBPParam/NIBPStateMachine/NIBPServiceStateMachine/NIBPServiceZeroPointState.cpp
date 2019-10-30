/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/20
 **/

#include "NIBPServiceZeroPointState.h"
#include "NIBPServiceStateDefine.h"
#include "NIBPParam.h"
#include "NIBPRepairMenuManager.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPServiceZeroPointState::run(void)
{
}

/**************************************************************************************************
 * 退出模式指令。
 *************************************************************************************************/
void NIBPServiceZeroPointState::triggerReturn()
{
    if (_isEnterSuccess && !nibpRepairMenuManager.getRepairError())
    {
        nibpParam.provider().serviceCalibrateZero(false);
        _isReturn = true;
    }
    else
    {
        switchState(NIBP_SERVICE_STANDBY_STATE);
    }
}

/**************************************************************************************************
 * 关闭气阀控制。
 *************************************************************************************************/
void NIBPServiceZeroPointState::serviceCloseValve(bool enter)
{
    nibpParam.provider().serviceValve(enter);
}

/**************************************************************************************************
 * 校零指令。
 *************************************************************************************************/
void NIBPServiceZeroPointState::servicePressureZero(void)
{
    nibpParam.provider().servicePressureZero();
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceZeroPointState::enter(void)
{
    // 测量的时限。
    nibpParam.provider().serviceCalibrateZero(true);
    _isReturn = false;
    setTimeOut();
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPServiceZeroPointState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int /*argLen*/)
{
    switch (event)
    {
    case NIBP_EVENT_MODULE_RESET:
    case NIBP_EVENT_MODULE_ERROR:
        _isEnterSuccess = false;
        switchState(NIBP_SERVICE_ERROR_STATE);
        break;

    case NIBP_EVENT_TIMEOUT:
    {
    }
    break;

    case NIBP_EVENT_SERVICE_REPAIR_RETURN:
        if (_isEnterSuccess && !nibpRepairMenuManager.getRepairError())
        {
            nibpParam.provider().serviceCalibrate(false);
            _isReturn = true;
            setTimeOut();
        }
        else
        {
            nibpRepairMenuManager.returnMenu();
            switchState(NIBP_SERVICE_STANDBY_STATE);
        }
        break;

    case NIBP_EVENT_SERVICE_CALIBRATE_ZERO_ENTER:
        timeStop();
        if (_isReturn)
        {
            _isReturn = false;
            if (args[0] != 0x00)
            {
                return;
            }
            else
            {
                nibpRepairMenuManager.returnMenu();
                // 转换到测量状态。
                switchState(NIBP_SERVICE_STANDBY_STATE);
            }
        }
        else
        {
            nibpParam.setResult(!args[0]);
        }
        break;

    case NIBP_EVENT_SERVICE_PRESSURECONTROL_VALVE:
        nibpParam.setResult(true);
        break;

    case NIBP_EVENT_SERVICE_PRESSURECONTROL_PUMP:
        nibpParam.setResult(!args[0]);
        break;

    case NIBP_EVENT_SERVICE_STATE_CHANGE:
        if (args[0] == 0x01)
        {
            nibpParam.provider().serviceCalibrateZero(true);
            setTimeOut();
        }
        break;

    case NIBP_EVENT_CURRENT_PRESSURE:
    {
        int pressure;
        pressure = (args[1] << 8) + args[0];
        if (pressure != -1)
        {
            nibpParam.setManometerPressure(pressure);
            return;
        }
    }
    break;
    case NIBP_EVENT_SERVICE_CALIBRATE_ZERO:
        nibpParam.setResult(true);
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceZeroPointState::NIBPServiceZeroPointState() : NIBPState(NIBP_SERVICE_CALIBRATE_ZERO_STATE),
    _isReturn(false), _isEnterSuccess(false)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceZeroPointState::~NIBPServiceZeroPointState()
{
}
