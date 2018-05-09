#pragma once
#include <QMap>

enum NIBPStateMachineType
{
    NIBP_STATE_MACHINE_NONE,
    NIBP_STATE_MACHINE_MONITOR,           // 监护模式。
    NIBP_STATE_MACHINE_SERVICE,           // 维护模式
    NIBP_STATE_MACHINE_NR
};

class TimerAction
{
protected: // 用于赋值派生类处理定时器事件。
    struct TimerActionCount
    {
        TimerActionCount(int t)
        {
            targetCount = t;
            count = 0;
        }
        int targetCount;
        int count;
    };
    QMap<int, TimerActionCount> timerAction;

    void clearAction(void)
    {
        timerAction.clear();
    }

    bool isEmpty(void)
    {
        return timerAction.isEmpty();
    }

    void addAction(int action, int targetCount)
    {
        timerAction.insert(action, TimerActionCount(targetCount));
    }

    void addAction(const QMap<int, int> &newActions)
    {
        QMap<int, int>::const_iterator it = newActions.begin();
        for (; it != newActions.end(); ++it)
        {
            timerAction.insert(it.key(), TimerActionCount(it.value()));
        }
    }

    void deleteAction(int action)
    {
        timerAction.remove(action);
    }

    void deleteAction(const QList<int> &actions)
    {
        for (int i = 0; i < actions.size(); i++)
        {
            timerAction.remove(actions[i]);
        }
    }

    bool checkAction(int action)
    {
        bool ret = false;
        QMap<int, TimerActionCount>::iterator it = timerAction.find(action);

        if (it == timerAction.end())
        {
            return ret;
        }

//        if (it.value().delFlag)
//        {
//            timerAction.remove(action);
//            return ret;
//        }

        if (it.value().count++ >= it.value().targetCount)
        {
            ret = true;
            it.value().count = 0;
        }
        return ret;
    }

    int getActionRemainCount(int action)
    {
        QMap<int, TimerActionCount>::iterator it = timerAction.find(action);

        if (it == timerAction.end())
        {
            return 0;
        }

        return it.value().targetCount - it.value().count;
    }

    bool resetAction(int action)
    {
        bool ret = false;
        QMap<int, TimerActionCount>::iterator it = timerAction.find(action);
        if (it == timerAction.end())
        {
            return ret;
        }

        it.value().count = 0;
        return true;
    }

    bool isActionExist(int action)
    {
        QMap<int, TimerActionCount>::iterator it = timerAction.find(action);
        if (it == timerAction.end())
        {
            return false;
        }

        return true;
    }
};
