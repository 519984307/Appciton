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
#include "ParamInfo.h"
#include "ECGDefine.h"

// 记录R波标记,用于重画，防止R波标记被刷新后只显示一半
struct rMark_record
{
    rMark_record()
    {
        ellipse_x = 0;
        ellipse_y = 0;
        s_x = 0;
        s_y = 0;
    }

    int ellipse_x;
    int ellipse_y;
    int s_x;
    int s_y;
};

class WaveWidgetLabel;
class ECGWaveRuler;
class PopupList;
class ECGWaveRuler;
/**************************************************************************************************
 * ECG 波形显示控件
 *************************************************************************************************/
class ECGWaveWidget : public WaveWidget
{
    Q_OBJECT
public:
    ECGWaveWidget(WaveformID id, const QString &widgetName, const QString &leadName);
    ~ECGWaveWidget();

    // 获取当前控件的高度，毫秒为单位。
    int getHeightMM(void);

    // 设置+/-0.5mv对应的数值。
    void set05MV(int p05mv, int n05mv);

    // 添加波形数据。
    void addWaveformData(int data, int pace = 0);

    /**
     * @brief setGain 设置波形增益
     * @param gain
     * @param isAuto 是否为自动增益导致的增益设置
     */
    void setGain(ECGGain gain, bool isAuto = false);

    // 设置12导界面下波形增益
    void set12LGain(ECGGain gain);

    // 获取12导界面下波形增益
    ECGGain get12LGain();

    // 设置波形速度。
    void setSpeed(ECGSweepSpeed speed);

    // 设置带宽
    void setBandWidth(ECGBandwidth bandWidth);

    // 设置自动增益标志
    void setAutoGain(bool flag);

    // set wave notify message
    void setNotifyMesg(ECGWaveNotify mesg);

    // update the lead display name
    void updateLeadDisplayName(const QString &name);

    /**
     * @brief updateWaveConfig update the wave config
     */
    void updateWaveConfig();

    // 画ECG波形标记
    static void drawIPaceMark(QPainter &painter, QPoint &start, QPoint &end);

    static void drawEPaceMark(QPainter &painter, QPoint &start, QPoint &end, QRect &r,
                              unsigned flag, ECGWaveWidget *pObj = NULL);
    static void drawRMark(QPainter &painter, QPoint &p, QRect &r, ECGWaveWidget *pObj = NULL);

    // 记录R波标记,用于重画，防止R波标记被刷新后只显示一半
    QList<rMark_record> rMarkList;

    /**
     * @brief resetBackground 刷新背景
     */
    void updatePalette(const QPalette &pal);

    /**
     * @brief setWaveInfoVisible  设置波形相关信息是否可视化
     * @param isVisible
     */
    void setWaveInfoVisible(bool isVisible);

    /* reimplment */
    void updateWidgetConfig();

protected:
    // 重绘事件。
    virtual void paintEvent(QPaintEvent *e);

    // 窗体大小调整事件
    virtual void resizeEvent(QResizeEvent *e);

    // 显示
    virtual void showEvent(QShowEvent *e);

    // hide
    virtual void hideEvent(QHideEvent *e);

    // 焦点进入
    virtual void focusInEvent(QFocusEvent */*e*/);

    /* reimplment */
    void loadConfig(void);

private slots:
    // 鼠标释放事件
    void _popupDestroyed();
    void _getItemIndex(int);
    /**
     * @brief _updateFilterMode 更新滤波方式槽函数
     */
    void _updateFilterMode(void);
    /**
     * @brief _updateNotchInfo 更新陷波信息槽函数
     */
    void _updateNotchInfo(void);

private:
    double _calcRulerHeight(ECGGain gain);
    void _initValueRange(ECGGain gain);

private:
    WaveWidgetLabel *_notify;       // 提示标签
    WaveWidgetLabel *_gain;       // ECG第一道波形增益提示标签
    WaveWidgetLabel *_filterMode;   // ECG第一道波形滤波方式提示标签
    WaveWidgetLabel *_notchInfo;   // ECG第一道波形陷波信息提示标签
    ECGWaveRuler *_ruler;           // 标尺
    static int _paceHeight;         // 起搏标记高度, 单位像素
    int _p05mV;                     // +0.5mV对应的波形数值
    int _n05mV;                     // -0.5mV对应的波形数值
    ECGGain _12LGain;               // 12L界面下的显示带宽

    int _currentItemIndex;

private:
    void _autoGainHandle(int data);
    void _calcGainRange(void);

    bool _isAutoGain;
    unsigned _autoGainTime;
    int _autoGainTracePeek;
    int _autoGainTraveVally;
    ECGWaveNotify _mesg;
    struct autoGainLogicalRange
    {
        autoGainLogicalRange()
        {
            minRange = 0;
            maxRange = 0;
        }
        int minRange;
        int maxRange;
    };
    autoGainLogicalRange _autoGainLogicalRange[ECG_GAIN_NR - 1];

    /**
     * @brief _adjustLabelLayout  调整标签布局
     */
    void _adjustLabelLayout(void);
};
