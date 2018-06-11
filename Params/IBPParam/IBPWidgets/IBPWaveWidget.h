#pragma once
#include "WaveWidget.h"
#include "IBPDefine.h"
#include "ParamInfo.h"


class IBPWaveRuler;
class ComboListPopup;
class IBPWaveWidget: public WaveWidget
{
    Q_OBJECT

public:
    // 添加波形数据。
    void addWaveformData(short data, int flag = 0);

    // 设置标尺。
    void setRuler(int index);

    // 设置导联状态
    void setLeadSta(int info);

    // 自动设置标尺弹出；
    void displayManualRuler();

    // 设置波形上下限
    void setLimit(int low, int high);

    // 设置获取标名。
    void setEntitle(IBPPressureName entitle);
    IBPPressureName getEntitle(void);

    // 构造与析构。
    IBPWaveWidget(WaveformID id, const QString &waveName, const IBPPressureName &entitle);
    ~IBPWaveWidget();

protected:
    // 重绘事件。
    virtual void paintEvent(QPaintEvent *e);

    // 窗体大小调整事件
    virtual void resizeEvent(QResizeEvent *e);

    // 显示
    virtual void showEvent(QShowEvent *e);

    // 焦点进入
    virtual void focusInEvent(QFocusEvent */*e*/);


private slots:
    void _releaseHandle(IWidget *);
    void _IBPZoom(IWidget *widget);
    void _popupDestroyed();

private:
    // 自动标尺计算
    void _autoRulerHandle(short data);

    short _autoRulerTracePeek;
    short _autoRulerTraveVally;
    unsigned _autoRulerTime;

//    WaveWidgetLabel *_name;         // 名称
    WaveWidgetLabel *_leadSta;          // 导联状态
    WaveWidgetLabel *_zoom;             // 标尺
    IBPWaveRuler *_ruler;               // 标尺对象
    ComboListPopup *_zoomList;          // 标尺列表    
    IBPPressureName _entitle;           // 标名
    int _lowLimit;                      // 下标尺
    int _highLimit;                     // 上标尺
    bool _isAutoRuler;

};
