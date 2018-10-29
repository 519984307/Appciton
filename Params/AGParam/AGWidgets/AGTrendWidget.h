/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/30
 **/

#pragma once
#include "TrendWidget.h"
#include "AGDefine.h"
#include <QLabel>

class AGTrendWidget: public TrendWidget
{
    Q_OBJECT
public:
    // 设置麻醉剂类型
    void setAnestheticType(AGAnaestheticType type);

    // 设置测量实时数据。
    void setEtData(int16_t etValue);
    void setFiData(int16_t fiValue);

    // 刷新趋势参数上下限
    void updateLimit(void);

    // 是否报警
    void isAlarm(int id, bool flag);

    // 显示参数值
    void showValue(void);

    // 构造与析构。
    AGTrendWidget(const QString &trendName, const AGTypeGas gasType);
    ~AGTrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_etValue;
    QLabel *_fiName;
    QLabel *_fiValue;

    AGTypeGas _gasType;

    QString _etStr;
    QString _fiStr;
    bool _etAlarm;
    bool _fiAlarm;
};
