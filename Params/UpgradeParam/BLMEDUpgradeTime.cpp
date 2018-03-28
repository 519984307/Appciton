#include "BLMEDUpgradeTime.h"
#include "BLMEDUpgradeParam.h"
#include "Debug.h"
#include <QTimer>

BLMEDUpgradeTime *BLMEDUpgradeTime::_selfObj = NULL;

/**************************************************************************************************
 * 进入升级模式超时调用。
 *************************************************************************************************/
void BLMEDUpgradeTime::_timeoutEnter(void)
{
    _isTimeout = true;
    _timerEnter->stop();
    blmedUpgradeParam.serviceUpgradeEnter();
}

/**************************************************************************************************
 * 文件片段超时调用。
 *************************************************************************************************/
void BLMEDUpgradeTime::_timeoutFile()
{
    _isTimeout = true;
    _timerFile->stop();
    blmedUpgradeParam.serviceUpgradeFile();
}

/**************************************************************************************************
 * 数据发送启动超时调用。
 *************************************************************************************************/
void BLMEDUpgradeTime::_timeoutSend()
{
    _timerSend->stop();
    blmedUpgradeParam.serviceUpgradeSend();
}

/**************************************************************************************************
 * 进入升级模式启动定时
 *************************************************************************************************/
void BLMEDUpgradeTime::startEnter(int sec)
{
    _timerEnter->start(sec * 100);
    _isTimeout = false;
}

/**************************************************************************************************
 * 文件片段启动定时
 *************************************************************************************************/
void BLMEDUpgradeTime::startFile(int sec)
{
    _timerFile->start(sec * 1000);
    _isTimeout = false;
}

/**************************************************************************************************
 * 数据发送启动定时
 *************************************************************************************************/
void BLMEDUpgradeTime::startSend(int sec)
{
    _timerSend->start(sec * 100);
    _isTimeout = false;
}

/**************************************************************************************************
 * 进入升级模式停止。
 *************************************************************************************************/
void BLMEDUpgradeTime::stopEnter(void)
{
    _timerEnter->stop();
    _isTimeout = false;
}

/**************************************************************************************************
 * 文件片段停止。
 *************************************************************************************************/
void BLMEDUpgradeTime::stopFile(void)
{
    _timerFile->stop();
}

/**************************************************************************************************
 * 数据发送启动停止。
 *************************************************************************************************/
void BLMEDUpgradeTime::stopSend()
{
    _timerSend->stop();
}

/**************************************************************************************************
 * 是否超时。
 *************************************************************************************************/
bool BLMEDUpgradeTime::isTimeout(void) const
{
    return _isTimeout;
}


/**************************************************************************************************
 * 状态机主运行。
 *************************************************************************************************/
BLMEDUpgradeTime::BLMEDUpgradeTime()
{
    _isTimeout = false;

    _timerEnter = new QTimer();
    connect(_timerEnter, SIGNAL(timeout()), this, SLOT(_timeoutEnter()));

    _timerFile = new QTimer();
    connect(_timerFile, SIGNAL(timeout()), this, SLOT(_timeoutFile()));

    _timerSend = new QTimer();
    connect(_timerSend, SIGNAL(timeout()), this, SLOT(_timeoutSend()));
}

/**************************************************************************************************
 * 状态机主运行。
 *************************************************************************************************/
BLMEDUpgradeTime::~BLMEDUpgradeTime()
{
    if (NULL != _timerEnter)
    {
        delete _timerEnter;
        _timerEnter = NULL;
    }

    if (NULL != _timerFile)
    {
        delete _timerFile;
        _timerFile = NULL;
    }

    if (NULL != _timerSend)
    {
        delete _timerSend;
        _timerSend = NULL;
    }
}
