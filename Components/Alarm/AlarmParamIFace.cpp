#include "AlarmParamIFace.h"

/**************************************************************************************************
 * 清除所有的报警。
 *************************************************************************************************/
void AlarmOneShotIFace::clear(void)
{
    OneShotMap::iterator it = _oneShotAlarms.begin();
    for (; it != _oneShotAlarms.end(); ++it)
    {
        it.value().reset();
    }
}

void AlarmOneShotIFace::clear(QList<int> ID)
{
    OneShotMap::iterator it = _oneShotAlarms.begin();
    for (; it != _oneShotAlarms.end(); ++it)
    {
        if (ID.contains(it.key()))
        {
            continue;
        }
        it.value().reset();
    }
}

/**************************************************************************************************
 * 设置报警。
 *************************************************************************************************/
void AlarmOneShotIFace::setOneShotAlarm(int id, bool newStatus)
{
    OneShotMap::iterator it = _oneShotAlarms.find(id);
    if (it == _oneShotAlarms.end()) // 报警ID还未注册。
    {
        OneShotAlarmDesc desc(false);
        _oneShotAlarms.insert(id, desc);
    }

    it = _oneShotAlarms.find(id);
    OneShotAlarmDesc &desc = it.value();
    int count = desc.alarmList.count();
    if (1 == count)
    {
        if (desc.alarmList.first() != newStatus)
        {
            if (desc.isHandled)
            {
                desc.alarmList.clear();
            }

            desc.alarmList.append(newStatus);
            desc.isHandled = false;
        }
    }
    else if (count > 1)
    {
        if (desc.alarmList.last() != newStatus)
        {
            int index = desc.alarmList.indexOf(newStatus);
            if (-1 != index)
            {
                QList<bool> tmpList;
                bool isFirstOne = true;
                bool last = !desc.alarmList.first();
                for (int i = 0; i < count; ++i)
                {
                    bool cur = desc.alarmList.at(i);
                    if (cur != newStatus)
                    {
                        if (last != cur)
                        {
                            tmpList.append(cur);
                            last = cur;
                        }
                    }
                    else if (isFirstOne)
                    {
                        tmpList.append(cur);
                        isFirstOne = false;
                        last = cur;
                    }
                }
                tmpList.append(newStatus);
                desc.alarmList.clear();
                desc.alarmList.append(tmpList);
            }
            else
            {
                desc.alarmList.append(newStatus);
            }
        }
    }
    else
    {
        desc.alarmList.append(newStatus);
        desc.isHandled = false;
    }
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
bool AlarmOneShotIFace::isAlarmed(int id)
{
    OneShotMap::iterator it = _oneShotAlarms.find(id);
    if (it == _oneShotAlarms.end())
    {
        return false;
    }

    OneShotAlarmDesc &desc = it.value();
    int count = desc.alarmList.count();
    if (1 == count)
    {
        desc.isHandled = true;
        return desc.alarmList.first();
    }
    else if (count > 1)
    {
        desc.isHandled = false;
        return desc.alarmList.takeFirst();
    }
    else
    {
        desc.isHandled = true;
        desc.alarmList.append(false);
        return false;
    }
}

/**************************************************************************************************
 * return newest alarm status。
 *************************************************************************************************/
bool AlarmOneShotIFace::newestAlarmStatus(int id)
{
    OneShotMap::iterator it = _oneShotAlarms.find(id);
    if (it == _oneShotAlarms.end())
    {
        return false;
    }

    OneShotAlarmDesc &desc = it.value();
    int count = desc.alarmList.count();
    if (count > 0)
    {
        return desc.alarmList.last();
    }
    else
    {
        return false;
    }
}
