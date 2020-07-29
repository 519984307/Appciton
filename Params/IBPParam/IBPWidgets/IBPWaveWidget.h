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
#include "IBPDefine.h"
#include "ParamInfo.h"


class IBPWaveRuler;
class PopupList;
class IBPWaveWidget: public WaveWidget
{
    Q_OBJECT

public:
    // 添加波形数据。
    void addWaveformData(short data, int flag = 0);

    // 设置导联状态
    void setLeadSta(int info);

    // 设置波形上下限
    void setLimit(int low, int high);

    // 设置获取标名。
    void setEntitle(IBPLabel entitle);
    IBPLabel getEntitle(void);

    void setRulerLimit(IBPRulerLimit ruler);

    // 构造与析构。
    IBPWaveWidget(WaveformID id, const QString &waveName, const IBPChannel &ibpChn);
    ~IBPWaveWidget();

    /**
     * @brief resetBackground 刷新背景
     * @param pal
     */
    void updatePalette(const QPalette &pal);

    /**
     * @brief updateLimit update ruler range
     */
    void updateRulerRange(void);
protected:
    // 窗体大小调整事件
    virtual void resizeEvent(QResizeEvent *e);

    // 显示
    virtual void showEvent(QShowEvent *e);

    // 焦点进入
    virtual void focusInEvent(QFocusEvent */*e*/);

    /* reimplment */
    void loadConfig(void);

private:
    // 自动标尺计算
    void _autoRulerHandle(short data);

    short _autoRulerTracePeek;
    short _autoRulerTraveVally;
    unsigned _autoRulerTime;

//    WaveWidgetLabel *_name;         // 名称
    WaveWidgetLabel *_leadSta;          // 导联状态
    IBPWaveRuler *_ruler;               // 标尺对象
    IBPChannel _ibpChn;          // IBP channel ID
    IBPLabel _entitle;           // 标名
    int _lowLimit;                      // 下标尺
    int _highLimit;                     // 上标尺
    bool _isAutoRuler;
};
