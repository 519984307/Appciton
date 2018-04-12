#include "AlarmIndicator.h"
#include "AlarmMuteBarWidget.h"
#include "AlarmPhyInfoBarWidget.h"
#include "AlarmTechInfoBarWidget.h"
#include "SoundManager.h"
#include "LightManager.h"
#include "SystemManager.h"
#include "IConfig.h"
#include "AlarmStateMachine.h"
#include "Alarm.h"
#include "ECGAlarm.h"

AlarmIndicator *AlarmIndicator::_selfObj = NULL;

/**************************************************************************************************
 * 功能：判断释放存在报警。
 *************************************************************************************************/
bool AlarmIndicator::_existAlarm(void)
{
    if (_alarmInfoDisplayPool.empty())
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 * 功能：发布报警。
 *************************************************************************************************/
void AlarmIndicator::publishAlarm(AlarmAudioStatus status)
{
    if (_alarmInfoDisplayPool.isEmpty())
    {
        _displayPhyClear();
        _displayTechClear();
        soundManager.updateAlarm(false, ALARM_PRIO_LOW);
        lightManager.updateAlarm(false, ALARM_PRIO_LOW);
        return;
    }

    // 报警暂停处理。
    int newPhyAlarmIndex = -1;          // 记录新的报警，优先显示
    int oldPhyAlarmIndex = -1;          // 记录最久的被抢占显示的报警
    int newTechAlarmIndex = -1;         // 记录新的技术报警索引
    int oldTechAlarmIndex = -1;         // 记录被抢占的技术报警索引
    int lastPhyIndex = -1;             // 记录当前显示索引后面的技术报警索引
    int firstPhyIndex = -1;            // 记录第一个技术报警索引
    int lastTechIndex = -1;             // 记录当前显示索引后面的技术报警索引
    int firstTechIndex = -1;            // 记录第一个技术报警索引
    int index = 0;
    int phyAlarmNum = getAlarmCount(ALARM_TYPE_PHY);
    int techAlarmNum = getAlarmCount(ALARM_TYPE_TECH);
    bool hasNonLatchedPhyAlarm = false;
    bool hasPausedPhyAlarm = false;
    bool hasNewPromptTechAlarm = false;
    AlarmPriority phySoundPriority = ALARM_PRIO_PROMPT;
    AlarmPriority techSoundPriority = ALARM_PRIO_PROMPT;
    AlarmPriority lightPriority = ALARM_PRIO_PROMPT;
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        AlarmInfoNode node = *it;

        if (ALARM_OFF != status && ALARM_TYPE_TECH != node.alarmType)
        {
            if (0 < it->pauseTime)
            {
                --node.pauseTime;
                *it = node;
            }

            // pause/audio off状态没有生理报警声音
            if (0 == node.pauseTime && status != ALARM_AUDIO_OFF)
            {
                if (phySoundPriority < node.alarmPriority)
                {
                    phySoundPriority = node.alarmPriority;
                }
            }

            if (lightPriority < node.alarmPriority)
            {
                lightPriority = node.alarmPriority;
            }

            if (!node.latch)
            {
                hasNonLatchedPhyAlarm = true;
            }

            if (0 != node.pauseTime)
            {
                hasPausedPhyAlarm = true;
            }
        }
        else if (ALARM_TYPE_TECH == node.alarmType)
        {
            //技术报警只处理没有被acknowledge
            if ((!node.acknowledge || node.latch) && node.alarmPriority != ALARM_PRIO_PROMPT)
            {
                if (techSoundPriority < node.alarmPriority)
                {
                    techSoundPriority = node.alarmPriority;
                }

                if (lightPriority < node.alarmPriority)
                {
                    lightPriority = node.alarmPriority;
                }
            }
            else if (node.alarmPriority == ALARM_PRIO_PROMPT)
            {
                if (!node.promptAlarmBeep)
                {
                    node.promptAlarmBeep = true;
                    *it = node;
                    hasNewPromptTechAlarm = true;
                }
            }
        }

        // 记录最新的报警
        if (3 == node.displayTime)
        {
            if ((ALARM_TYPE_PHY == node.alarmType) && (-1 == newPhyAlarmIndex))
            {
                newPhyAlarmIndex = index;
            }

            else if ((ALARM_TYPE_TECH == node.alarmType) && (-1 == newTechAlarmIndex))
            {
                newTechAlarmIndex = index;
            }
        }
        else if (0 != node.displayTime)
        {
            if (ALARM_TYPE_PHY == node.alarmType)
            {
                oldPhyAlarmIndex = index;
            }
            else if (ALARM_TYPE_TECH == node.alarmType)
            {
                oldTechAlarmIndex = index;
            }
        }

        //索引第一条生理报警与后一条生理报警
        if (ALARM_TYPE_PHY == node.alarmType)
        {
            if (-1 == firstPhyIndex)
            {
                firstPhyIndex = index;
            }

            if ((index > _alarmPhyDisplayIndex) && (-1 == lastPhyIndex))
            {
                lastPhyIndex = index;
            }
        }

        //索引第一条技术报警与后一条技术报警
        if (ALARM_TYPE_TECH == node.alarmType)
        {
            if (-1 == firstTechIndex)
            {
                firstTechIndex = index;
            }

            if ((index > _alarmTechDisplayIndex) && (-1 == lastTechIndex))
            {
                lastTechIndex = index;
            }
        }

        ++index;
    }

    // 更新声音
    if (phySoundPriority != ALARM_PRIO_PROMPT)
    {
        soundManager.updateAlarm(true, phySoundPriority);
    }
    else if (techSoundPriority != ALARM_PRIO_PROMPT)
    {
        soundManager.updateAlarm(true, techSoundPriority);
    }
    else
    {
        soundManager.updateAlarm(false, phySoundPriority);
    }

    //nor-alert beeps in non-aed modes
    if (hasNewPromptTechAlarm && _enableNonAlarmBeepsInNonAED)
    {
        soundManager.errorTone();
    }

    // 更新灯光
    if (lightPriority != ALARM_PRIO_PROMPT)
    {
        lightManager.updateAlarm(true, lightPriority);
    }
    else
    {
        lightManager.updateAlarm(false, lightPriority);
    }

    //生理报警
    if (phyAlarmNum > 0)
    {
        if (_alarmPhyDisplayIndex >= _alarmInfoDisplayPool.count())
        {
            _alarmPhyDisplayIndex = firstPhyIndex;
        }
        AlarmInfoNode alarmPhyNode;

        _displayInfoNode(alarmPhyNode,_alarmPhyDisplayIndex,
                         newPhyAlarmIndex,oldPhyAlarmIndex,firstPhyIndex,lastPhyIndex);
        if (NULL != alarmPhyNode.alarmMessage)
        {
            _displayPhySet(alarmPhyNode);
        }
    }
    else
    {
        _displayPhyClear();
    }

    //技术报警
    if (techAlarmNum > 0)
    {
        if (_alarmTechDisplayIndex >= _alarmInfoDisplayPool.count())
        {
            _alarmTechDisplayIndex = firstTechIndex;
        }
        AlarmInfoNode alarmTechNode;
        _displayInfoNode(alarmTechNode,_alarmTechDisplayIndex,
                         newTechAlarmIndex,oldTechAlarmIndex,firstTechIndex,lastTechIndex);

        if (NULL != alarmTechNode.alarmMessage)
        {
            _displayTechSet(alarmTechNode);
        }
    }
    else
    {
        _displayTechClear();
    }

    // 生理报警条件全部解除
    if (!hasNonLatchedPhyAlarm)
    {
        alarmStateMachine.handAlarmEvent(ALARM_STATE_EVENT_ALL_PHY_ALARM_LATCHED, 0, 0);
    }

    // 没有处于倒计时的报警
    if (!hasPausedPhyAlarm)
    {
        alarmStateMachine.handAlarmEvent(ALARM_STATE_EVENT_NO_PAUSED_PHY_ALARM, 0, 0);
    }
}

