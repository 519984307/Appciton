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
    void setPIValue(int16_t pi);

    /*reimplment*/
    void updateLimit();

    void isAlarm(bool flag);

    PITrendWidget();
    ~PITrendWidget();

protected:
    /*reimplment*/
    void setTextSize();

private:
    PITrendWidgetPrivate * const d_ptr;
};
