/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/20
 **/

#include "NIBPServicePressureControlState.h"
#include "NIBPServiceStateDefine.h"
#include "NIBPParam.h"
#include "NIBPMaintainMgrInterface.h"

/**************************************************************************************************
 * 充气压力指令。
 *************************************************************************************************/
void NIBPServicePressureControlState::servicePressureInflate(int Value)
{
    nibpParam.provider().servicePressureinflate(Value);
}

/**************************************************************************************************
 * 放气指令。
 *************************************************************************************************/
void NIBPServicePressureControlState::servicePressureDeflate(void)
{
    nibpParam.provider().servicePressuredeflate();
}

/**************************************************************************************************
 * 设置病人类型。
 *************************************************************************************************/
void NIBPServicePressureControlState::servicePatientType(unsigned char type)
{
    nibpParam.provider().setPatientType(type);
}

/**************************************************************************************************
 * 过压保护。
 *************************************************************************************************/
void NIBPServicePressureControlState::servicePressureProtect(bool enter)
{
    nibpParam.provider().servicePressureProtect(enter);
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServicePressureControlState::enter(void)
{
    // 测量的时限。
    nibpParam.provider().servicePressurecontrol(true);
    setTimeOut();
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPServicePressureControlState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int /*argLen*/)
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

    case NIBP_EVENT_SERVICE_PRESSURECONTROL_ENTER:
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
                // 转换到测量状态。
                switchState(NIBP_SERVICE_STANDBY_STATE);
            }
        }
        else
        {
            nibpParam.setResult(!args[0]);
        }
        break;

    case NIBP_EVENT_SERVICE_STATE_CHANGE:
        if (args[0] == 0x01)
        {
            nibpParam.provider().servicePressurecontrol(true);
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
    case NIBP_EVENT_SERVICE_PRESSURECONTROL_INFLATE:
        nibpParam.setResult(!args[0]);
        break;
    case NIBP_EVENT_SERVICE_PRESSURECONTROL_DEFLATE:
        nibpParam.setResult(true);
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServicePressureControlState::NIBPServicePressureControlState() :
    NIBPState(NIBP_SERVICE_PRESSURECONTROL_STATE), _isReturn(false), _isEnterSuccess(false)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServicePressureControlState::~NIBPServicePressureControlState()
{
}
