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
    explicit OxyCRGTrendWaveWidget(const QString &waveName,
                                   OxyCRGTrendWaveWidgetPrivate * p);
    ~OxyCRGTrendWaveWidget();

    /**
     * @brief addDataBuf
     * @param value
     * @param flag
     */
    void addDataBuf(int value, int flag);

    /**
     * @brief getIntervalTime
     * @return
     */
    static OxyCRGInterval getIntervalTime(void);

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

private slots:
    /**
     * @brief onTimeOutExec
     */
    void onTimeOutExec(void);

protected:
    /* reimpelment */
    void paintEvent(QPaintEvent *e);

    /* reimplment */
    void showEvent(QShowEvent *e);

    /* reimplment */
    void hideEvent(QHideEvent *e);

    /**
     * @brief setDataRate
     * @param rate
     */
    virtual void setDataRate(int rate);

    QScopedPointer<OxyCRGTrendWaveWidgetPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(OxyCRGTrendWaveWidget)
};
