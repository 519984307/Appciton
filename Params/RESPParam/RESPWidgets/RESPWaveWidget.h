/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/27
 **/


#pragma once
#include "WaveWidget.h"
#include "RESPDefine.h"

class WaveWidgetLabel;
class PopupList;
class RESPWaveWidget: public WaveWidget
{
    Q_OBJECT

public:
    RESPWaveWidget(const QString &waveName, const QString &title);
    ~RESPWaveWidget();

    void setZoom(int zoom);

    // 电极脱落
    void leadoff(bool flag);

    // 窗口是否使能
    virtual bool waveEnable();

    /**
     * @brief setWaveformMode  设置波形模式
     * @param mode  模式
     */
    void setWaveformMode(RESPSweepMode mode);

protected:
    void resizeEvent(QResizeEvent *e);
    void focusInEvent(QFocusEvent */*e*/);
    void hideEvent(QHideEvent *e);
    /* reimplment */
    void loadConfig();

private slots:
    void _respZoom(IWidget *widget);
    void _popupDestroyed();
    void _getItemIndex(int);

    /**
     * @brief _onCalcLeadChanged  计算导联改变槽函数
     * @param lead  导联
     */
    void _onCalcLeadChanged(RESPLead lead);

private:
    /**
     * @brief _adjustLabelLayout  调整标签布局
     */
    void _adjustLabelLayout(void);

//    WaveWidgetLabel *_name;
    WaveWidgetLabel *_notify;
    WaveWidgetLabel *_gain;
    PopupList *_gainList;
    int _currentItemIndex;
    WaveWidgetLabel *_lead;
};