/**************************************************************************************************
 * 清除生理报警界面。
 *************************************************************************************************/
void AlarmIndicator::_displayPhyClear(void)
{
    _alarmPhyInfoWidget->clear();
}

/**************************************************************************************************
 * 清除技术报警界面。
 *************************************************************************************************/
void AlarmIndicator::_displayTechClear()
{
    _alarmTechInfoWidget->clear();
}

/**************************************************************************************************
 * 设置生理报警提示信息。
 *************************************************************************************************/
void AlarmIndicator::_displayPhySet(AlarmInfoNode &node)
{
    _alarmPhyInfoWidget->display(node);
}

/**************************************************************************************************
 * 设置技术报警提示信息。
 *************************************************************************************************/
void AlarmIndicator::_displayTechSet(AlarmInfoNode &node)
{
    _alarmTechInfoWidget->display(node);
}

/**************************************************************************************************
 * 报警信息显示。
 *************************************************************************************************/
void AlarmIndicator::_displayInfoNode(AlarmInfoNode &alarmNode, int &indexint, int newAlarmIndex, int oldAlarmIndex,
                                      int firstIndex, int lastIndex)
{
    //最新的报警优先显示
    if (-1 != newAlarmIndex)
    {
        alarmNode = _alarmInfoDisplayPool.at(newAlarmIndex);
        --alarmNode.displayTime;
        _alarmInfoDisplayPool[newAlarmIndex] = alarmNode;
    }
    //有报警还有显示剩余时间
    else if ((-1 != oldAlarmIndex) && (indexint != oldAlarmIndex))
    {
        alarmNode = _alarmInfoDisplayPool.at(oldAlarmIndex);
        --alarmNode.displayTime;
        _alarmInfoDisplayPool[oldAlarmIndex] = alarmNode;
    }
    else
    {
        alarmNode = _alarmInfoDisplayPool.at(indexint);
        if (0 != alarmNode.displayTime)
        {
            --alarmNode.displayTime;
            _alarmInfoDisplayPool[indexint] = alarmNode;
        }
        else
        {
            // 如果是当前记录的索引大于技术报警总数，则从头开始。
            indexint = lastIndex;
            if (indexint == -1)
            {
                indexint = firstIndex;
            }
            alarmNode = _alarmInfoDisplayPool.at(indexint);
            alarmNode.displayTime = 2;
            _alarmInfoDisplayPool[indexint] = alarmNode;
        }
    }
}

