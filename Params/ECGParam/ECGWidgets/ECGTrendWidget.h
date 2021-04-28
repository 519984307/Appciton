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
class QStackedWidget;
class ECGTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    /**
     * @brief setHRValue  and the function of setting HR value
     * @param hr  and hr value
     * @param type
     */
    void setHRValue(short hr, HRSourceType type = HR_SOURCE_AUTO);

    /**
     * @brief setPluginPR
     * @param pr
     */
    void setPluginPR(short pr);

    // 刷新趋势参数上下限
    void updateLimit(void);

    // 是否发生报警
    void isAlarm(bool isAlarm);
    void isPluginPRAlarm(bool isAlarm);

    // 显示参数值
    void showValue(void);

    // 闪烁心跳图标
    void blinkBeatPixmap();

    ECGTrendWidget();
    ~ECGTrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;

public:
    virtual void doRestoreNormalStatus();

public slots:
    /**
     * @brief updateTrendWidget 更新控件
     */
    void updateTrendWidget(bool isPluginConnected = false);
protected:
    void showEvent(QShowEvent *e);
    virtual void setTextSize(void);

    /* reimplment */
    void loadConfig();

    /**
     * @brief setShowStacked set HR/PR stacked show
     * @param num
     */
    void setShowStacked(int num);

private slots:
    void _releaseHandle(IWidget *);
    void _timeOut();

private:
    QLabel *_hrBeatIcon;
    QLabel *_hrValue;
    QLabel *_pluginPRName;    // plugin PR name
    QLabel *_pluginPRValue;   // plugin PR value
    QStackedWidget *_stackedwidget;
    QPixmap beatPixmap;
    QString _hrString;
    QString _pluginPRString;
    bool _isAlarm;
    bool _ispluginPRAlarm;
    void _drawBeatIcon(QColor color);
    QColor lastIconColor;
private:
    QTimer *_timer;
};
