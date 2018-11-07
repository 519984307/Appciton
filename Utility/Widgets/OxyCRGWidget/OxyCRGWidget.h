/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/20
 **/

#pragma once
#include "IWidget.h"
#include "OxyCRGSymbol.h"

class OxyCRGWidgetPrivate;
class OxyCRGCO2WaveWidget;
class OxyCRGRESPWaveWidget;
class OxyCRGSPO2TrendWidget;
class OxyCRGRRHRWaveWidget;
class OxyCRGWidget : public IWidget
{
    Q_OBJECT
public:
    OxyCRGWidget();
    ~OxyCRGWidget();

    /**
     * @brief setOxyCRGRespWidget
     * @param p
     */
    void setOxyCRGRespWidget(OxyCRGRESPWaveWidget *p);

    /**
     * @brief setOxyCRGSpo2Trend
     * @param p
     */
    void setOxyCRGSPO2Trend(OxyCRGSPO2TrendWidget *p);

    /**
     * @brief setOxyCRGCo2Widget
     * @param p
     */
    void setOxyCRGCO2Widget(OxyCRGCO2WaveWidget *p);

    /**
     * @brief setOxyCRGRrHrWidget
     * @param p
     */
    void setOxyCRGRrHrWidget(OxyCRGRRHRWaveWidget *p);

    /* reimplement */
    void getSubFocusWidget(QList<QWidget *> &subWidget) const;

protected:
    void paintEvent(QPaintEvent *event);
    // 窗体大小调整事件
    void resizeEvent(QResizeEvent *e);

    /* reimplment */
    void showEvent(QShowEvent *e);

    /* reimplment */
    void hideEvent(QHideEvent *e);

private slots:
    /**
     * @brief onIntervalChanged handle the interval change issue
     */
    void onIntervalChanged(int);

    /**
     * @brief onCompressWaveChanged handle the compress wave changed issue
     */
    void onCompressWaveChanged(int);

    /**
     * @brief onCompressWaveClicked on compress wave clicked
     * @param widget
     */
    void onCompressWaveClicked(IWidget *widget);

    /**
     * @brief onIntervalClicked handle interval label click
     * @param widget
     */
    void onIntervalClicked(IWidget *widget);

    /**
     * @brief onSetup slot of the settup label click signal
     * @param widget
     */
    void onSetupClicked(IWidget *widget);

    /**
     * @brief _autoSetRuler  自动设置标尺槽函数
     */
    void onAutoClicked(void);

private:
    OxyCRGWidgetPrivate *const d_ptr;
};