/**************************************************************************************************
 * 功能：注册生理报警界面指示器。
 *************************************************************************************************/
void AlarmIndicator::setAlarmPhyWidgets(AlarmPhyInfoBarWidget *alarmWidget, AlarmMuteBarWidget *muteWidget)
{
    _alarmPhyInfoWidget = alarmWidget;
    _muteWidget = muteWidget;
}

/**************************************************************************************************
 * 功能：注册技术报警界面指示器。
 *************************************************************************************************/
void AlarmIndicator::setAlarmTechWidgets(AlarmTechInfoBarWidget *alarmWidget)
{
    _alarmTechInfoWidget = alarmWidget;
}

/**************************************************************************************************
 * 功能：添加报警信息。
 * 参数：
 *      alarmTime：报警时间。
 *      alarmType：报警类型。
 *      alarmPriority：报警优先级。
 *      alarmInfo：报警字串。
 *      isRemoveAfterLatch:latch后是否移除报警信息
 *************************************************************************************************/
bool AlarmIndicator::addAlarmInfo(unsigned alarmTime, AlarmType alarmType,
        AlarmPriority alarmPriority, const char *alarmMessage, bool isRemoveAfterLatch)
{
    //报警存在
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if ((it->alarmType == alarmType) && (it->alarmMessage == alarmMessage))
        {
            AlarmInfoNode node = *it;
            if (alarmType != ALARM_TYPE_TECH)
            {
                if (node.latch)
                {
                    node.latch = false;
                    node.pauseTime = 0;
                    *it = node;
                }
            }
            return true;
        }
    }

    // 已满。
    if (list->size() >= ALARM_INFO_POOL_LEN)
    {
        return false;
    }

    AlarmInfoNode node(alarmTime, alarmType, alarmPriority, alarmMessage);
    node.removeAfterLatch = isRemoveAfterLatch;

    list->append(node);
    return true;
}

