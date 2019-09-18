/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/20
 **/

#pragma once
#include "AlarmDefine.h"
#include <qglobal.h>

class AlarmInfoBarWidget
{
public:
    virtual ~AlarmInfoBarWidget(){}

    static AlarmInfoBarWidget *registerAlarmInfoBar(AlarmType alarmType, AlarmInfoBarWidget *instance);

    static AlarmInfoBarWidget *getAlarmPhyInfoBar(AlarmType alarmType);

    /**
     * @brief clear 清除界面。
     */
    virtual void clear(void) = 0;

    /**
     * @brief display 设置报警提示信息。
     * @param node
     */
    virtual void display(AlarmInfoNode &node) = 0;

    /**
     * @brief updateList 更新报警列表
     */
    virtual void updateList() = 0;

    /**
     * @brief setAlarmPause set the alarm pause time
     * @param seconds the left second of alarm pause
     * @note if the seconds equal to INT_MAX, will display alarm off;
     *       if seconds <= 0, will display normal alarm message
     */
    virtual void setAlarmPause(int seconds){Q_UNUSED(seconds)}
};
