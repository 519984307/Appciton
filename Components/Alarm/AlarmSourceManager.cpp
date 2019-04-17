/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/20
 **/

#include "AlarmSourceManager.h"

class AlarmSourceManagerPrivate
{
public:
    AlarmSourceManagerPrivate(){}

    QMap<OneShotAlarmSourceID, AlarmOneShotIFace *> oneShotAlarmSourceMap;
    QMap<LimitAlarmSourceID, AlarmLimitIFace *> limitAlarmSourceMap;
};

AlarmSourceManager &AlarmSourceManager::getInstance()
{
    static AlarmSourceManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new AlarmSourceManager();
    }
    return *instance;
}

AlarmSourceManager::~AlarmSourceManager()
{
    delete d_ptr;
}

AlarmSourceManager::AlarmSourceManager()
    : d_ptr(new AlarmSourceManagerPrivate())
{
}

void AlarmSourceManager::registerOneShotAlarmSource(AlarmOneShotIFace *oneShot, OneShotAlarmSourceID id)
{
    if (d_ptr->oneShotAlarmSourceMap.value(id, NULL) == NULL)
    {
        d_ptr->oneShotAlarmSourceMap.insert(id, oneShot);
    }
}

void AlarmSourceManager::registerLimitAlarmSource(AlarmLimitIFace *limit, LimitAlarmSourceID id)
{
    if (d_ptr->limitAlarmSourceMap.value(id, NULL) == NULL)
    {
        d_ptr->limitAlarmSourceMap.insert(id, limit);
    }
}

AlarmOneShotIFace *AlarmSourceManager::getOneShotAlarmSource(OneShotAlarmSourceID id)
{
    QMap<OneShotAlarmSourceID, AlarmOneShotIFace *>::ConstIterator it = d_ptr->oneShotAlarmSourceMap.find(id);
    if (it == d_ptr->oneShotAlarmSourceMap.end())
    {
        return NULL;
    }

    return it.value();
}

AlarmLimitIFace *AlarmSourceManager::getLimitAlarmSource(LimitAlarmSourceID id)
{
    QMap<LimitAlarmSourceID, AlarmLimitIFace *>::ConstIterator it = d_ptr->limitAlarmSourceMap.find(id);
    if (it == d_ptr->limitAlarmSourceMap.end())
    {
        return NULL;
    }

    return it.value();
}