/**************************************************************************************************
 * 功能：删除报警信息。
 * 参数：
 *      alarmType：报警类型。
 *      alarmInfo：报警的字串。
 *************************************************************************************************/
void AlarmIndicator::delAlarmInfo(AlarmType alarmType, const char *alarmMessage)
{
    //删除显示报警信息池
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if ((it->alarmType == alarmType) && (it->alarmMessage == alarmMessage))
        {
            list->erase(it);
            break;
        }
    }
}

/**************************************************************************************************
 * 功能：栓锁报警信息。
 * 参数：
 *      alarmType：报警类型。
 *      alarmInfo：报警的字串。
 * 返回:true，找到并栓锁，否则返回false。
 *************************************************************************************************/
bool AlarmIndicator::latchAlarmInfo(AlarmType alarmType, const char *alarmMessage)
{
    //删除显示报警信息池
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if ((it->alarmType == alarmType) && (it->alarmMessage == alarmMessage))
        {
            AlarmInfoNode node = *it;
            if (!node.latch)
            {
                node.latch = true;
                node.pauseTime = 0;
                *it = node;
            }

            return true;
        }
    }

    return false;
}

/**************************************************************************************************
 * 功能：更新报警的栓锁状态。
 * 参数：
 *      alarmType：报警类型。
 *      alarmInfo：报警的字串。
 * 返回:true，找到并栓锁，否则返回false。
 *************************************************************************************************/
bool AlarmIndicator::updateLatchAlarmInfo(const char *alarmMessage, bool flag)
{
    //删除显示报警信息池
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if (it->alarmMessage == alarmMessage)
        {
            AlarmInfoNode node = *it;
            node.latch = flag;
            if (flag)
            {
                node.pauseTime = 0;
            }
            *it = node;
            return true;
        }
    }

    return false;
}

/**************************************************************************************************
 * 功能：删除栓锁的报警。
 * 参数：
 *      alarmType：报警类型。
 *      alarmInfo：报警的字串。
 *************************************************************************************************/
void AlarmIndicator::delLatchPhyAlarm()
{
    //删除显示报警信息池
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end();)
    {
        if ((it->alarmType != ALARM_TYPE_TECH) && it->latch)
        {
            it = list->erase(it);
        }
        else
        {
            ++it;
        }
    }
}

/**************************************************************************************************
 * 功能：删除所有的生理报警。
 *************************************************************************************************/
void AlarmIndicator::delAllPhyAlarm()
{
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end();)
    {
        if (it->alarmType != ALARM_TYPE_TECH)
        {
            it = list->erase(it);
        }
        else
        {
            ++it;
        }
    }
}

/**************************************************************************************************
 * 功能：清除暂停倒计时。
 *************************************************************************************************/
void AlarmIndicator::clearAlarmPause()
{
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        AlarmInfoNode node = *it;
        if (0 != node.pauseTime)
        {
            node.pauseTime = 0;
            *it = node;
        }
    }
}

/**************************************************************************************************
 * 功能：检查报警是否存在。
 *************************************************************************************************/
bool AlarmIndicator::checkAlarmIsExist(AlarmType alarmType, const char *alarmMessage)
{
    bool ret = false;
    if (NULL == alarmMessage)
    {
        return ret;
    }

    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if (alarmType == it->alarmType && alarmMessage == it->alarmMessage)
        {
            ret = true;
            break;
        }
    }

    return ret;
}

