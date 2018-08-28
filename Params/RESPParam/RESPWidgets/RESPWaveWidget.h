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

protected:
    void resizeEvent(QResizeEvent *e);
    void focusInEvent(QFocusEvent */*e*/);
    void hideEvent(QHideEvent *e);

private slots:
    void _releaseHandle(IWidget *);
    void _respZoom(IWidget *widget);
    void _popupDestroyed();
    void _getItemIndex(int);

private:
    void _loadConfig();
//    WaveWidgetLabel *_name;
    WaveWidgetLabel *_notify;
    WaveWidgetLabel *_gain;
    PopupList *_gainList;
    int _currentItemIndex;
};
