/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/3
 **/

#include "Alarm.h"
#include "IConfig.h"
#include <QMap>
#include "AlarmStateMachineInterface.h"
#include "ECGParamInterface.h"
#include "ECGSymbol.h"
#include "SPO2Symbol.h"
#include "RESPSymbol.h"
#include "CO2Symbol.h"
#include "IBPSymbol.h"
#include "COSymbol.h"
#include "AGSymbol.h"
#include "TEMPSymbol.h"
#include "NIBPSymbol.h"
#include "O2Symbol.h"
#include "TrendDataStorageManagerInterface.h"
#include "AlarmIndicatorInterface.h"
#include "TrendCacheInterface.h"
#include "ParamInfoInterface.h"

#define ALARM_LIMIT_TIMES (3)   // 超限3次后，发生报警
static int curSecondAlarmNum = 0;  // record the number of alarms happend in the save seconds

struct AlarmTraceCtrl
{
    AlarmTraceCtrl()
    {
        Reset();
    }

    void Reset(void)
    {
        type = ALARM_TYPE_PHY;
        priority = ALARM_PRIO_LOW;
        alarmMessage = NULL;
        lastAlarmed = false;
        isLatched = true;
        overHighLimit = false;
        normalTimesCount = 0;
        alarmTimesCount = 0;
        timestamp = 0;
        order = 0;      // record the order when alarms happened in the same seconds
    }

    AlarmType type;
    AlarmPriority priority;
    const char *alarmMessage;
    bool lastAlarmed;               //报警、拴锁报警
    bool isLatched;
    bool overHighLimit;
    char order;
    unsigned normalTimesCount;
    unsigned alarmTimesCount;
    unsigned timestamp;
};
typedef QMap<QString, AlarmTraceCtrl> TraceMap;
static TraceMap _traceCtrl;

/**************************************************************************************************
 * 功能：获取报警跟踪控制对象。
 * 参数：
 *      traceID：报警源ID。
 *************************************************************************************************/
static AlarmTraceCtrl &_getAlarmTraceCtrl(const QString traceID)
{
    TraceMap::iterator it = _traceCtrl.find(traceID);
    if (it != _traceCtrl.end())
    {
        return it.value();
    }

    _traceCtrl.insert(traceID, AlarmTraceCtrl());
    it = _traceCtrl.find(traceID);
    return it.value();
}

/**************************************************************************************************
 * 功能：获取报警源的ID，该ID用于跟踪报警状态。
 * 参数：
 *      ID: 报警源内部的ID号。
 *      traceID：返回报警源的跟踪ID。
 *************************************************************************************************/
void Alarm::_getAlarmID(AlarmParamIFace *alarmSource, int id, QString &traceID)
{
    // NOTE: this function is called too frequently, make a cache here
    typedef QPair<qint64, int> AlarmIDCacheKey;
    typedef QMap<AlarmIDCacheKey, QString> AlarmIDCacheType;
    static  AlarmIDCacheType alarmIdCache;


    AlarmIDCacheKey key((qint64)alarmSource, id);
    AlarmIDCacheType::iterator iter = alarmIdCache.find(key);
    if (iter == alarmIdCache.end())
    {
        traceID = alarmSource->getAlarmSourceName();
        traceID += QString::number(id);
        alarmIdCache.insert(key, traceID);
    }
    else
    {
        traceID = iter.value();
    }
}

/**************************************************************************************************
 * 功能：处理超限报警。
 *************************************************************************************************/
