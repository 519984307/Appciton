#pragma once
#include "IWidget.h"
#include "OxyCRGSymbol.h"

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

protected:
    void paintEvent(QPaintEvent *event);
    // 窗体大小调整事件
    void resizeEvent(QResizeEvent *e);

private slots:
    void _intervalSlot(IWidget *widget);
    void _changeTrendSlot(IWidget *widget);
    void _intervalDestroyed();
    void _changeTrendDestroyed();

private:
    static const int _titleBarHeight = 15;
    static const int _labelHeight = 20;
    QVBoxLayout *_mainLayout;
    QVBoxLayout *_hLayoutWave;
    QLabel *_titleLabel;
    QHBoxLayout *bottomLayout;
    OxyCRGWidgetLabel *_interval;         // 时间间隔
    OxyCRGWidgetLabel *_changeTrend;      // 呼吸波与RR
    ComboListPopup *_intervalList;      // 时间间隔
    ComboListPopup *_changeTrendList;   // 呼吸波与RR列表

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

};
