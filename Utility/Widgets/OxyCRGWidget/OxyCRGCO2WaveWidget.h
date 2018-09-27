/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/25
 **/

#pragma once
#include "OxyCRGTrendWaveWidget.h"
class OxyCRGCO2WaveWidgetPrivate;
class OxyCRGCO2WaveWidget : public OxyCRGTrendWaveWidget
{
    Q_OBJECT
public:
    explicit OxyCRGCO2WaveWidget(const QString &waveName);
    ~OxyCRGCO2WaveWidget();

    /* reimplment */
    void setDataRate(int rate);

protected:
    /* reimplment */
    void paintEvent(QPaintEvent *e);

    /* reimplment */
    void showEvent(QShowEvent *e);

    /* reimplment */
    void hideEvent(QHideEvent *e);

private:
    /**
     * @brief init
     */
    void init(void);

    Q_DECLARE_PRIVATE(OxyCRGCO2WaveWidget)
};
