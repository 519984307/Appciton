/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/19
 **/

#pragma once
#include "AlarmLimitModel.h"
#include "ConfigEditMenuWindow.h"

class ConfigEditAlarmLimitModelPrivate;
class ConfigEditAlarmLimitModel : public AlarmLimitModel
{
    Q_OBJECT
public:
    explicit ConfigEditAlarmLimitModel(Config *config);
    ~ConfigEditAlarmLimitModel();

protected:
    virtual void alarmDataUpdate(const AlarmDataInfo &info, int type);

private:
    ConfigEditAlarmLimitModelPrivate *d_ptr;
};
