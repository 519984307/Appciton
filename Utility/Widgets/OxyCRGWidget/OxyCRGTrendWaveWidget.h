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
#include "RingBuff.h"
#include "OxyCRGTrendWaveWidget_p.h"

class OxyCRGTrendWaveWidget : public IWidget
{
    Q_OBJECT
public:
    OxyCRGTrendWaveWidget(const QString &waveName,
                                   OxyCRGTrendWaveWidgetPrivate * p);
    ~OxyCRGTrendWaveWidget();

    /**
     * @brief addWaveData
     * @param value
     * @param flag
     */
    void addWaveData(int value);

    /**
     * @brief getIntervalTime
     * @return
     */
    OxyCRGInterval getInterval() const;

    /**
     * @brief setInterval set the
     */
    void setInterval(OxyCRGInterval interval);

    /**
     * @brief setClearWaveDataStatus
     * @param clearStatus
     */
    void setClearWaveDataStatus(bool clearStatus);

    /**
     * @brief setRulerValue
     * @param valueHigh
     * @param valueLow
     */
    void setRulerValue(int valueHigh, int valueLow);

    /**
     * @brief setDataRate
     * @param rate
     */
    virtual void setDataRate(int rate);

private slots:
    /**
     * @brief onTimeOutExec
     */
    void onTimeOutExec(void);

protected:
    /* reimplement */
    void paintEvent(QPaintEvent *e);

    /* reimplement */
    void showEvent(QShowEvent *e);

    /* reimplement */
    void hideEvent(QHideEvent *e);

    /* reimplement */
    void resizeEvent(QResizeEvent *e);

    OxyCRGTrendWaveWidgetPrivate *const d_ptr;
};
