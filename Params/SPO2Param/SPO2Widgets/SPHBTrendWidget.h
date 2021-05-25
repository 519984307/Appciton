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
#include "Framework/Utility/Unit.h"

class SPHBTrendWidgetPrivate;
class SPHBTrendWidget : public TrendWidget
{
    Q_OBJECT
public:
    void setSPHBValue(int16_t sphb);

    /*reimplment*/
    void updateLimit();

    /**
     * @brief updateUnit update the display unit label
     * @param unit
     */
    void updateUnit(UnitType unit);

    // 是否报警
    void isAlarm(bool flag);

    // 显示参数值
    void showValue();

    SPHBTrendWidget();
    ~SPHBTrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;


public:
    virtual void doRestoreNormalStatus();

protected:
    /*reimplment*/
    void setTextSize();
    /* reimplment */
    void loadConfig();

private slots:
    void onRelease();

private:
    SPHBTrendWidgetPrivate * const d_ptr;
};
