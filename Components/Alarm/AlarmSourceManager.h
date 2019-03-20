/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/20
 **/

#pragma once
#include "AlarmParamIFace.h"

enum OneShotAlarmSourceID
{
    ONESHOT_SYSTEM,
};

enum LimitAlarmSourceID
{
};

class AlarmSourceManagerPrivate;
class AlarmSourceManager
{
public:
    static AlarmSourceManager &getInstance();
    ~AlarmSourceManager();
    /**
     * @brief registerOneShotAlarmSource 注册一个OneShot报警源
     * @param oneShot
     * @param id
     */
    void registerOneShotAlarmSource(AlarmOneShotIFace *oneShot, OneShotAlarmSourceID id);

    /**
     * @brief registerLimitAlarmSource 注册一个超限报警源
     * @param limit
     * @param id
     */
    void registerLimitAlarmSource(AlarmLimitIFace *limit, LimitAlarmSourceID id);

    /**
     * @brief getOneShotAlarmSource 获取一个指定的OneShot报警源
     * @param id
     * @return
     */
    AlarmOneShotIFace *getOneShotAlarmSource(OneShotAlarmSourceID id);

    /**
     * @brief getLimitAlarmSource 获取一个指定的超限报警源
     * @param id
     * @return
     */
    AlarmLimitIFace *getLimitAlarmSource(LimitAlarmSourceID id);

private:
    AlarmSourceManager();
    AlarmSourceManager(const AlarmSourceManager &other);
    AlarmSourceManagerPrivate * const d_ptr;
};
#define alarmSourceManager (AlarmSourceManager::getInstance())
