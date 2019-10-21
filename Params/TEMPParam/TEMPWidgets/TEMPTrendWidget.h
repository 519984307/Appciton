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
#include <QLabel>
#include "TEMPDefine.h"
#include <QStackedWidget>

enum TempState
{
    TEMP_STATE_ENABLE,
    TEMP_STATE_DISABLE,
    TEMP_STATE_NR,
};

class TEMPTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 设置体温的值。
    void setTEMPValue(int16_t t1, int16_t t2, int16_t td);

    // 设置体温单位
    void setUNit(UnitType u);

    // 是否报警
    void isAlarm(int id, bool flag);

    // 显示新值。
    void showValue(void);

    // 刷新上下限
    void updateLimit();

    void showErrorStatckedWidget(bool error);

    TEMPTrendWidget();
    ~TEMPTrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;
public:
    virtual void doRestoreNormalStatus();

    virtual void updatePalette(const QPalette &pal);

protected:
    virtual void setTextSize(void);

    /* reimplment */
    void loadConfig();
    void showAlarmParamLimit(QWidget *value, const QString &valueStr, QPalette psrc);

    /**
     * @brief darkParamLimit 使得上下限颜色变暗
     */
    void darkerPalette(QPalette &pal);

private slots:
    void _releaseHandle(IWidget *);

    /**
     * @brief onTempNameUpdate
     * @param channel 通道号
     * @param TEMPChannelType 通道类型
     */
    void onTempNameUpdate(TEMPChannelIndex channel, TEMPChannelType type);

private:
    // 温度组
    enum tempGrp
    {
        TEMP_GRP_T1 = 0,
        TEMP_GRP_T2,
        TEMP_GRP_TD,
        TEMP_GRP_NR
    };

    QLabel *_t1Value;
    QLabel *_t2Value;
    QLabel *_tdValue;
    QLabel *_t1Name;
    QLabel *_t2Name;
    QLabel *_tdName;

    QString _t1Str;
    QString _t2Str;
    QString _tdStr;
    bool _t1Alarm;
    bool _t2Alarm;
    bool _tdAlarm;

    QLabel *_t1UpLimit;
    QLabel *_t1DownLimit;
    QLabel *_t2UpLimit;
    QLabel *_t2DownLimit;

    QStackedWidget *statckedWidget;  // 窗口显示内容
    QLabel *_message;    // 显示禁用等信息
};
