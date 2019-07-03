/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/12/18
 **/


#include "FreezeManager.h"
#include "ParamDefine.h"
#include "TimeDate.h"
#include <QList>
#include "TimeDate.h"
#include "TrendCache.h"
#include "TrendDataDefine.h"
#include "TrendDataStorageManager.h"

class FreezeManagerPrivate
{
public:
    FreezeManagerPrivate()
        :currentReviewSecond(0),
          inReviewMode(false)
    {
    }
    int currentReviewSecond;
    QList<FreezeDataModel*> dataModels;
    bool inReviewMode;
    TrendDataPackage trendData;
};

FreezeManager::FreezeManager()
    :QObject(), d_ptr(new FreezeManagerPrivate())
{
}

FreezeManager::~FreezeManager()
{
    qDeleteAll(d_ptr->dataModels);
    d_ptr->dataModels.clear();
}

FreezeManager &FreezeManager::getInstance()
{
    static FreezeManager  *instance = NULL;
    if (instance == NULL)
    {
        instance = new FreezeManager();
    }
    return *instance;
}

void FreezeManager::startFreeze()
{
    emit freeze(true);
    TrendCacheData data;
    TrendAlarmStatus almStatus;
    unsigned t = timeDate.time();
    trendCache.collectTrendData(t);
    trendCache.collectTrendAlarmStatus(t);
    trendCache.getTrendData(t, data);
    trendCache.getTrendAlarmStatus(t, almStatus);
    bool alarm = false;
    foreach(bool st, almStatus.alarms) {
        if (st)
        {
            alarm = true;
            break;
        }
    }
    d_ptr->trendData.subparamValue = data.values;
    d_ptr->trendData.subparamAlarm = almStatus.alarms;
    d_ptr->trendData.co2Baro = data.co2baro;
    d_ptr->trendData.time = t;
    d_ptr->trendData.alarmFlag = alarm;

    trendDataStorageManager.storeData(t, TrendDataStorageManager::CollectStatusFreeze);
}

void FreezeManager::stopFreeze()
{
    emit freeze(false);
    qDeleteAll(d_ptr->dataModels);
    d_ptr->dataModels.clear();
    d_ptr->inReviewMode = false;
    d_ptr->currentReviewSecond = 0;
}

bool FreezeManager::isInReviewMode() const
{
    return d_ptr->inReviewMode;
}

void FreezeManager::enterReviewMode()
{
    if (!d_ptr->inReviewMode)
    {
        d_ptr->inReviewMode = true;
        emit freezeReview();
    }
}

int FreezeManager::getCurReviewSecond() const
{
    return d_ptr->currentReviewSecond;
}

void FreezeManager::setCurReviewSecond(int reviewSecond)
{
    if (reviewSecond == d_ptr->currentReviewSecond || reviewSecond > MAX_REVIEW_SECOND)
    {
        return;
    }

    d_ptr->currentReviewSecond = reviewSecond;

    foreach(FreezeDataModel *m, d_ptr->dataModels) {
        m->setReviewStartSecond(reviewSecond);
    }
}

FreezeDataModel *FreezeManager::getWaveDataModel(int waveid)
{
    WaveformID id = static_cast<WaveformID> (waveid);

    // check whether already exist
    foreach(FreezeDataModel *m, d_ptr->dataModels) {
        if (m->getWaveformID() == id)
        {
            return m;
        }
    }

    FreezeDataModel *model = new FreezeDataModel(timeDate.time(), id);
    d_ptr->dataModels.append(model);
    return model;
}

const TrendDataPackage &FreezeManager::getTrendData() const
{
    return d_ptr->trendData;
}