/**************************************************************************************************
 * 功能：生理报警暂停状态处理。
 * 返回:
 * true,新的生理报警暂停，没有新的生理报警
 *************************************************************************************************/
bool AlarmIndicator::phyAlarmPauseStatusHandle()
{
    bool ret = hasNonPausePhyAlarm();

    AlarmInfoList *list;
    AlarmInfoList::iterator it;
    list = &_alarmInfoDisplayPool;
    it = list->begin();
    for (; it != list->end(); ++it)
    {
        if (it->alarmType != ALARM_TYPE_TECH)
        {
            AlarmInfoNode node = *it;
            if (0 == it->pauseTime)
            {
                if (node.alarmMessage != ecgOneShotAlarm.toString(ECG_ONESHOT_CHECK_PATIENT_ALARM))
                {
                    node.pauseTime = _audioPauseTime;
                }
                else
                {
                    node.pauseTime = _checkPatientAlarmPauseTime;
                }
            }
            else if (!ret)
            {
                node.pauseTime = 0;
            }

            *it = node;
        }
    }

    return ret;
}

/**************************************************************************************************
 * 功能：技术报警报警暂停状态处理。
 *************************************************************************************************/
bool AlarmIndicator::techAlarmPauseStatusHandle()
{
    bool ret = false;

    //技术报警
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end();)
    {
        if (it->alarmType == ALARM_TYPE_TECH && it->alarmPriority != ALARM_PRIO_PROMPT)
        {
            if (it->removeAfterLatch)
            {
                it = list->erase(it);
                ret |= true;
                continue;
            }
            else
            {
                AlarmInfoNode node = *it;
                if (!node.acknowledge)
                {
                    node.acknowledge = true;
                    *it = node;
                    ret |= true;
                }
            }
        }

         ++it;
    }

    return ret;
}

/**************************************************************************************************
 * 功能：是否有处于非暂停的生理报警。
 * 返回:
 * true,有， false，没有
 *************************************************************************************************/
bool AlarmIndicator::hasNonPausePhyAlarm()
{
    //更新显示报警信息池
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if ((it->alarmType != ALARM_TYPE_TECH) && (0 == it->pauseTime))
        {
            return true;
        }
    }

    return false;
}

/**************************************************************************************************
 * 功能：是否有处于栓锁的生理报警。
 * 返回:
 * true,有， false，没有
 *************************************************************************************************/
bool AlarmIndicator::hasLatchPhyAlarm()
{
    //更新显示报警信息池
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if ((it->alarmType != ALARM_TYPE_TECH) && it->latch)
        {
            return true;
        }
    }

    return false;
}

/**************************************************************************************************
 * 功能：更新报警级别。
 * 参数：
 *      alarmType：报警类型。
 *      alarmInfo：报警的字串。
 *      priority:报警级别
 *************************************************************************************************/
void AlarmIndicator::updataAlarmPriority(AlarmType alarmType, const char *alArmMessage,
                                         AlarmPriority priority)
{
    AlarmInfoList *list = &_alarmInfoDisplayPool;

    // 查找报警信息并更新。
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if ((it->alarmType == alarmType) && (it->alarmMessage == alArmMessage))
        {
            AlarmInfoNode node = *it;
            if (node.alarmPriority != priority)
            {
                node.alarmPriority = priority;
                node.displayTime = 3;
                node.acknowledge = false;
                *it = node;
            }
            break;
        }
    }
}

/**************************************************************************************************
 * 功能：更新报警信息。
 * 参数：
 *      node:报警信息
 *************************************************************************************************/
void AlarmIndicator::updateAlarmInfo(const AlarmInfoNode &node)
{
    AlarmInfoList *list = &_alarmInfoDisplayPool;

    // 查找报警信息并更新。
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if ((it->alarmType == node.alarmType) && (it->alarmMessage == node.alarmMessage))
        {
            *it = node;
            break;
        }
    }
}

