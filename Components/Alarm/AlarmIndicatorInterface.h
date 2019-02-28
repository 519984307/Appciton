/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/2/27
 **/
#pragma once

class AlarmIndicatorInterface
{
public:
    AlarmIndicatorInterface(){}
    ~AlarmIndicatorInterface(){}

    /**
     * @brief registerAlarmIndicator 注册报警指示器
     * @return
     */
    static AlarmIndicatorInterface *registerAlarmIndicator(AlarmIndicatorInterface* interface);

    /**
     * @brief getAlarmIndicator 获取报警指示器句柄
     * @return
     */
    static AlarmIndicatorInterface *getAlarmIndicator();
};
