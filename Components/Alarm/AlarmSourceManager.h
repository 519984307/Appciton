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
    ONESHOT_ALARMSOURCE_ECG,
    ONESHOT_ALARMSOURCE_RESP,
    ONESHOT_ALARMSOURCE_SPO2,
    ONESHOT_ALARMSOURCE_NIBP,
    ONESHOT_ALARMSOURCE_CO2,
    ONESHOT_ALARMSOURCE_TEMP,
    ONESHOT_ALARMSOURCE_IBP,
    ONESHOT_ALARMSOURCE_CO,
    ONESHOT_ALARMSOURCE_AG,
    ONESHOT_ALARMSOURCE_O2,
    ONESHOT_ALARMSOURCE_SYSTEM,
    ONESHOT_ALARMSOURCE_PRINT,
    ONESHOT_ALARMSOURCE_BATTERY,
};

enum LimitAlarmSourceID
{
    LIMIT_ALARMSOURCE_ECG,
    LIMIT_ALARMSOURCE_ECGDUP,
    LIMIT_ALARMSOURCE_RESP,
    LIMIT_ALARMSOURCE_RESPDUP,
    LIMIT_ALARMSOURCE_SPO2,
    LIMIT_ALARMSOURCE_NIBP,
    LIMIT_ALARMSOURCE_CO2,
    LIMIT_ALARMSOURCE_TEMP,
    LIMIT_ALARMSOURCE_IBP,
    LIMIT_ALARMSOURCE_CO,
    LIMIT_ALARMSOURCE_AG,
    LIMIT_ALARMSOURCE_O2,
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
