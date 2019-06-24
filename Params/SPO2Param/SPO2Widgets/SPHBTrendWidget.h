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

class SPHBTrendWidgetPrivate;
class SPHBTrendWidget : public TrendWidget
{
    Q_OBJECT
public:
    void setSPHBValue(int16_t sphb);

    /*reimplment*/
    void updateLimit();

    // 是否报警
    void isAlarm(bool flag);

    // 显示参数值
    void showValue();

    SPHBTrendWidget();
    ~SPHBTrendWidget();
public:
    virtual void doRestoreNormalStatus();

protected:
    /*reimplment*/
    void setTextSize();

private slots:
    void onRelease();

private:
    SPHBTrendWidgetPrivate * const d_ptr;
};