void Alarm::_handleLimitAlarm(AlarmLimitIFace *alarmSource, QList<ParamID> &alarmParam)
{
    int curValue = InvData();
    int n = alarmSource->getAlarmSourceNR();
    QString traceID;
    AlarmInfoSegment infoSegment;

    for (int i = 0; i < n; i++)
    {
        // 获取该参数报警的跟踪ID，再取得跟踪对象。
        _getAlarmID(alarmSource, i, traceID);
        AlarmTraceCtrl *traceCtrl = &_getAlarmTraceCtrl(traceID);
        AlarmPriority priority = alarmSource->getAlarmPriority(i);
        // 更新报警级别
        if (priority != traceCtrl->priority && NULL != traceCtrl->alarmMessage)
        {
            traceCtrl->priority = priority;
            AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
            if (alarmIndicator)
            {
                alarmIndicator->updataAlarmPriority(traceCtrl->type, traceCtrl->alarmMessage, priority);
            }
        }
        TrendCacheData data;
        TrendCacheInterface *trendCache = TrendCacheInterface::getTrendCache();
        if (trendCache)
        {
            trendCache->getTrendData(_timestamp, data);
        }
        curValue = data.values[alarmSource->getSubParamID(i)];

        bool isEnable = alarmSource->isAlarmEnable(i);
        int completeResult = alarmSource->getCompare(curValue, i);

        // 报警关闭
        if (!isEnable)
        {
            if (traceCtrl->lastAlarmed && _isLatchLock)
            {
                traceCtrl->normalTimesCount = ALARM_LIMIT_TIMES;
                AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
                if (alarmIndicator && alarmIndicator->latchAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage))
                {
                    alarmSource->notifyAlarm(i, true);
                    continue;
                }
            }
            AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
            if (alarmIndicator)
            {
                alarmIndicator->delAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage);      // 关闭了报警则取消该生理报警
            }
            traceCtrl->Reset();
            alarmSource->notifyAlarm(i, false);
            continue;
        }

        // 第一种情况：数据变为无效值或者报警恢复正常。
        if (curValue == InvData() || curValue == UnknownData() || (0 == completeResult))
        {
            if (_isLatchLock)
            {
                if (traceCtrl->lastAlarmed)
                {
                    if (traceCtrl->normalTimesCount >= ALARM_LIMIT_TIMES)
                    {
                        AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
                        if (alarmIndicator
                                && alarmIndicator->latchAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage))
                        {
                            alarmSource->notifyAlarm(i, true);
                            traceCtrl->isLatched = true;
                            continue;
                        }
                    }
                    else
                    {
                        ++traceCtrl->normalTimesCount;
                        alarmSource->notifyAlarm(i, true);
                        continue;
                    }
                }
                traceCtrl->Reset();
                alarmSource->notifyAlarm(i, false);
            }
            else
            {
                AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
                if (alarmIndicator)
                {
                    alarmIndicator->delAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage);
                }
                traceCtrl->Reset();
                alarmSource->notifyAlarm(i, false);
            }
            continue;
        }

        if (!traceCtrl->lastAlarmed)
        {
            //超限报警持续一定的时间才发生报警
            ++traceCtrl->alarmTimesCount;
            if (traceCtrl->alarmTimesCount < ALARM_LIMIT_TIMES)
            {
                alarmSource->notifyAlarm(i, false);
                continue;
            }

            traceCtrl->isLatched = false;

            alarmSource->notifyAlarm(i, true);
            if (completeResult < 0)  // 超低限。
            {
                traceCtrl->lastAlarmed = true;
                traceCtrl->overHighLimit = false;
                traceCtrl->type = ALARM_TYPE_PHY;
                traceCtrl->priority = alarmSource->getAlarmPriority(i);
                traceCtrl->alarmMessage = alarmSource->toString(i);
                traceCtrl->timestamp = _timestamp;
                traceCtrl->order = ++curSecondAlarmNum;
                AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
                if (alarmIndicator)
                {
                    alarmIndicator->addAlarmInfo(_timestamp, traceCtrl->type,
                                                traceCtrl->priority, traceCtrl->alarmMessage, alarmSource, i);
                }
                alarmParam.append(alarmSource->getParamID());
                // summaryStorageManager.addPhyAlarm(_timestamp, alarmSource->getParamID(), i,
                //        false, alarmSource->getWaveformID(i));
                infoSegment.alarmLimit = alarmSource->getLower(i);
                infoSegment.alarmInfo = 0;
            }
            else if (completeResult > 0)  // 超高限。
            {
                traceCtrl->lastAlarmed = true;
                traceCtrl->overHighLimit = true;
                traceCtrl->type = ALARM_TYPE_PHY;
                traceCtrl->priority = alarmSource->getAlarmPriority(i);
                traceCtrl->alarmMessage = alarmSource->toString(i);
                traceCtrl->timestamp = _timestamp;
                traceCtrl->order = ++curSecondAlarmNum;
                AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
                alarmIndicator->addAlarmInfo(_timestamp, traceCtrl->type,
                                            traceCtrl->priority, traceCtrl->alarmMessage, alarmSource, i);

                alarmParam.append(alarmSource->getParamID());
                // summaryStorageManager.addPhyAlarm(_timestamp, alarmSource->getParamID(), i,
                //        false, alarmSource->getWaveformID(i));
                infoSegment.alarmLimit = alarmSource->getUpper(i);
                infoSegment.alarmInfo = 0 | 1 << 1;
            }

            infoSegment.subParamID = alarmSource->getSubParamID(i);
            infoSegment.alarmType = i;

            EventInfo eventInfo;
            eventInfo.alarmSource = static_cast<AlarmParamIFace *>(alarmSource);
            eventInfo.infoSegment = infoSegment;
            eventInfo.waveId = i;
            eventList.append(eventInfo);

            switch (infoSegment.subParamID)
            {
            case SUB_PARAM_HR_PR:
            #ifndef HIDE_OXYCRG_REVIEW_FUNCTION
                eventStorageManager.triggerAlarmOxyCRGEvent(infoSegment, OxyCRGEventECG, _timestamp);
            #endif
                break;
            case SUB_PARAM_SPO2:
            #ifndef HIDE_OXYCRG_REVIEW_FUNCTION
                eventStorageManager.triggerAlarmOxyCRGEvent(infoSegment, OxyCRGEventSpO2, _timestamp);
            #endif
                break;
            case SUB_PARAM_RR_BR:
            #ifndef HIDE_OXYCRG_REVIEW_FUNCTION
                eventStorageManager.triggerAlarmOxyCRGEvent(infoSegment, OxyCRGEventResp, _timestamp);
            #endif
                break;
            default:
                break;
            }
            AlarmStateMachineInterface *alarmStateMachine = AlarmStateMachineInterface::getAlarmStateMachine();
            if (alarmStateMachine)
            {
                alarmStateMachine->handAlarmEvent(ALARM_STATE_EVENT_NEW_PHY_ALARM, 0, 0);
            }
        }  //栓锁的报警重新发生报警
        else
        {
            AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
            if (alarmIndicator && !alarmIndicator->checkAlarmIsExist(traceCtrl->type, traceCtrl->alarmMessage))
            {
                traceCtrl->Reset();
                ++traceCtrl->alarmTimesCount;
                alarmSource->notifyAlarm(i, false);
                continue;
            }

            traceCtrl->isLatched = false;
            alarmSource->notifyAlarm(i, true);
            if (traceCtrl->normalTimesCount >= ALARM_LIMIT_TIMES)
            {
                ++traceCtrl->alarmTimesCount;
                if (traceCtrl->alarmTimesCount < ALARM_LIMIT_TIMES)
                {
                    continue;
                }
                else
                {
                    if (alarmIndicator)
                    {
                        alarmIndicator->addAlarmInfo(_timestamp, traceCtrl->type,
                                                    traceCtrl->priority, traceCtrl->alarmMessage, alarmSource, i);
                    }
                }
            }
        }

        // 发生报警或者重新从栓锁到报警
        traceCtrl->alarmTimesCount = 0;
        traceCtrl->normalTimesCount = 0;
    }
}

