/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/20
 **/

#include "NIBPServiceManometerState.h"
#include "NIBPServiceStateDefine.h"
#include "NIBPParam.h"
#include "NIBPMaintainMgrInterface.h"

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceManometerState::enter(void)
{
    // 测量的时限。
    nibpParam.provider().serviceManometer(true);
    nibpParam.provider().servicePressureProtect(false);
    setTimeOut();
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPServiceManometerState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int /*argLen*/)
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
    {
        NIBPMaintainMgrInterface *nibpMaintainMgr;
        nibpMaintainMgr = NIBPMaintainMgrInterface::getNIBPMaintainMgr();
        if (_isEnterSuccess && nibpMaintainMgr && !nibpMaintainMgr->getRepairError())
        {
            nibpParam.provider().serviceCalibrate(false);
            _isReturn = true;
            setTimeOut();
        }
        else
        {
            switchState(NIBP_SERVICE_STANDBY_STATE);
        }
    }
        break;

    case NIBP_EVENT_SERVICE_MANOMETER_ENTER:
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
                switchState(NIBP_SERVICE_STANDBY_STATE);
            }
        }
        else
        {
            nibpParam.setResult(!args[0]);
        }
        break;

    case NIBP_EVENT_CURRENT_PRESSURE:
    {
        int16_t pressure;
        pressure = (args[1] << 8) + args[0];
        if (pressure != -1)
        {
            nibpParam.setManometerPressure(pressure);
            return;
        }
    }
    break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceManometerState::NIBPServiceManometerState() : NIBPState(NIBP_SERVICE_MANOMETER_STATE),
    _isReturn(false), _isEnterSuccess(false)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceManometerState::~NIBPServiceManometerState()
{
}
