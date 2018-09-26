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
#include "OxyCRGRESPWidget.h"
#include "OxyCRGHRWidget.h"
#include "OxyCRGSPO2Widget.h"
#include "IButton.h"
#include "OxyCRGCO2Widget.h"

class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class OxyCRGWidgetLabel;
class PopupList;
class OxyCRGWidgetPrivate;

class OxyCRGCO2WaveWidget;
class OxyCRGRESPWaveWidget;
class OxyCRGSPO2TrendWidget;
class OxyCRGRRHRWaveWidget;
class OxyCRGTrendWaveWidget;
class OxyCRGWidget : public IWidget
{
    Q_OBJECT
public:
    OxyCRGWidget();
    ~OxyCRGWidget();

    virtual void setVisible(bool visible);

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

    void setWaveType(int index);
    int  getWaveType(void)const;

protected:
    void paintEvent(QPaintEvent *event);
    // 窗体大小调整事件
    void resizeEvent(QResizeEvent *e);

    /* reimplment */
    void showEvent(QShowEvent *e);

    /* reimplment */
    void hideEvent(QHideEvent *e);

private slots:
    void _intervalSlot(IWidget *widget);
    void _changeTrendSlot(IWidget *widget);
    void _onSetupUpdated(IWidget *widget);
    void _intervalDestroyed();
    void _changeTrendDestroyed();
    void _getIntervalIndex(int);
    void _getChangeTrendIndex(int);
    /**
     * @brief _autoSetRuler  自动设置标尺槽函数
     */
    void _autoSetRuler(void);

signals:
    void _intervalChanged(int index);
private:
    static const int _titleBarHeight = 15;
    static const int _labelHeight = 40;
    QVBoxLayout *_mainLayout;
    QVBoxLayout *_hLayoutWave;
    QLabel *_titleLabel;
    QHBoxLayout *bottomLayout;
    OxyCRGWidgetLabel *_interval;         // 时间间隔
    OxyCRGWidgetLabel *_changeTrend;      // 呼吸波与CO2
    PopupList *_intervalList;
    PopupList *_changeTrendList;
    OxyCRGRESPWidget *_oxycrgWidget;
    OxyCRGHRWidget *_oxycrgHrWidget;
    OxyCRGSPO2Widget *_oxycrgSpo2Widget;
    OxyCRGCO2Widget  *_oxycrgCo2Widget;
    OxyCRGTrendWaveWidget *_oxycrgTrendWaveWidget;
    void _trendLayout(void);
    void _clearLayout(void);

    // 设置/获取时间间隔。
    void _setInterval(OxyCRGInterval index);
    OxyCRGInterval _getInterval(void);

    // 设置/获取RESP/RR。
    void _setTrend(OxyCRGTrend index);
    OxyCRGTrend _getTrend(void);

    int _roundUp(int value, int step);
    int _roundDown(int value, int step);

    float _pixelWPitch;                // 屏幕像素点距, 单位mm
    float _pixelHPitch;                // 屏幕像素点距, 单位mm
    bool _isShowGrid;                         // 是否显示网格
    bool _isShowFrame;                         // 是否显示边框
    bool _isShowScale;                         // 是否显示刻度
    int  _waveType;                    // 波形类型

    int _intervalItemIndex;
    int _changeTrendItemIndex;

private:
    OxyCRGWidgetPrivate *const d_ptr;
};