/**************************************************************************************************
 * 功能：处理OneShot报警。
 *************************************************************************************************/
void Alarm::_handleOneShotAlarm(AlarmOneShotIFace *alarmSource)
{
    bool isEnable = false;
    int n = alarmSource->getAlarmSourceNR();
    QString traceID;

    for (int i = 0; i < n; i++)
    {
        // 获取该参数报警的跟踪ID，再取得跟踪对象。
        _getAlarmID(alarmSource, i, traceID);
        AlarmTraceCtrl *traceCtrl = &_getAlarmTraceCtrl(traceID);

        bool isAlarm = alarmSource->isAlarmed(i);
        AlarmType type = alarmSource->getAlarmType(i);
        AlarmPriority priority = alarmSource->getAlarmPriority(i);
        bool isRemoveAfterLatch = alarmSource->isRemoveAfterLatch(i);
        bool isRemoveLightAfterConfirm = alarmSource->isRemoveLightAfterConfirm(i);

        // 报警关闭不处理生理报警报警, don't handle phy alarm when in alarm pause state
        if ((_curAlarmStatus == ALARM_STATUS_OFF || _curAlarmStatus == ALARM_STATUS_PAUSE) && type != ALARM_TYPE_TECH)
        {
            alarmSource->notifyAlarm(i, false);
            traceCtrl->Reset();
            continue;
        }

        // 更新报警级别
        if (priority != traceCtrl->priority && NULL != traceCtrl->alarmMessage)
        {
            traceCtrl->priority = priority;
            AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
            if (alarmIndicator)
            {
                alarmIndicator->updataAlarmPriority(traceCtrl->type, traceCtrl->alarmMessage, priority);
            }
        }

        // 根据各自要求是否需要删除报警
        if (alarmSource->isNeedRemove(i))
        {
            if (traceCtrl->lastAlarmed)
            {
                AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
                if (alarmIndicator)
                {
                    alarmIndicator->delAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage);
                }
            }

            alarmSource->notifyAlarm(i, false);
            traceCtrl->Reset();
            continue;
        }

        // 报警使能关闭
        isEnable = alarmSource->isAlarmEnable(i);
        if (!isEnable)
        {
            if (traceCtrl->lastAlarmed)
            {
                if (traceCtrl->type != ALARM_TYPE_TECH && _isLatchLock)
                {
                    AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
                    if (alarmIndicator &&
                            alarmIndicator->latchAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage))
                    {
                        alarmSource->notifyAlarm(i, true);
                        continue;
                    }
                }
                else
                {
                    AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
                    if (alarmIndicator)
                    {
                        alarmIndicator->delAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage);
                    }
                }
            }

            traceCtrl->Reset();
            alarmSource->notifyAlarm(i, false);
            continue;
        }

        if (!isAlarm)
        {
            // 上次报警，现在恢复正常了。
            if (traceCtrl->lastAlarmed)
            {
                if (traceCtrl->type != ALARM_TYPE_TECH && _isLatchLock)
                {
                    // 栓锁打开时，才栓锁PhyOneShotAlarm
                    AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
                    if (alarmIndicator &&
                            !alarmIndicator->latchAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage))
                    {
                        alarmSource->notifyAlarm(i, false);
                        traceCtrl->Reset();
                    }
                    else
                    {
                        alarmSource->notifyAlarm(i, true);
                    }
                }
                else
                {
                    AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
                    if (alarmIndicator)
                    {
                        alarmIndicator->delAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage);
                    }
                    traceCtrl->Reset();
                }
            }
            else
            {
                alarmSource->notifyAlarm(i, false);
            }

            continue;
        }
        else
        {
            if (traceCtrl->lastAlarmed)
            {
                // 生命报警从栓锁恢复到继续报警。
                if (traceCtrl->type != ALARM_TYPE_TECH && _isLatchLock)
                {
                    AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
                    if (alarmIndicator &&
                            alarmIndicator->checkAlarmIsExist(traceCtrl->type, traceCtrl->alarmMessage))
                    {
                        alarmIndicator->updateLatchAlarmInfo(traceCtrl->alarmMessage, false);
                        alarmSource->notifyAlarm(i, true);
                        continue;
                    }
                    else
                    {
                        traceCtrl->Reset();
                    }
                }
                else
                {
                    continue;
                }
            }
        }

        // 更新跟踪信息。
        traceCtrl->lastAlarmed = isAlarm;
        traceCtrl->type = type;
        traceCtrl->alarmMessage = alarmSource->toString(i);
        traceCtrl->priority = priority;
        traceCtrl->timestamp = _timestamp;
        traceCtrl->order = ++curSecondAlarmNum;

        // 发布该报警。
        AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
        if (alarmIndicator)
        {
            alarmIndicator->addAlarmInfo(_timestamp, traceCtrl->type,
                                        traceCtrl->priority, traceCtrl->alarmMessage, alarmSource, i, isRemoveAfterLatch
                                        , isRemoveLightAfterConfirm);
        }

        if (traceCtrl->type == ALARM_TYPE_LIFE || traceCtrl->type == ALARM_TYPE_PHY)
        {
            // one shot事件储存
            AlarmInfoSegment infoSegment;
            infoSegment.subParamID = alarmSource->getSubParamID(i);
            infoSegment.alarmInfo = 1;  // one shot 报警
            infoSegment.alarmType = i;
            EventStorageManagerInterface *eventStorageManager = EventStorageManagerInterface::getEventStorageManager();
            if (eventStorageManager)
            {
                eventStorageManager->triggerAlarmEvent(infoSegment, alarmSource->getWaveformID(i), _timestamp);
            }
            else
            {
                qDebug() << Q_FUNC_INFO << "get event storage manager faild.";
            }
            alarmSource->notifyAlarm(i, true);
            AlarmStateMachineInterface *alarmStateMachine = AlarmStateMachineInterface::getAlarmStateMachine();
            if (alarmStateMachine)
            {
                alarmStateMachine->handAlarmEvent(ALARM_STATE_EVENT_NEW_PHY_ALARM, 0, 0);
            }
        }
        else
        {
            AlarmStateMachineInterface *alarmStateMachine = AlarmStateMachineInterface::getAlarmStateMachine();
            if (alarmStateMachine)
            {
                alarmStateMachine->handAlarmEvent(ALARM_STATE_EVENT_NEW_PHY_ALARM, 0, 0);
            }
        }
    }
}

