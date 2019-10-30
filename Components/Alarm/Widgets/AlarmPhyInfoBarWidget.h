/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/25
 **/


#pragma once
#include "IWidget.h"
#include "AlarmDefine.h"
#include "AlarmInfoWindow.h"
#include "AlarmInfoBarWidget.h"

class AlarmPhyWindow;
class AlarmPhyInfoBarWidget : public IWidget, public AlarmInfoBarWidget
{
    Q_OBJECT

public:
    // 清除界面。
    void clear(void);

    // 设置报警提示信息。
    void display(const AlarmInfoNode &node);

    // 构造与析构。
    explicit AlarmPhyInfoBarWidget(const QString &name);
    ~AlarmPhyInfoBarWidget();

    void updateList();

    /**
     * @brief setAlarmPause set the alarm pause time
     * @param seconds the left second of alarm pause
     * @note if the seconds equal to INT_MAX, will display alarm off;
     *       if seconds <= 0, will display normal alarm message
     */
    void setAlarmPause(int seconds);

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);

private slots:
    void _releaseHandle(IWidget *);
    void _alarmListHide();

private:
    void _drawBackground(void);
    void _drawText(void);
    void _drawAlarmPauseMessage(void);

    AlarmPriority _alarmPriority;
    QString _text;
    const char *_message;
    bool _latch;
    bool _acknowledge;
    AlarmParamIFace *_alarmSource;
    int _alarmID;
    int _alarmPauseTime;

    AlarmInfoWindow *_alarmWindow;
    AlarmType _alarmType;
};