/**************************************************************************************************
 * 功能：控制报警音。
 *************************************************************************************************/
void AlarmIndicator::setAudioStatus(AlarmAudioStatus status)
{
    if (_audioStatus == status)
    {
        return;
    }

    switch (status)
    {
        case ALARM_AUDIO_NORMAL:
            _muteWidget->setAudioNormal();
            break;
        case ALARM_AUDIO_SUSPEND:
            _muteWidget->setAlarmPause();
            break;
        case ALARM_AUDIO_OFF:
            _muteWidget->setAudioOff();
            break;
        default:
            _muteWidget->setAlarmOff();
            break;
    }

    if (status != ALARM_AUDIO_SUSPEND)
    {
        systemConfig.setNumValue("PrimaryCfg|Alarm|AlarmStatus", (int)(status + 1));
    }

    _audioStatus = status;
    alertor.addAlarmStatus(status);
}

/**************************************************************************************************
 * 功能：获取报警个数。
 *************************************************************************************************/
int AlarmIndicator::getAlarmCount(AlarmType type)
{
    int count = 0;
    AlarmInfoList *list = &_alarmInfoDisplayPool;

    // 无数据。
    if (list->empty())
    {
        return count;
    }

    // 查找报警信息并更新。
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if (it->alarmType == type)
        {
            ++count;
            continue;
        }
    }

    return count;
}

/**************************************************************************************************
 * 功能：获取报警个数。
 *************************************************************************************************/
int AlarmIndicator::getAlarmCount(void)
{
    return _alarmInfoDisplayPool.count();
}

/**************************************************************************************************
 * 功能：获取指定的报警信息
 * 参数：
 *      index：指定的序号
 *      node：带回报警信息
 *************************************************************************************************/
void AlarmIndicator::getAlarmInfo(int index, AlarmInfoNode &node)
{
    if (index >= _alarmInfoDisplayPool.count())
    {
        return;
    }

    node = _alarmInfoDisplayPool.at(index);
}

/**************************************************************************************************
 * 功能：获取指定的报警信息
 * 参数：
 *      alarmType:报警类型
 *      alArmMessage:报警信息
 *      node：带回报警信息
 * 返回：true成功，false失败
 *************************************************************************************************/
bool AlarmIndicator::getAlarmInfo(AlarmType type, const char *alArmMessage,
                                  AlarmInfoNode &node)
{
    AlarmInfoList *list = &_alarmInfoDisplayPool;

    // 查找报警信息并更新。
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if ((it->alarmType == type) && (it->alarmMessage == alArmMessage))
        {
            node = *it;
            return true;
        }
    }

    return false;
}

/**************************************************************************************************
 * 功能：构造。
 *************************************************************************************************/
AlarmIndicator::AlarmIndicator()
{
    _alarmPhyInfoWidget = NULL;
    _alarmTechInfoWidget = NULL;
    _muteWidget = NULL;

    _alarmPhyDisplayIndex = 0;
    _alarmTechDisplayIndex = 0;
    _audioStatus = ALARM_AUDIO_NORMAL;

    _alarmInfoDisplayPool.clear();

    // 记录报警暂停时间
    int pauseTimeIndex = 0;
    _audioPauseTime = 90;
    superConfig.getNumValue("Alarm|AlarmPauseTime", pauseTimeIndex);
    if (pauseTimeIndex > ALARM_PAUSE_TIME_NR)
    {
        pauseTimeIndex = ALARM_PAUSE_TIME_90S;
    }
    _audioPauseTime = 60 + 30 * pauseTimeIndex;

    superConfig.getNumValue("Alarm|NonAlertsBeepsInNonAED", _enableNonAlarmBeepsInNonAED);
}

/**************************************************************************************************
 * 功能：析构。
 *************************************************************************************************/
AlarmIndicator::~AlarmIndicator()
{
    _alarmInfoDisplayPool.clear();
}
