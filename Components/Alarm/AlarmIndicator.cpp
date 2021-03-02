/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/12
 **/

#include "AlarmIndicator.h"
#include "AlarmStatusWidget.h"
#include "AlarmInfoBarWidget.h"
#include "Framework/Sound/SoundManagerInterface.h"
#include "LightManagerInterface.h"
#include "IConfig.h"
#include "AlarmSourceManager.h"
#include "AlarmStateMachineInterface.h"
#include "AlarmInterface.h"
#include "NurseCallManagerInterface.h"
#include "PatientInfoWidgetInterface.h"

/**************************************************************************************************
 * 功能：发布报警。
 *************************************************************************************************/
void AlarmIndicator::publishAlarm(AlarmStatus status)
{
    SoundManagerInterface *soundManager = SoundManagerInterface::getSoundManager();
    LightManagerInterface *lightManager = LightManagerInterface::getLightManager();
    AlarmStateMachineInterface *alarmStateMachine = AlarmStateMachineInterface::getAlarmStateMachine();
    if (_alarmInfoDisplayPool.isEmpty())
    {
        _displayPhyClear();
        _displayTechClear();
        if (soundManager)
        {
            soundManager->updateAlarm(false, ALARM_PRIO_LOW);
        }
        if (lightManager)
        {
            lightManager->updateAlarm(false, ALARM_PRIO_LOW);
        }
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
    bool hasAcknowledgAlarm = false;
    AlarmPriority phySoundPriority = ALARM_PRIO_PROMPT;
    AlarmPriority techSoundPriority = ALARM_PRIO_PROMPT;
    AlarmPriority lightPriority = ALARM_PRIO_PROMPT;
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    AlarmInterface *alertor = AlarmInterface::getAlarm();
    for (; it != list->end(); ++it)
    {
        AlarmInfoNode node = *it;

        if (0 != node.acknowledge)
        {
            hasAcknowledgAlarm = true;
        }

        if (ALARM_STATUS_OFF != status && ALARM_TYPE_TECH != node.alarmType)
        {
            if (0 < it->pauseTime)
            {
                --node.pauseTime;
                *it = node;
            }

            if (0 < it->alarmAudioDelay)
            {
                --node.alarmAudioDelay;
                *it = node;
            }
            // pause/audio off/reset状态没有生理报警声音
            if (0 == node.pauseTime
                    && status != ALARM_STATUS_AUDIO_OFF
                    && status != ALARM_STATUS_RESET
                    && status != ALARM_STATUS_PAUSE
                    && !node.acknowledge
                    && 0 == node.alarmAudioDelay)
            {
                if (phySoundPriority < node.alarmPriority)
                {
                    phySoundPriority = node.alarmPriority;
                }
            }

            if (alertor && node.alarmPriority != ALARM_PRIO_PROMPT)
            {
                // 处理确认后且开启了报警复位灯，或者未确认的报警
                if (lightPriority < node.alarmPriority)
                {
                    lightPriority = node.alarmPriority;
                }
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
            if ((!node.acknowledge || node.latch)
                    && node.alarmPriority != ALARM_PRIO_PROMPT)
            {
                if (status != ALARM_STATUS_PAUSE && techSoundPriority < node.alarmPriority)
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
                }
            }
            else if (node.acknowledge && !node.removeLigthAfterConfirm && node.alarmPriority != ALARM_PRIO_PROMPT)
            {
                // 处理确认后不移除灯光
                if (lightPriority < node.alarmPriority)
                {
                    lightPriority = node.alarmPriority;
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
    if (phySoundPriority != ALARM_PRIO_PROMPT || techSoundPriority != ALARM_PRIO_PROMPT)
    {
        if (phySoundPriority >= techSoundPriority && _canPlayAudio(status, false))
        {
            // 生理报警等级更高时，播放生理报警等级的报警音
            if (soundManager)
            {
                soundManager->updateAlarm(true, phySoundPriority);
            }
        }
        else if (techSoundPriority > phySoundPriority && _canPlayAudio(status, true))
        {
            // 技术报警等级更高时，播放技术报警等级的报警音
            if (soundManager)
            {
                soundManager->updateAlarm(true, techSoundPriority);
            }
        }
        else
        {
            // 关闭报警音
            if (soundManager)
            {
                soundManager->updateAlarm(false, phySoundPriority);
            }
        }
    }
    else
    {
        // 生理和技术报警都是提示等级时，关闭报警音
        if (soundManager)
        {
            soundManager->updateAlarm(false, phySoundPriority);
        }
    }

    // 更新灯光
    if (lightManager)
    {
        lightManager->updateAlarm(true, lightPriority);
    }

    //生理报警
    if (phyAlarmNum > 0)
    {
        if (_alarmPhyDisplayIndex >= _alarmInfoDisplayPool.count() || _alarmPhyDisplayIndex == -1)
        {
            _alarmPhyDisplayIndex = firstPhyIndex;
        }
        AlarmInfoNode alarmPhyNode;

        _displayInfoNode(&alarmPhyNode, &_alarmPhyDisplayIndex,
                         newPhyAlarmIndex, oldPhyAlarmIndex, firstPhyIndex, lastPhyIndex);
        if (NULL != alarmPhyNode.alarmMessage && alarmPhyNode.alarmType == ALARM_TYPE_PHY)
        {
            _displayPhySet(alarmPhyNode);
        }
    }
    else
    {
        _alarmPhyDisplayIndex = firstPhyIndex;
        _displayPhyClear();
    }

    //技术报警
    if (techAlarmNum > 0)
    {
        if (_alarmTechDisplayIndex >= _alarmInfoDisplayPool.count() || _alarmTechDisplayIndex == -1)
        {
            _alarmTechDisplayIndex = firstTechIndex;
        }
        AlarmInfoNode alarmTechNode;
        _displayInfoNode(&alarmTechNode, &_alarmTechDisplayIndex,
                         newTechAlarmIndex, oldTechAlarmIndex, firstTechIndex, lastTechIndex);

        if (NULL != alarmTechNode.alarmMessage)
        {
            _displayTechSet(alarmTechNode);
        }
    }
    else
    {
        _alarmTechDisplayIndex = firstTechIndex;
        _displayTechClear();
    }

    // 生理报警条件全部解除
    if (!hasNonLatchedPhyAlarm)
    {
        if (alarmStateMachine)
        {
            alarmStateMachine->handAlarmEvent(ALARM_STATE_EVENT_ALL_PHY_ALARM_LATCHED, 0, 0);
        }
    }

    // 没有处于倒计时的报警
    if (!hasPausedPhyAlarm)
    {
        if (alarmStateMachine)
        {
            alarmStateMachine->handAlarmEvent(ALARM_STATE_EVENT_NO_PAUSED_PHY_ALARM, 0, 0);
        }
    }

    // 没有被确认的报警
    if (!hasAcknowledgAlarm)
    {
        if (alarmStateMachine)
        {
            alarmStateMachine->handAlarmEvent(ALARM_STATE_EVENT_NO_ACKNOWLEDG_ALARM, 0, 0);
        }
    }

    // 护士呼叫
    NurseCallManagerInterface *nurseCallManager = NurseCallManagerInterface::getNurseCallManagerInterface();
    if (nurseCallManager)
    {
        for (int i = ALARM_TYPE_PHY; i <= ALARM_TYPE_TECH; i++)
        {
            for (int j = ALARM_PRIO_LOW; j <= ALARM_PRIO_HIGH; j++)
            {
                int count = getAlarmCount(static_cast<AlarmType>(i), static_cast<AlarmPriority>(j));
                nurseCallManager->callNurse(static_cast<AlarmType>(i), static_cast<AlarmPriority>(j), count);
            }
        }
    }
}

/**************************************************************************************************
 * 清除生理报警界面。
 *************************************************************************************************/
void AlarmIndicator::_displayPhyClear(void)
{
    if (_alarmPhyInfoWidget)
    {
        _alarmPhyInfoWidget->clear();
    }
}

/**************************************************************************************************
 * 清除技术报警界面。
 *************************************************************************************************/
void AlarmIndicator::_displayTechClear()
{
    if (_alarmTechInfoWidget)
    {
        _alarmTechInfoWidget->clear();
    }
}

/**************************************************************************************************
 * 设置生理报警提示信息。
 *************************************************************************************************/
void AlarmIndicator::_displayPhySet(const AlarmInfoNode &node)
{
    if (_alarmPhyInfoWidget)
    {
        _alarmPhyInfoWidget->display(node);
    }
}

/**************************************************************************************************
 * 设置技术报警提示信息。
 *************************************************************************************************/
void AlarmIndicator::_displayTechSet(const AlarmInfoNode &node)
{
    if (_alarmTechInfoWidget)
    {
        _alarmTechInfoWidget->display(node);
    }
}

bool AlarmIndicator::_canPlayAudio(AlarmStatus status, bool isTechAlarm)
{
    int alarmAudio = 1; /* defaut enable */
    systemConfig.getNumValue("Alarms|AlarmAudio", alarmAudio);
    if (status == ALARM_STATUS_NORMAL)
    {
        if (alarmAudio)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    if (isTechAlarm && status != ALARM_STATUS_OFF)
    {
        if (alarmAudio)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}

/**************************************************************************************************
 * 报警信息显示。
 *************************************************************************************************/
void AlarmIndicator::_displayInfoNode(AlarmInfoNode *alarmNode, int *indexint, int newAlarmIndex, int oldAlarmIndex,
                                      int firstIndex, int lastIndex)
{
    //最新的报警优先显示
    if (-1 != newAlarmIndex)
    {
        *alarmNode = _alarmInfoDisplayPool.at(newAlarmIndex);
        --alarmNode->displayTime;
        _alarmInfoDisplayPool[newAlarmIndex] = *alarmNode;
    }
    //有报警还有显示剩余时间
    else if ((-1 != oldAlarmIndex) && (*indexint != oldAlarmIndex))
    {
        *alarmNode = _alarmInfoDisplayPool.at(oldAlarmIndex);
        --alarmNode->displayTime;
        _alarmInfoDisplayPool[oldAlarmIndex] = *alarmNode;
    }
    else
    {
        int index = *indexint;
        *alarmNode = _alarmInfoDisplayPool.at(index);
        if (0 != alarmNode->displayTime)
        {
            --alarmNode->displayTime;
            _alarmInfoDisplayPool[*indexint] = *alarmNode;
        }
        else
        {
            // 如果是当前记录的索引大于技术报警总数，则从头开始。
            *indexint = lastIndex;
            if (*indexint == -1)
            {
                *indexint = firstIndex;
            }
            *alarmNode = _alarmInfoDisplayPool.at(index);
            alarmNode->displayTime = 2;
            _alarmInfoDisplayPool[index] = *alarmNode;
        }
    }
}

/**************************************************************************************************
 * 功能：注册生理报警界面指示器。
 *************************************************************************************************/
void AlarmIndicator::setAlarmPhyWidgets(AlarmInfoBarWidget *alarmWidget, AlarmStatusWidget *muteWidget,
                                        PatientInfoWidgetInterface *patInfoWidget)
{
    _alarmPhyInfoWidget = alarmWidget;
    _alarmStatusWidget = muteWidget;
    _patInfoWidget = patInfoWidget;
}

/***************************************************************************************************
 * constructor
 **************************************************************************************************/
AlarmIndicator &AlarmIndicator::getInstance()
{
    static AlarmIndicator *instance = NULL;
    if (instance == NULL)
    {
        instance = new AlarmIndicator();
        AlarmIndicatorInterface *old = registerAlarmIndicator(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

/**************************************************************************************************
 * 功能：注册技术报警界面指示器。
 *************************************************************************************************/
void AlarmIndicator::setAlarmTechWidgets(AlarmInfoBarWidget *alarmWidget)
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
                                  AlarmPriority alarmPriority, const char *alarmMessage,
                                  AlarmParamIFace *alarmSource, int alarmID, bool isRemoveAfterLatch,
                                  bool isRemoveLightAfterConfirm)
{
    if (alarmMessage == NULL)
    {
        return false;
    }
    //报警存在
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if ((it->alarmType == alarmType) && strcmp(it->alarmMessage, alarmMessage) == 0)
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

    AlarmInfoNode node(alarmTime, alarmType, alarmPriority, alarmMessage, alarmSource, alarmID);
    node.removeAfterLatch = isRemoveAfterLatch;
    node.removeLigthAfterConfirm = isRemoveLightAfterConfirm;
    node.alarmAudioDelay = alarmSource->getAlarmAudioDelay(alarmID);

    list->append(node);

    NurseCallManagerInterface *nurseCallManager = NurseCallManagerInterface::getNurseCallManagerInterface();
    if (nurseCallManager)
    {
        nurseCallManager->callNurse(alarmType, alarmPriority);
    }
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
    if (alarmMessage == NULL)
    {
        return;
    }
    //删除显示报警信息池
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    bool isNeedUpdatePhyIndex = false;
    bool isNeedUpdateTechIndex = false;
    for (; it != list->end(); ++it)
    {
        if ((it->alarmType == alarmType) && strcmp(it->alarmMessage, alarmMessage) == 0)
        {
            if (alarmType == ALARM_TYPE_PHY)
            {
                // 如果删除当前正在显示的报警,
                if (_alarmPhyDisplayIndex == _alarmInfoDisplayPool.indexOf(*it))
                {
                    isNeedUpdatePhyIndex = true;
                }
            }
            else if (alarmType == ALARM_TYPE_TECH)
            {
                if (_alarmTechDisplayIndex == _alarmInfoDisplayPool.indexOf(*it))
                {
                    isNeedUpdateTechIndex = true;
                }
            }
            list->erase(it);
            break;
        }
    }

    // 将索引更新为下一条同类报警的索引
    if (isNeedUpdatePhyIndex)
    {
        int index = _alarmPhyDisplayIndex;
        _alarmPhyDisplayIndex = -1;
        for (int i = index ; i < _alarmInfoDisplayPool.count(); i++)
        {
            if (_alarmInfoDisplayPool.at(i).alarmType == ALARM_TYPE_PHY)
            {
                _alarmPhyDisplayIndex = i;
                break;
            }
        }
    }
    else if (isNeedUpdateTechIndex)
    {
        int index = _alarmTechDisplayIndex;
        _alarmTechDisplayIndex = -1;
        for (int i = index ; i < _alarmInfoDisplayPool.count(); i++)
        {
            if (_alarmInfoDisplayPool.at(i).alarmType == ALARM_TYPE_TECH)
            {
                _alarmTechDisplayIndex = i;
                break;
            }
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
    if (alarmMessage == NULL)
    {
        return false;
    }
    //删除显示报警信息池
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if ((it->alarmType == alarmType) && strcmp(it->alarmMessage, alarmMessage) == 0)
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
    if (alarmMessage == NULL)
    {
        return false;
    }
    //删除显示报警信息池
    AlarmInfoList *list = &_alarmInfoDisplayPool;
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if (strcmp(it->alarmMessage, alarmMessage) == 0)
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
        if (alarmType == it->alarmType && strcmp(alarmMessage, it->alarmMessage) == 0)
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
                node.pauseTime = _audioPauseTime;
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
bool AlarmIndicator::updataAlarmPriority(AlarmType alarmType, const char *alarmMessage,
        AlarmPriority priority)
{
    if (alarmMessage == NULL)
    {
        return false;
    }
    AlarmInfoList *list = &_alarmInfoDisplayPool;

    // 查找报警信息并更新。
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if ((it->alarmType == alarmType) && strcmp(it->alarmMessage, alarmMessage) == 0)
        {
            AlarmInfoNode node = *it;
            if (node.alarmPriority != priority)
            {
                node.alarmPriority = priority;
                node.displayTime = 3;
                node.acknowledge = it->acknowledge;
                *it = node;
                return true;
            }
            break;
        }
    }
    return false;
}

/**************************************************************************************************
 * 功能：更新报警信息。
 * 参数：
 *      node:报警信息
 *************************************************************************************************/
void AlarmIndicator::updateAlarmInfo(const AlarmInfoNode &node)
{
    if (node.alarmMessage == NULL)
    {
        return;
    }
    AlarmInfoList *list = &_alarmInfoDisplayPool;

    // 查找报警信息并更新。
    AlarmInfoList::iterator it = list->begin();
    for (; it != list->end(); ++it)
    {
        if ((it->alarmType == node.alarmType) && strcmp(it->alarmMessage, node.alarmMessage) == 0)
        {
            *it = node;
            break;
        }
    }
}

/**************************************************************************************************
 * 功能：控制报警音。
 *************************************************************************************************/
void AlarmIndicator::setAlarmStatus(AlarmStatus status)
{
    if (_audioStatus == status)
    {
        return;
    }

    if (_alarmStatusWidget)
    {
        _alarmStatusWidget->setAlarmStatus(status);
    }

    if (status != ALARM_STATUS_PAUSE && status != ALARM_STATUS_RESET)
    {
        systemConfig.setNumValue("PrimaryCfg|Alarm|AlarmStatus", static_cast<int>(status + 1));
    }

    _audioStatus = status;
    AlarmInterface *alertor = AlarmInterface::getAlarm();
    if (alertor)
    {
        alertor->addAlarmStatus(status);
    }
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

int AlarmIndicator::getAlarmCount(AlarmPriority priority)
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
        if (it->alarmPriority == priority)
        {
            ++count;
            continue;
        }
    }

    return count;
}

int AlarmIndicator::getAlarmCount(AlarmType type, AlarmPriority priority)
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
        if (it->alarmType == type && it->alarmPriority == priority)
        {
            ++count;
            continue;
        }
    }

    return count;
}

/**************************************************************************************************
 * 功能：获取指定的报警信息    alarmIndicator.latchAlarmInfo(ALARM_TYPE_PHY, useCase[ALARM_INFO_POOL_LEN]);
    alarmIndicator.delLatchPhyAlarm();
    QCOMPARE(alarmIndicator.hasLatchPhyAlarm(), false);
 * 参数：
 *      index：指定的序号
 *      node：带回报警信息
 *************************************************************************************************/
AlarmInfoNode AlarmIndicator::getAlarmInfo(int index)
{
    if (index >= _alarmInfoDisplayPool.count())
    {
        return AlarmInfoNode();
    }
    return _alarmInfoDisplayPool.at(index);
}

/**************************************************************************************************
 * 功能：构造。
 *************************************************************************************************/
AlarmIndicator::AlarmIndicator()
    : AlarmIndicatorInterface(),
      _audioStatus(ALARM_STATUS_NORMAL)
{
    _alarmPhyInfoWidget = NULL;
    _alarmTechInfoWidget = NULL;
    _alarmStatusWidget = NULL;

    _alarmPhyDisplayIndex = 0;
    _alarmTechDisplayIndex = 0;

    _alarmInfoDisplayPool.clear();

    // 记录报警暂停时间
    int pauseTimeIndex = 0;
    _audioPauseTime = 90;
    systemConfig.getNumValue("Alarms|AlarmPauseTime", pauseTimeIndex);
    if (pauseTimeIndex > ALARM_PAUSE_TIME_NR)
    {
        pauseTimeIndex = ALARM_PAUSE_TIME_2MIN;
    }
    _audioPauseTime = 60 + 30 * pauseTimeIndex;
}

/**************************************************************************************************
 * 功能：析构。
 *************************************************************************************************/
AlarmIndicator::~AlarmIndicator()
{
    _alarmInfoDisplayPool.clear();
}

void AlarmIndicator::updateAlarmPauseTime(int seconds)
{
    if (_patInfoWidget)
    {
        _patInfoWidget->setAlarmPauseTime(seconds);
    }
}

bool AlarmIndicator::phyAlarmResetStatusHandle()
{
    bool ret = false;
    AlarmInfoList *list;
    AlarmInfoList::iterator it;
    list = &_alarmInfoDisplayPool;
    it = list->begin();
    for (; it != list->end(); ++it)
    {
        if (it->alarmType == ALARM_TYPE_PHY && it->alarmPriority > ALARM_PRIO_PROMPT)
        {
            // 只确认中级和高级的报警
            AlarmInfoNode node = *it;
            if (0 == it->pauseTime)
            {
                if (!node.acknowledge)
                {
                    node.acknowledge = true;
                    ret = true;
                }
            }
            *it = node;
        }
    }
    return ret;
}

bool AlarmIndicator::techAlarmResetStatusHandle()
{
    bool ret = false;
    AlarmInfoList *list;
    AlarmInfoList::iterator it;
    list = &_alarmInfoDisplayPool;
    it = list->begin();
    while (it != list->end())
    {
        if (it->alarmType == ALARM_TYPE_TECH && it->alarmPriority > ALARM_PRIO_PROMPT)
        {
            // 只确认非提示等级的报警
            if (it->removeAfterLatch)
            {
                it = list->erase(it);
                ret = true;
                continue;
            }
            else if (!it->acknowledge)
            {
                it->acknowledge = true;
                it->removeLigthAfterConfirm = false;
                ret = true;
            }
        }
        ++it;
    }
    return ret;
}

bool AlarmIndicator::removeAllAlarmResetStatus()
{
    bool ret = false;
    AlarmInfoList *list;
    AlarmInfoList::iterator it;
    list = &_alarmInfoDisplayPool;
    it = list->begin();
    while (it != list->end())
    {
        if (it->alarmPriority > ALARM_PRIO_PROMPT)
        {
            if (it->acknowledge)
            {
                it->acknowledge = false;
                it->removeLigthAfterConfirm = false;
                ret = true;
            }
        }
        ++it;
    }
    return ret;
}

void AlarmIndicator::updateAlarmAudioState()
{
    _alarmStatusWidget->updateAlarmAudioStatus();
}

bool AlarmIndicator::isAlarmAudioState()
{
    int index = 0;
    systemConfig.getNumValue("Alarms|AlarmAudio", index);
    return index ? true : false;
}

void AlarmIndicator::setAlarmAudioState(bool flag)
{
    int index = flag ? 1 : 0;
    systemConfig.setNumValue("Alarms|AlarmAudio", index);
    updateAlarmAudioState();
}
