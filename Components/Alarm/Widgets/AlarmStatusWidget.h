/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/31
 **/


#pragma once
#include "IWidget.h"
#include "AlarmDefine.h"

class AlarmStatusWidgetPrivate;
class AlarmStatusWidget : public IWidget
{
    Q_OBJECT
public:
    /**
     * @brief setAlarmStatus set the alarm status
     * @param status new status
     */
    void setAlarmStatus(AlarmStatus status);

    // 构造与析构。
    AlarmStatusWidget();
    virtual ~AlarmStatusWidget();

    /**
     * @brief setAlarmAudioStatus 设置是否显示报警音关闭
     * @param status
     */
    void updateAlarmAudioStatus();

protected:
    void paintEvent(QPaintEvent *e);

private:
    AlarmStatusWidgetPrivate * const d_ptr;
};