/**************************************************************************************************
 * 功能：处理报警。
 *************************************************************************************************/
void Alarm::_handleAlarm(void)
{
    // 处理生理超限报警
    QList<AlarmLimitIFace *> limitAlarmSourceList = _limitSources.values();
    QList<ParamID> alarmParamID;
    QList<ParamID> paramID;
    paramID.clear();
    eventList.clear();
    foreach(AlarmLimitIFace *source, limitAlarmSourceList)
    {
        alarmParamID.clear();
        _handleLimitAlarm(source, alarmParamID);
        if (!alarmParamID.isEmpty())
        {
            paramID.append(alarmParamID);
        }
    }

    // 统一触发之前产生的事件，目的为了触发的事件是当前时刻全部事件
    EventStorageManagerInterface *eventStorageManager = EventStorageManagerInterface::getEventStorageManager();
    if (eventStorageManager)
    {
        for (int i = 0; i < eventList.count(); i++)
        {
            AlarmParamIFace *alarmSource = eventList.at(i).alarmSource;
            int id = eventList.at(i).waveId;
            eventStorageManager->triggerAlarmEvent(eventList.at(i).infoSegment,
                                                   alarmSource->getWaveformID(id),
                                                   _timestamp);
        }
    }

    // 合并同一时刻的所有生理超限报警
    if (-1 != paramID.indexOf(PARAM_NIBP))
    {
        // nibp alarm add after check sys, dia, map
        TrendDataStorageManagerInterface *trendDataStorageManager = TrendDataStorageManagerInterface::getTrendDataStorageManager();
        TrendCacheInterface *trendCache = TrendCacheInterface::getTrendCache();
        if (trendDataStorageManager && trendCache)
        {
            trendCache->collectTrendData(_timestamp);
            trendCache->collectTrendAlarmStatus(_timestamp);
            trendDataStorageManager->storeData(_timestamp, TrendDataStorageManagerInterface::CollectStatusAlarm);
        }
        paramID.removeAll(PARAM_NIBP);
    }

    if (!paramID.isEmpty())
    {
        TrendDataStorageManagerInterface *trendDataStorageManager = TrendDataStorageManagerInterface::getTrendDataStorageManager();
        TrendCacheInterface *trendCache = TrendCacheInterface::getTrendCache();
        if (trendDataStorageManager && trendCache)
        {
            trendCache->collectTrendData(_timestamp);
            trendCache->collectTrendAlarmStatus(_timestamp);
            trendDataStorageManager->storeData(_timestamp, TrendDataStorageManagerInterface::CollectStatusAlarm);
        }
    }

    // 处理生理、技术和生命报警。
    QList<AlarmOneShotIFace *> oneshotSourceList = _oneshotSources.values();
    foreach(AlarmOneShotIFace *source, oneshotSourceList)
    {
        _handleOneShotAlarm(source);
    }
}

