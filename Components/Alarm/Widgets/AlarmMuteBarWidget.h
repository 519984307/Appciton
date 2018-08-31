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

class AlarmMuteBarWidget : public IWidget
{
    Q_OBJECT

public:
    // 设置状态正常。
    void setAudioNormal(void);

    // 设置报警静音开关状态。
    void setAudioOff(void);

    // 设置报警关闭状态
    void setAlarmOff(void);

    // 设置报警暂停状态
    void setAlarmPause(void);

    // 构造与析构。
    AlarmMuteBarWidget();
    virtual ~AlarmMuteBarWidget();

protected:
    void paintEvent(QPaintEvent *e);

private:
    QPixmap _muteAlarmPause;            // 报警暂停图标。
    QPixmap _muteAudioOff;              // 报警静音图标。
    QPixmap _muteAlarmOff;              // 报警关闭图标。
    AlarmAudioStatus _audioStatus;
};
