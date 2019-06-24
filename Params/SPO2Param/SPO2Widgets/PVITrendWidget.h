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

class PVITrendWidgetPrivate;
class PVITrendWidget : public TrendWidget
{
    Q_OBJECT
public:
    void setPVIValue(int16_t pvi);

    /*reimplment*/
    void updateLimit(void);

    void isAlarm(bool flag);

    void showValue(void);

    /*reimplment*/
    virtual void doRestoreNormalStatus();

    PVITrendWidget();
    ~PVITrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;

protected:
    /*reimplment*/
    void setTextSize();

    /*reimplment*/
    void loadConfig();

private slots:
    void onRelease();

private:
    PVITrendWidgetPrivate *const d_ptr;
};
