/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#pragma once
#include "TrendWidget.h"
#include <QLabel>

class RESPTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 窗口是否使能
    bool enable();

    /**
     * @brief setRRValue  set BR/RR value
     * @param rr  rr value
     * @param isRR  if is rr source
     * @param isAutoType  if is auto type
     */
    void setRRValue(int16_t rr, bool isRR, bool isAutoType = false);

    // 刷新趋势参数上下限
    void updateLimit(void);

    // 是否报警
    void isAlarm(bool flag);

    // 显示
    void showValue(void);

    RESPTrendWidget();
    ~RESPTrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;
public:
    virtual void doRestoreNormalStatus();

protected:
    virtual void setTextSize(void);

    /* reimplment */
    void loadConfig();

private slots:
    void _releaseHandle(IWidget *);

    /**
     * @brief _onBrSourceStatusUpdate  br来源状态更新槽函数
     */
    void _onBrSourceStatusUpdate();

private:
    QLabel *_rrValue;
    bool _isAlarm;
    QString _rrString;
    QLabel *_rrSource;
};
