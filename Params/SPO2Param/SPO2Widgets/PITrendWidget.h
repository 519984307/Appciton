/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/6/10
 **/

#pragma once
#include "TrendWidget.h"

class PITrendWidgetPrivate;
class PITrendWidget : public TrendWidget
{
    Q_OBJECT
public:
    void setPIValue(short pi, bool isPlugin = false);

    /*reimplment*/
    void updateLimit();

    // 是否报警
    void isAlarm(bool flag);
    void isPluginPIAlarm(bool flag);

    // 显示参数值
    void showValue();

    PITrendWidget();
    ~PITrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;

public:
    virtual void doRestoreNormalStatus();

protected:
    /*reimplment*/
    void setTextSize();

private slots:
    void onRelease();

private:
    PITrendWidgetPrivate * const d_ptr;
};
