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
class OxyCRGSPO2TrendWidgetPrivate;
class OxyCRGSPO2TrendWidget : public OxyCRGTrendWaveWidget
{
    Q_OBJECT
public:
    explicit OxyCRGSPO2TrendWidget(const QString &waveName);
    ~OxyCRGSPO2TrendWidget();

    /**
     * @brief addTrendData
     * @param data
     */
    void addTrendData(int data);

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

    Q_DECLARE_PRIVATE(OxyCRGSPO2TrendWidget)
};
