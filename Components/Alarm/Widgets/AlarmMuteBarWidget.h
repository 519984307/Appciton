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
