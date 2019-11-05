/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/20
 **/

#pragma once
#include "OxyCRGTrendWaveWidget.h"
#include "Framework/Utility/RingBuff.h"

class OxyCRGRRHRWaveWidgetPrivate;
class OxyCRGRRHRWaveWidget : public OxyCRGTrendWaveWidget
{
    Q_OBJECT
public:
    explicit OxyCRGRRHRWaveWidget(const QString &waveName);
    ~OxyCRGRRHRWaveWidget();

    /**
     * @brief addRrTrendData
     * @param value
     */
    void addRrTrendData(int value);

    /**
     * @brief addHrDataBuf
     * @param value
     */
    void addHrTrendData(int value);

    /**
     * @brief setRrRulerValue
     * @param valueHigh
     * @param valueLow
     */
    void setRrRulerValue(int valueHigh, int valueLow);

    /**
     * @brief setHrRulerValue
     * @param valueHigh
     * @param valueLow
     */
    void setHrRulerValue(int valueHigh, int valueLow);

    /**
     * @brief setRrTrendShowStatus
     * @param isShow
     */
    void setRrTrendShowStatus(bool isShow);

    /* reimplement */
    void clearData();

    /* reimplement */
    void setInterval(OxyCRGInterval interval);

protected:
    /* reimplment */
    void paintEvent(QPaintEvent *e);

private:
    Q_DECLARE_PRIVATE(OxyCRGRRHRWaveWidget)
};
