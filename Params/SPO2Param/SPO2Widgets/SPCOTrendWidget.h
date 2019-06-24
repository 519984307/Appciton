/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/6/19
 **/

#pragma once
#include "TrendWidget.h"

class SPCOTrendWidgetPrivate;
class SPCOTrendWidget : public TrendWidget
{
    Q_OBJECT
public:
    SPCOTrendWidget();
    ~SPCOTrendWidget();

    void setSPCOValue(int16_t spco);

    void isAlarm(bool flag);

    void showValue();

    /*reimplment*/
    void updateLimit();

    QList<SubParamID> getShortTrendSubParams() const;

public:
    virtual void doRestoreNormalStatus();

protected:
    /*reimplment*/
    void setTextSize();

private slots:
    void onRelease();

private:
    SPCOTrendWidgetPrivate * const d_ptr;
};
