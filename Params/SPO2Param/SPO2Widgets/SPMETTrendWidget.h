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

class SPMETTrendWidgetPrivate;
class SPMETTrendWidget : public TrendWidget
{
public:
    void setSpMetValue(int16_t spmet);

    /*reimplment*/
    void updateLimit();

    void isAlarm(bool flag);

    SPMETTrendWidget();
    ~SPMETTrendWidget();

protected:
    /*reimplment*/
    void setTextSize();

private:
    SPMETTrendWidgetPrivate * const d_ptr;
};
