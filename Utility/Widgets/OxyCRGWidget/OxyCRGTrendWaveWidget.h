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

class OxyCRGTrendWaveWidgetPrivate;
class OxyCRGTrendWaveWidget : public IWidget
{
    Q_OBJECT
public:
    explicit OxyCRGTrendWaveWidget(const QString &waveName);
    ~OxyCRGTrendWaveWidget();

    /**
     * @brief getXShift
     * @return
     */
    int getXShift(void) const;

    /**
     * @brief getYShift
     * @return
     */
    int getYShift(void) const;

    /**
     * @brief getWxShift
     * @return
     */
    int getWxShift(void) const;

    /**
     * @brief getBufLen
     * @return
     */
    int getBufLen(void) const;

    /**
     * @brief getBufIndex
     * @return
     */
    int getBufIndex(void) const;

    /**
     * @brief getWaveColor
     * @return
     */
    QColor &getWaveColor(void) const;

    /**
     * @brief getRulerHighValue
     * @return
     */
    int getRulerHighValue(void) const;

    /**
     * @brief getRulerLowValue
     * @return
     */
    int getRulerLowValue(void) const;

    /**
     * @brief getWaveBuf
     * @return
     */
    RingBuff<int> *getWaveBuf(void) const;

    /**
     * @brief getFlagBuf
     * @return
     */
    RingBuff<int> *getFlagBuf(void) const;

protected:
    /**
     * @brief addDataBuf
     * @param value
     * @param flag
     */
    void addDataBuf(int value, int flag);

    /**
     * @brief setWaveColor
     * @param color
     */
    void setWaveColor(const QColor &color);

    /**
     * @brief setRulerValue
     * @param valueHigh
     * @param valueLow
     */
    void setRulerValue(int valueHigh, int valueLow);

    /**
     * @brief setBufSize
     * @param bufSize
     */
    void setBufSize(int bufSize);

    /**
     * @brief getIntervalTime
     * @return
     */
    static OxyCRGInterval getIntervalTime(void);

protected:
    /* reimpelment */
    void paintEvent(QPaintEvent *e);

private:
    OxyCRGTrendWaveWidgetPrivate *const d_ptr;
};
