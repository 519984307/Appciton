/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/16
 **/

#include "NIBPParam.h"
#include "NIBPServiceCalibrateState.h"
#include "NIBPServiceStateDefine.h"
#include "NIBPMaintainMgrInterface.h"
#include "MessageBox.h"
#include "LanguageManager.h"

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPServiceCalibrateState::enter(void)
{
    // 测量的时限。
    nibpParam.provider().serviceCalibrate(true);
    setTimeOut();
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPServiceCalibrateState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int argLen)
{
    switch (event)
    {
    case NIBP_EVENT_MODULE_RESET:
    case NIBP_EVENT_MODULE_ERROR:
        timeStop();
        _isEnterSuccess = false;
        switchState(NIBP_SERVICE_ERROR_STATE);
        break;

    case NIBP_EVENT_TIMEOUT:
    {
        _isEnterSuccess = false;
        MessageBox messbox(trs("Warn"), trs("NIBPDirectiveTimeout"), false);
        messbox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        messbox.exec();
    }
    break;

    case NIBP_EVENT_SERVICE_REPAIR_RETURN:
    {
        NIBPMaintainMgrInterface *nibpMaintaiMgr;
        nibpMaintaiMgr = NIBPMaintainMgrInterface::getNIBPMaintainMgr();
        if (_isEnterSuccess && nibpMaintaiMgr && !nibpMaintaiMgr->getRepairError())
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
    case NIBP_EVENT_SERVICE_CALIBRATE_ENTER:
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

    case NIBP_EVENT_SERVICE_CALIBRATE_CMD_PRESSURE_POINT:
        setTimeOut();
        nibpParam.provider().servicePressurepoint(args, argLen);
        break;

    case NIBP_EVENT_SERVICE_CALIBRATE_RSP_PRESSURE_POINT:
        timeStop();
        nibpParam.setResult(args[0]);
    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPServiceCalibrateState::NIBPServiceCalibrateState() : NIBPState(NIBP_SERVICE_CALIBRATE_STATE)
{
    _isReturn = false;
    _isEnterSuccess = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPServiceCalibrateState::~NIBPServiceCalibrateState()
{
}
