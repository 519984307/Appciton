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
class ComboListPopup;
class OxyCRGWidget : public IWidget
{
    Q_OBJECT
public:
    OxyCRGWidget();
    ~OxyCRGWidget();

    virtual void setVisible(bool visible);
    void setOxyCrgRespWidget(OxyCRGRESPWidget *p);
    void setOxyCrgHrWidget(OxyCRGHRWidget *p);
    void setOxyCrgSpo2Widget(OxyCRGSPO2Widget *p);
    void setOxyCrgCo2Widget(OxyCRGCO2Widget *p);
    void setWaveType(int index);
    int  getWaveType(void)const;
protected:
    void paintEvent(QPaintEvent *event);
    // 窗体大小调整事件
    void resizeEvent(QResizeEvent *e);

private slots:
    void _intervalSlot(IWidget *widget);
    void _changeTrendSlot(IWidget *widget);
    void _onSetupUpdated(IWidget *widget);
    void _intervalDestroyed();
    void _changeTrendDestroyed();
signals:
    void _intervalChanged(int index);
private:
    static const int _titleBarHeight = 15;
    static const int _labelHeight = 20;
    QVBoxLayout *_mainLayout;
    QVBoxLayout *_hLayoutWave;
    QLabel *_titleLabel;
    QHBoxLayout *bottomLayout;
    OxyCRGWidgetLabel *_setUp;
    OxyCRGWidgetLabel *_interval;         // 时间间隔
    OxyCRGWidgetLabel *_changeTrend;      // 呼吸波与CO2
    ComboListPopup *_intervalList;      // 时间间隔
    ComboListPopup *_changeTrendList;   // 呼吸波与CO2列表
    OxyCRGRESPWidget *_oxycrgWidget;
    OxyCRGHRWidget *_oxycrgHrWidget;
    OxyCRGSPO2Widget *_oxycrgSpo2Widget;
    OxyCRGCO2Widget  *_oxycrgCo2Widget;
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
};
