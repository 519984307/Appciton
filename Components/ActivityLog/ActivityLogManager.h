#pragma once
#include <QString>
#include <QList>
#include "CircleFile.h"

#define MAX_INCIDENT_STR_LEN      (64)

// 日志事件条目
struct IncidentEntry
{
    IncidentEntry(unsigned time = 0, const QString &title = QString())
    {
        this->time = time;
        setTitle(title);
        checkSum = 0;
    }

    void setTitle(const QString &title)
    {
        ::memset(this->title, 0, sizeof(this->title));

        if (title.isEmpty())
        {
            return;
        }

        // 复制名称字符串，并保证以NUL字符结尾
        ::strncpy(this->title, title.toLocal8Bit().constData(), (sizeof(this->title) - 1));
    }

    unsigned makeSum()
    {
        unsigned sum = 0;
        for (unsigned i = 0; i < MAX_INCIDENT_STR_LEN; ++i)
        {
            sum += title[i];
        }

        sum += time;

        return sum;
    }

    unsigned time;
    char title[MAX_INCIDENT_STR_LEN];
    unsigned checkSum;
};

typedef QList<IncidentEntry*> IncidentList;

//活动日志管理
class ActivityLogManager : public CircleFile
{
public:
    static ActivityLogManager &construction()
    {
        if (NULL == _selfobj)
        {
            _selfobj = new ActivityLogManager();
        }

        return *_selfobj;
    }
    static ActivityLogManager *_selfobj;
    ~ActivityLogManager();

    //添加数据项
    void addLogItem(int time_t, QString str);

private:
    ActivityLogManager();
};
#define activityLogManager (ActivityLogManager::construction())
#define deleteActivityLogManager() (delete ActivityLogManager::_selfobj)
