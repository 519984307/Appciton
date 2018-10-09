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

class COTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 构造和析构
    explicit COTrendWidget(const QString &trendName);
    ~COTrendWidget();

public:
    // display C.O. and C.I. data.
    void setMeasureResult(u_int16_t coData, u_int16_t ciData);

    // display temp blood data.
    void setTBData(u_int16_t tbData);

    QList<SubParamID> getShortTrendSubParams() const;
protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_coValue;

    QLabel *_ciName;
    QLabel *_ciValue;
    QLabel *_tbName;
    QLabel *_tbValue;

    QString _coStr;
    QString _ciStr;
    QString _tbStr;
};
