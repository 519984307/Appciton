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
#include "RingBuff.h"

class OxyCRGRRHRWaveWidgetPrivate;
class OxyCRGRRHRWaveWidget : public OxyCRGTrendWaveWidget
{
    Q_OBJECT
public:
    explicit OxyCRGRRHRWaveWidget(const QString &waveName);
    ~OxyCRGRRHRWaveWidget();

    /**
     * @brief addRrDataBuf
     * @param value
     * @param flag
     */
    void addRrDataBuf(int value, int flag);

    /**
     * @brief addHrDataBuf
     * @param value
     * @param flag
     */
    void addHrDataBuf(int value, int flag);

    /**
     * @brief getHrWaveColor
     * @return
     */
    QColor &getHrWaveColor(void);

    /**
     * @brief getHrRulerHighValue
     * @return
     */
    int getHrRulerHighValue(void) const;

    /**
     * @brief getHrRulerLowValue
     * @return
     */
    int getHrRulerLowValue(void) const;

    /**
     * @brief getWaveBuf
     * @return
     */
    RingBuff<int> *getHrWaveBuf(void) const;

    /**
     * @brief getFlagBuf
     * @return
     */
    RingBuff<int> *getHrFlagBuf(void) const;

protected:
    /* reimplment */
    void paintEvent(QPaintEvent *e);

private slots:
    /**
     * @brief onTimeOutExec
     */
    void onTimeOutExec(void);

private:
    /**
     * @brief init
     */
    void init(void);

    OxyCRGRRHRWaveWidgetPrivate *const d_ptr;
};
