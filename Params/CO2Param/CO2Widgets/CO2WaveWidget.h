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
#include "CO2Define.h"

class WaveWidgetLabel;
class CO2WaveRuler;
class PopupList;

class CO2WaveWidget: public WaveWidget
{
    Q_OBJECT

public:
    // 窗口是否使能
    virtual bool waveEnable();

    // 设置波形模式。
    void setWaveformMode(CO2SweepMode mode);

    // 设置标尺。
    void setRuler(CO2DisplayZoom zoom);

    /**
     * @brief resetBackground 刷新背景
     * @param pal
     */
    void updatePalette(const QPalette &pal);

    // 构造与析构。
    CO2WaveWidget(const QString &waveName, const QString &title);
    ~CO2WaveWidget();

protected:
    void resizeEvent(QResizeEvent *e);
    void focusInEvent(QFocusEvent */*e*/);
    void hideEvent(QHideEvent *e);

    /* reimplment */
    void loadConfig();

private slots:
    void _getItemIndex(int);

private:
//    WaveWidgetLabel *_name;        // 名称
    CO2WaveRuler *_ruler;          // 标尺对象
    int _currentItemIndex;
};
