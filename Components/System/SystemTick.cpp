#include "SystemTick.h"
#include <QTimer>
#include "Debug.h"
SystemTick *SystemTick::_selfObj = NULL;

/**************************************************************************************************
 * 功能： 开始运行。
 *************************************************************************************************/
void SystemTick::_timerOutSlot(void)
{
    struct timeval newTime;
    gettimeofday(&newTime, NULL);
    int interval = (newTime.tv_sec - _lastTime.tv_sec) * 1000 +
            (newTime.tv_usec - _lastTime.tv_usec) / 1000;
    _lastTime = newTime;

#ifdef USE_VECTOR
    for (int i = 0; i < _systemTickCtrl.size(); i++)
    {
        _systemTickCtrl[i].curMS += interval;
        if ((_systemTickCtrl[i].hook != NULL)
                && (_systemTickCtrl[i].curMS >= _systemTickCtrl[i].fixedMS))
        {
            _systemTickCtrl[i].curMS -= _systemTickCtrl[i].fixedMS;
            _systemTickCtrl[i].hook();
        }
    }
#else
    for(int i = 0; i < LEN ;i++)
    {
        _systemTickCtrl[i].curMS += interval;
        if ((_systemTickCtrl[i].hook != NULL)
                && (_systemTickCtrl[i].curMS >= _systemTickCtrl[i].fixedMS))
        {
            _systemTickCtrl[i].curMS -= _systemTickCtrl[i].fixedMS;
            _systemTickCtrl[i].hook();
        }
    }
#endif
}

/**************************************************************************************************
 * 功能：注册一个入口。
 * 参数：
 *      ms：毫秒。
 *      initCount: 初始计数值。
 *      hook：回调入口。
 *************************************************************************************************/
void SystemTick::addHook(int ms, int initCount, SystemTickHook hook)
{
#ifdef USE_VECTOR
    SystemTickCtrl ctrl(ms, initCount, hook);
    _systemTickCtrl.append(ctrl);
#else
    for(int i = 0; i < LEN ;i++)
    {
        if(_systemTickCtrl[i].hook == NULL)
        {
            _systemTickCtrl[i].fixedMS = ms;
            _systemTickCtrl[i].curMS = (initCount > ms) ? ms : initCount;
            _systemTickCtrl[i].hook = hook;
            break;
        }
    }
#endif
}

/**************************************************************************************************
 * 功能： 开始运行。
 *************************************************************************************************/
void SystemTick::startTick(void)
{
    gettimeofday(&_lastTime, NULL);
    _sysTimer->start(50);
}

/**************************************************************************************************
 * 功能： 将_lastTime设置到一百小时之前
 *************************************************************************************************/
void SystemTick::resetLastTime(void)
{
    gettimeofday(&_lastTime, NULL);
}

/**************************************************************************************************
 * 功能： 构造。
 *************************************************************************************************/
SystemTick::SystemTick()
        : QObject()
{
    _lastTime.tv_sec = 0;
    _lastTime.tv_usec = 0;
    _sysTimer = new QTimer(this);
    connect(_sysTimer, SIGNAL(timeout()), this, SLOT(_timerOutSlot()));
}

/**************************************************************************************************
 * 功能： 析构。
 *************************************************************************************************/
SystemTick::~SystemTick()
{
    _sysTimer->stop();
    delete _sysTimer;
}

