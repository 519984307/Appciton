/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/19
 **/

#pragma once
#include "IWidget.h"
#include "OxyCRGDefine.h"

class OxyCRGTrendWaveWidgetPrivate;
class OxyCRGTrendWaveWidget : public IWidget
{
    Q_OBJECT
public:
    OxyCRGTrendWaveWidget(const QString &waveName,
                                   OxyCRGTrendWaveWidgetPrivate * p);
    ~OxyCRGTrendWaveWidget();

    /**
     * @brief addWaveData add wave data to the ring buffer and the interval point cache
     * @param value
     */
    void addWaveData(int value);

    /**
     * @brief getIntervalTime get the intervel setting of the OxyCRG widget
     * @return
     */
    OxyCRGInterval getInterval() const;

    /**
     * @brief setInterval set the interval
     */
    virtual void setInterval(OxyCRGInterval interval);

    /**
     * @brief clearData clear all the wave data and the interval point cache
     */
    virtual void clearData();

    /**
     * @brief setRulerValue set the wave data ruler
     * @param valueHigh the high value
     * @param valueLow the low value
     */
    void setRulerValue(int valueHigh, int valueLow);

protected:
    /* reimplement */
    void paintEvent(QPaintEvent *e);

    /* reimplement */
    void resizeEvent(QResizeEvent *e);

    OxyCRGTrendWaveWidgetPrivate *const d_ptr;
};