/**************************************************************************************************
 * 功能：注册一个报警源。
 * 参数：
 *      alarmSource：报警源
 *************************************************************************************************/
Alarm &Alarm::getInstance()
{
    static Alarm *instance = NULL;

    if (instance == NULL)
    {
        instance = new Alarm();
        AlarmInterface *old = registerAlarm(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

void Alarm::clear()
{
    // 删除报警源。
    QList<AlarmLimitIFace *> limitAlarmSourceList = _limitSources.values();
    foreach(AlarmLimitIFace *source, limitAlarmSourceList)
    {
        delete source;
    }

    _limitSources.clear();

    QList<AlarmOneShotIFace *> oneshotSourceList = _oneshotSources.values();
    foreach(AlarmOneShotIFace *source, oneshotSourceList)
    {
        delete source;
    }
    _oneshotSources.clear();
    _alarmStatusList.clear();
}

void Alarm::addLimtSource(AlarmLimitIFace *alarmSource)
{
    _limitSources.insert(alarmSource->getParamID(), alarmSource);
}

/**************************************************************************************************
 * 功能：注册一个报警源。
 * 参数：
 *      alarmSource：报警源
 *************************************************************************************************/
void Alarm::addOneShotSource(AlarmOneShotIFace *alarmSource)
{
    _oneshotSources.insert(alarmSource->getParamID(), alarmSource);
}

/**************************************************************************************************
 * 功能：设置静音键状态。
 * 参数：
 *      isPressed： 是否为按下。
 *************************************************************************************************/
void Alarm::updateMuteKeyStatus(bool isPressed)
{
    AlarmStateMachineInterface *alarmStateMachine = AlarmStateMachineInterface::getAlarmStateMachine();
    if (isPressed)
    {
        if (alarmStateMachine)
        {
            alarmStateMachine->handAlarmEvent(ALARM_STATE_EVENT_MUTE_BTN_PRESSED, 0, 0);
        }
    }
    else
    {
        if (alarmStateMachine)
        {
            alarmStateMachine->handAlarmEvent(ALARM_STATE_EVENT_MUTE_BTN_RELEASED, 0, 0);
        }
    }
}

void Alarm::updateResetKeyStatus(bool isPressed)
{
    AlarmStateMachineInterface *alarmStateMachine = AlarmStateMachineInterface::getAlarmStateMachine();
    if (isPressed)
    {
        if (alarmStateMachine)
        {
            alarmStateMachine->handAlarmEvent(ALARM_STATE_EVENT_RESET_BTN_PRESSED, 0, 0);
        }
    }
    else
    {
        if (alarmStateMachine)
        {
            alarmStateMachine->handAlarmEvent(ALARM_STATE_EVENT_RESET_BTN_RELEASED, 0, 0);
        }
    }
}

/**************************************************************************************************
 * 功能： 获取报警源参数的值。
 * 参数：
 *      data：带回数据；
 *      len：data的长度。
 *************************************************************************************************/
void Alarm::getAlarmSourceValue(qint32 *data, int len)
{
    QList<AlarmLimitIFace *> limitAlarmSourceList = _limitSources.values();
    foreach(AlarmLimitIFace *source, limitAlarmSourceList)
    {
        int nr = source->getAlarmSourceNR();
        for (int j = 0; j < nr; j++)
        {
            int index = source->getSubParamID(j);
            if (index < len)
            {
                data[index] = source->getValue(j);
            }
        }
    }
}

/**************************************************************************************************
 * 功能： 获取报警源参数的报警状态,超限报警存在栓锁此接口仅能判断是否发生报警。
 * 参数：
 *      sourceName:报警源名称
 *      id：子参数的ID。
 * 		ignoreLatched: ignore latch alarm status
 *      return value: 0, No alarm ; 1, low limit alarm; 2, high limit alarm
 *************************************************************************************************/
char Alarm::getAlarmSourceStatus(const QString &sourceName, SubParamID id, bool ignoreLatched)
{
    unsigned char flag = 0;
    int i = 0;
    AlarmLimitIFace *source = NULL;
    QList<AlarmLimitIFace *> limitAlarmSourceList = _limitSources.values();
    for (; i < limitAlarmSourceList.size(); i++)
    {
        if (sourceName == limitAlarmSourceList[i]->getAlarmSourceName())
        {
            source = limitAlarmSourceList[i];
            break;
        }
    }

    i = 0;
    if (NULL != source)
    {
        int n = source->getAlarmSourceNR();
        for (; i < n; ++i)
        {
            if (id == source->getSubParamID(i))
            {
                QString traceID;

                _getAlarmID(source, i, traceID);
                AlarmTraceCtrl *traceCtrl = &_getAlarmTraceCtrl(traceID);

                if (traceCtrl->lastAlarmed)
                {
                    flag = traceCtrl->overHighLimit ? HighLimitAlarm : LowLimitAlarm;
                    if (ignoreLatched)
                    {
                        if (traceCtrl->isLatched)
                        {
                            flag = NotLimitAlarm;
                        }
                    }
                    break;
                }
            }
        }
    }

    return flag;
}

/**************************************************************************************************
 * 功能： 主运行，1妙运行一次。
 *************************************************************************************************/
void Alarm::mainRun(unsigned t)
{
    if (!_alarmStatusList.isEmpty())
    {
        _curAlarmStatus = _alarmStatusList.takeFirst();
    }

    curSecondAlarmNum = 0;
    _timestamp = t;
    _handleAlarm();
    AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
    if (alarmIndicator)
    {
        alarmIndicator->publishAlarm(_curAlarmStatus);
    }
}

/**************************************************************************************************
 * 功能： 添加报警状态。
 *************************************************************************************************/
void Alarm::addAlarmStatus(AlarmStatus status)
{
    while (!_alarmStatusList.isEmpty())
    {
        int index = _alarmStatusList.indexOf(status);
        if (-1 != index)
        {
            _alarmStatusList.removeAt(index);
        }
        else
        {
            break;
        }
    }

    _alarmStatusList.append(status);
}

int Alarm::getAlarmStatusCount()
{
    return _alarmStatusList.count();
}

/***************************************************************************************************
 * getAlarmMessage : get alarm message from the alarm source
 **************************************************************************************************/
const char *Alarm::getAlarmMessage(ParamID paramId, int alarmType, bool isOneshotAlarm)
{
    if (isOneshotAlarm)
    {
        QList<AlarmOneShotIFace *> source = _oneshotSources.values(paramId);
        if (source.size() > 1 || source.size() == 0)
        {
            // multiple sources for paramid or no source;
            debug("Unable to get oneshot alarm message for param:%d\n", paramId);
            return NULL;
        }
        return source.first()->toString(alarmType);
    }
    else
    {
        QList<AlarmLimitIFace *> source = _limitSources.values(paramId);
        if (source.size() > 1 || source.size() == 0)
        {
            // multiple sources for paramid or no source;
            debug("Unable to get limit alarm message for param: %d\n", paramId);
            return NULL;
        }
        return source.first()->toString(alarmType);
    }
}

static bool alarmInfoLessThan(Alarm::AlarmInfo info1, Alarm::AlarmInfo info2)
{
    if (info1.timestamp == info2.timestamp)
    {
        return info1.order < info2.order;
    }
    else
    {
        return info1.timestamp < info2.timestamp;
    }
}

/***************************************************************************************************
 * getCurrentAlarmInfo : get the current alarm info
 **************************************************************************************************/
QList<Alarm::AlarmInfo> Alarm::getCurrentPhyAlarmInfo()
{
    QList<Alarm::AlarmInfo> alarmInfoList;
    Alarm::AlarmInfo almInfo;
    QList<AlarmLimitIFace *> limitAlarmSourceList = _limitSources.values();

    foreach(AlarmLimitIFace *source, limitAlarmSourceList)
    {
        int  n = source->getAlarmSourceNR();
        for (int i = 0; i < n; i++)
        {
            QString traceID;

            _getAlarmID(source, i, traceID);
           AlarmTraceCtrl *traceCtrl = &_getAlarmTraceCtrl(traceID);
            if (traceCtrl->lastAlarmed)
            {
                almInfo.paramid = source->getParamID();
                almInfo.isOneshot = false;
                almInfo.alarmType = i;
                almInfo.timestamp = traceCtrl->timestamp;
                almInfo.order = traceCtrl->order;
                alarmInfoList.append(almInfo);
            }
        }
    }

    QList<AlarmOneShotIFace *> oneshotAlarmSouceList = _oneshotSources.values();
    foreach(AlarmOneShotIFace *source, oneshotAlarmSouceList)
    {
        int n = source->getAlarmSourceNR();
        for (int i = 0; i < n; i++)
        {
            if (source->getAlarmType(i) == ALARM_TYPE_PHY
                    || source->getAlarmType(i) == ALARM_TYPE_LIFE)
            {
                QString traceID;

                _getAlarmID(source, i, traceID);
                 AlarmTraceCtrl *traceCtrl = &_getAlarmTraceCtrl(traceID);
                if (traceCtrl->lastAlarmed)
                {
                    almInfo.paramid = source->getParamID();
                    almInfo.isOneshot = true;
                    almInfo.alarmType = i;
                    almInfo.timestamp = traceCtrl->timestamp;
                    almInfo.order = traceCtrl->order;
                    alarmInfoList.append(almInfo);
                }
            }
        }
    }

    qSort(alarmInfoList.begin(), alarmInfoList.end(), alarmInfoLessThan);

    return alarmInfoList;
}


/***************************************************************************************************
 *  check specific oneshot alarm is active
 **************************************************************************************************/
bool Alarm::getOneShotAlarmStatus(AlarmOneShotIFace *iface, int alarmId)
{
    QList<AlarmOneShotIFace *> oneshotAlarmSouceList = _oneshotSources.values();
    if (oneshotAlarmSouceList.contains(iface))
    {
        QString traceID;
        _getAlarmID(iface, alarmId, traceID);
         AlarmTraceCtrl *traceCtrl = &_getAlarmTraceCtrl(traceID);
        return traceCtrl->lastAlarmed;
    }
    else
    {
        return false;
    }
}

/**************************************************************************************************
 * according to SubParamID get alarmLimitIFace
 *************************************************************************************************/
AlarmLimitIFace *Alarm::getAlarmLimitIFace(SubParamID id)
{
    ParamInfoInterface *paramInfo = ParamInfoInterface::getParamInfo();
    if (!paramInfo)
    {
        return NULL;
    }
    ParamID paramId = paramInfo->getParamID(id);
    if (_limitSources.end() != _limitSources.find(paramId))
    {
        return _limitSources.find(paramId).value();
    }
    return NULL;
}

AlarmOneShotIFace *Alarm::getAlarmOneShotIFace(SubParamID id)
{
    ParamInfoInterface *paramInfo = ParamInfoInterface::getParamInfo();
    if (!paramInfo)
    {
        return NULL;
    }
    ParamID paramId = paramInfo->getParamID(id);
    if (paramId == PARAM_DUP_ECG)
    {
        paramId = PARAM_ECG;
    }
    else if (paramId == PARAM_DUP_RESP)
    {
        paramId = PARAM_RESP;
    }
    if (_oneshotSources.end() != _oneshotSources.find(paramId))
    {
        return _oneshotSources.find(paramId).value();
    }
    return NULL;
}

/**************************************************************************************************
 * 功能： 构造。
 *************************************************************************************************/
Alarm::Alarm() :
    AlarmInterface(),
    _isLatchLock(true), _alarmLightOnAlarmReset(false)
{
    // 栓锁状态初始化
    int boltLockIndex = 0;
    systemConfig.getNumValue("Alarms|PhyParAlarmLatchlockOn", boltLockIndex);
    if (boltLockIndex)
    {
        _isLatchLock = true;
    }
    else
    {
        _isLatchLock = false;
    }
    _alarmStatusList.clear();
    _curAlarmStatus = ALARM_STATUS_NORMAL;
}

/**************************************************************************************************
 * 功能： 析构。
 *************************************************************************************************/
Alarm::~Alarm()
{
    // 删除报警源。
    QList<AlarmLimitIFace *> limitAlarmSourceList = _limitSources.values();
    foreach(AlarmLimitIFace *source, limitAlarmSourceList)
    {
        delete source;
    }

    _limitSources.clear();

    QList<AlarmOneShotIFace *> oneshotSourceList = _oneshotSources.values();
    foreach(AlarmOneShotIFace *source, oneshotSourceList)
    {
        delete source;
    }
    _oneshotSources.clear();
    _alarmStatusList.clear();
}

QString Alarm::getPhyAlarmMessage(ParamID paramId, SubParamID subId, int alarmType, bool isOneShot)
{
    switch (paramId)
    {
    case PARAM_ECG:
        if (isOneShot)
        {
            if (alarmType < static_cast<int>(ECG_ONESHOT_NR))
            {
                ECGParamInterface *ecgParam = ECGParamInterface::getECGParam();
                if (ecgParam)
                {
                    return ECGSymbol::convert((ECGOneShotType)alarmType, ecgParam->getLeadConvention());
                }
                else
                {
                    return ECGSymbol::convert(static_cast<ECGOneShotType>(alarmType), ECG_CONVENTION_AAMI);
                }
            }
        }
        else
        {
            // no limit alarm
        }
        break;
    case PARAM_DUP_ECG:
        if (isOneShot)
        {
            // no oneshot alarm
        }
        else
        {
            if (alarmType < static_cast<int>(ECG_DUP_LIMIT_ALARM_NR))
            {
                return ECGSymbol::convert((ECGDupLimitAlarmType)alarmType);
            }
        }
        break;
    case PARAM_SPO2:
        if (isOneShot)
        {
            if (alarmType < static_cast<int>(SPO2_ONESHOT_NR))
            {
                if (subId == SUB_PARAM_SPO2_2)
                {
                    return SPO2Symbol::convert((SPO2OneShotType)alarmType, true);
                }
                else
                {
                    return SPO2Symbol::convert((SPO2OneShotType)alarmType, false);
                }
            }
        }
        else
        {
            if (alarmType < static_cast<int>(SPO2_LIMIT_ALARM_NR))
            {
                return SPO2Symbol::convert((SPO2LimitAlarmType)alarmType);
            }
        }
        break;
    case PARAM_RESP:
        if (isOneShot)
        {
            if (alarmType < static_cast<int>(RESP_ONESHOT_NR))
            {
                return RESPSymbol::convert((RESPOneShotType)alarmType);
            }
        }
        else
        {
            // no limit alarm
        }
        break;
    case PARAM_DUP_RESP:
        if (isOneShot)
        {
            // no oneshot alarm
        }
        else
        {
            if (alarmType < static_cast<int>(RESP_DUP_LIMIT_ALARM_NR))
            {
                return RESPSymbol::convert((RESPDupLimitAlarmType)alarmType);
            }
        }
        break;
    case PARAM_NIBP:
        if (isOneShot)
        {
            if (alarmType < static_cast<int>(NIBP_ONESHOT_NR))
            {
                return NIBPSymbol::convert((NIBPOneShotType)alarmType);
            }
        }
        else
        {
            if (alarmType < static_cast<int>(NIBP_LIMIT_ALARM_NR))
            {
                return NIBPSymbol::convert((NIBPLimitAlarmType)alarmType);
            }
        }
        break;
    case PARAM_CO2:
        if (isOneShot)
        {
            if (alarmType < static_cast<int>(CO2_ONESHOT_NR))
            {
                return CO2Symbol::convert((CO2OneShotType)alarmType);
            }
        }
        else
        {
            if (alarmType < static_cast<int>(CO2_LIMIT_ALARM_NR))
            {
                return CO2Symbol::convert((CO2LimitAlarmType)alarmType);
            }
        }
        break;
    case PARAM_TEMP:
        if (isOneShot)
        {
            if (alarmType < static_cast<int>(TEMP_ONESHOT_NR))
            {
                return TEMPSymbol::convert((TEMPOneShotType)alarmType);
            }
        }
        else
        {
            if (alarmType < static_cast<int>(TEMP_LIMIT_ALARM_NR))
            {
                return TEMPSymbol::convert((TEMPLimitAlarmType)alarmType);
            }
        }
        break;
    case PARAM_CO:
        if (isOneShot)
        {
            if (alarmType < static_cast<int>(CO_ONESHOT_NR))
            {
                return COSymbol::convert((COOneShotType)alarmType);
            }
        }
        else
        {
            if (alarmType < static_cast<int>(CO_LIMIT_ALARM_NR))
            {
                return COSymbol::convert((COLimitAlarmType)alarmType);
            }
        }
        break;
    case PARAM_IBP:
        if (isOneShot)
        {
            if (alarmType < static_cast<int>(IBP_ONESHOT_NR))
            {
                return IBPSymbol::convert((IBPOneShotType)alarmType);
            }
        }
        else
        {
            if (alarmType < static_cast<int>(IBP_LIMIT_ALARM_NR))
            {
                return IBPSymbol::convert((IBPLimitAlarmType)alarmType);
            }
        }
        break;
    case PARAM_AG:
        if (isOneShot)
        {
            if (alarmType < static_cast<int>(AG_ONESHOT_NR))
            {
                return AGSymbol::convert((AGOneShotType)alarmType);
            }
        }
        else
        {
            if (alarmType < static_cast<int>(AG_LIMIT_ALARM_NR))
            {
                return AGSymbol::convert((AGLimitAlarmType)alarmType);
            }
        }
        break;
    case PARAM_O2:
        if (isOneShot)
        {
            if (alarmType < static_cast<int>(O2_ONESHOT_ALARM_NR))
            {
                return O2Symbol::convert((O2OneShotAlarmType)alarmType);
            }
        }
        else
        {
            if (alarmType < static_cast<int>(O2_LIMIT_ALARM_NR))
            {
                return O2Symbol::convert((O2LimitAlarmType)alarmType);
            }
        }
        break;
    default:
        break;
    }
    return "";
}

void Alarm::setLatchLockSta(bool status)
{
    _isLatchLock = status;
}

void Alarm::removeAllLimitAlarm()
{
    QList<AlarmLimitIFace *> limitAlarmSourceList = _limitSources.values();
    foreach(AlarmLimitIFace *source, limitAlarmSourceList)
    {
        int n = source->getAlarmSourceNR();
        for (int i = 0; i < n; i++)
        {
            QString traceID;
            _getAlarmID(source, i, traceID);
            _traceCtrl.remove(traceID);
            source->notifyAlarm(i, false);
        }
    }
}

void Alarm::setAlarmLightOnAlarmReset(bool flag)
{
    int index = flag ? 1 : 0;
    systemConfig.setNumValue("Alarms|AlarmLightOnAlarmReset", index);
}

bool Alarm::getAlarmLightOnAlarmReset()
{
    int index = 0;
    systemConfig.getNumValue("Alarms|AlarmLightOnAlarmReset", index);
    return index;
}
