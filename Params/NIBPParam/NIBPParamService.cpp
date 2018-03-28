#include "NIBPParamService.h"
#include "NIBPAlarm.h"
#include "IConfig.h"
#include "TimeDate.h"
#include "TimeManager.h"
#include "NIBPServiceCalibrate.h"
#include "NIBPServiceManometer.h"
#include "NIBPServicePressureControl.h"
#include "NIBPServiceRepair.h"
#include "NIBPServiceZeroPoint.h"

NIBPParamService *NIBPParamService::_selfObj = NULL;

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void NIBPParamService::initParam(void)
{
    if (NULL != _provider)
    {
        _provider->service_Enter(false);
    }
}

/**************************************************************************************************
 * 模块复位。
 *************************************************************************************************/
void NIBPParamService::reset(void)
{
    _isNIBPError = false;

    if (NULL != _provider)
    {
        _provider->sendSelfTest();
        _provider->service_Enter(false);
    }

    if(_currentServiceState != NULL)
    {
        _currentServiceState->connectError();
    }
//    serviceWindowManager.NIBPWarn();
}

void NIBPParamService::errorDisable()
{
    _isNIBPError = true;
//    serviceWindowManager.NIBPWarn();
}

/**************************************************************************************************
 * 解析包。
 *************************************************************************************************/
void NIBPParamService::unPacket(unsigned char *packet, int len)
{
    if (_currentServiceState == NULL)
    {
        return;
    }
    _currentServiceState->unPacket(packet, len);
}

/**************************************************************************************************
 * 状态机主运行。
 *************************************************************************************************/
void NIBPParamService::machineRun(void)
{
    if (_currentServiceState == NULL)
    {
        return;
    }
    _currentServiceState->run();
}

/**************************************************************************************************
 * 状态机切换。
 *************************************************************************************************/
void NIBPParamService::switchState(NIBPServiceStateType state)
{
    if (state == NIBP_Service_NULL)
    {
        _currentServiceState = NULL;
        return;
    }

    StateMap::iterator it = _servicestates.find(state);

    if (it == _servicestates.end())
    {
        return;
    }

    // 先退出之前的状态，载进入新状态。
    _currentServiceState = it.value();
    _currentServiceState->enter();
}

/**************************************************************************************************
 * 通信中断处理。
 *************************************************************************************************/
void NIBPParamService::disConnected(bool flag)
{
    if (flag)
    {
        if(_currentServiceState != NULL)
        {
            _currentServiceState->connectError();
        }
//        serviceWindowManager.NIBPWarn();
        connectWrongFlag = true;
    }
    else
    {
        connectWrongFlag = false;
    }
}

/**************************************************************************************************
 * 进入维护模式指令。
 *************************************************************************************************/
void NIBPParamService::serviceEnter(bool enter)
{
    if (_currentServiceState == NULL)
    {
        return;
    }
    if(enter)
    {
        _currentServiceState->triggerEnter();
    }
}

/**************************************************************************************************
 * 退出指令。
 *************************************************************************************************/
void NIBPParamService::serviceReturn()
{
    if (_currentServiceState == NULL)
    {
        return;
    }
    _currentServiceState->triggerReturn();

}

/**************************************************************************************************
 * 校准点。
 *************************************************************************************************/
void NIBPParamService::servicePressurepoint(int point, int value)
{
    if (_currentServiceState == NULL)
    {
        return;
    }
    _currentServiceState->calibratedPoint(point, value);

}

/**************************************************************************************************
 * 充气压力值指令。
 *************************************************************************************************/
void NIBPParamService::servicePressureinflate(int Value)
{
    if (_currentServiceState == NULL)
    {
        return;
    }
    _currentServiceState->servicePressureInflate(Value);
}

/**************************************************************************************************
 * 放气指令。
 *************************************************************************************************/
void NIBPParamService::servicePressuredeflate(void)
{
    if (_currentServiceState == NULL)
    {
        return;
    }
    _currentServiceState->servicePressureDeflate();
}

/**************************************************************************************************
 * 关闭气阀控制。
 *************************************************************************************************/
void NIBPParamService::serviceCloseValve(bool enter)
{
    if (_currentServiceState == NULL)
    {
        return;
    }
    _currentServiceState->serviceCloseValve(enter);
}

/**************************************************************************************************
 * 病人类型。
 *************************************************************************************************/
void NIBPParamService::servicePatientType(unsigned char type)
{
    if (_currentServiceState == NULL)
    {
        return;
    }
    _currentServiceState->servicePatientType(type);
}

/**************************************************************************************************
 * 校零。
 *************************************************************************************************/
void NIBPParamService::servicePressureZero(void)
{
    if (_currentServiceState == NULL)
    {
        return;
    }
    _currentServiceState->servicePressureZero();
}

/**************************************************************************************************
 * 过压保护。
 *************************************************************************************************/
void NIBPParamService::servicePressureProtect(bool enter)
{
    if (_currentServiceState == NULL)
    {
        return;
    }
    _currentServiceState->servicePressureProtect(enter);
}

/**************************************************************************************************
 * 设置服务模式数据提供对象。
 *************************************************************************************************/
void NIBPParamService::setServiceProvider(NIBPProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    _provider = provider;
    NIBPServiceState::setProvider(_provider);
    _provider->sendSelfTest();
}

/**************************************************************************************************
 * 单位设置。
 *************************************************************************************************/
UnitType NIBPParamService::getUnit(void)
{
    int unit = UNIT_MMHG;
    superConfig.getNumValue("Display|NIBPUnit", unit);
    return (UnitType)unit;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPParamService::NIBPParamService() : Param(PARAM_NIBP)
{
    connectWrongFlag = false;
    _isNIBPError = false;

    // 构造状态对象。
    NIBPServiceState *state;


    _provider = NULL;
    _currentServiceState = NULL;

    state = new NIBPServiceRepair();
//    _currentServiceState = state;
    _servicestates.insert(state->type(), state);

    state = new NIBPServiceCalibrate();
    _servicestates.insert(state->type(), state);

    state = new NIBPServiceManometer();
    _servicestates.insert(state->type(), state);

    state = new NIBPServicePressureControl();
    _servicestates.insert(state->type(), state);

    state = new NIBPServiceZeroPoint();
    _servicestates.insert(state->type(), state);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPParamService::~NIBPParamService()
{
    // 删除状态机。
    StateMap::iterator it = _servicestates.begin();
    for (; it != _servicestates.end(); ++it)
    {
        delete it.value();
    }
}

/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void NIBPParamService::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    if (NULL != _provider)
    {
        _provider->sendCmdData(cmdId,data,len);
    }
}

/**************************************************************************************************
 * 透传模式。
 *************************************************************************************************/
void NIBPParamService::setPassthroughMode(bool flag)
{
    _provider->setPassthroughMode(flag);
}


