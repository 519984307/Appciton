/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/7
 **/


#pragma once
#include "TrendWidget.h"
#include "ECGDefine.h"

class QLabel;
class QTimer;
class ECGTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    /**
     * @brief setHRValue  and the function of setting HR value
     * @param hr  and hr value
     * @param type
     */
    void setHRValue(int16_t hr, HRSourceType type = HR_SOURCE_AUTO);

    // 刷新趋势参数上下限
    void updateLimit(void);

    // 是否发生报警
    void isAlarm(bool isAlarm);

    // 显示参数值
    void showValue(void);

    // 闪烁心跳图标
    void blinkBeatPixmap();

    ECGTrendWidget();
    ~ECGTrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;
public:
    virtual void doRestoreNormalStatus();

protected:
    void showEvent(QShowEvent *e);
    virtual void setTextSize(void);

    /* reimplment */
    void loadConfig();

private slots:
    void _releaseHandle(IWidget *);
    void _timeOut();

private:
    QLabel *_hrBeatIcon;
    QLabel *_hrValue;
    QPixmap beatPixmap;
    QString _hrString;
    bool _isAlarm;
    void _drawBeatIcon(QColor color);
    QColor lastIconColor;
private:
    QTimer *_timer;
};
