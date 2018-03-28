#pragma once
#include "WaveWidget.h"
#include "CO2Define.h"

class WaveWidgetLabel;
class CO2WaveRuler;
class ComboListPopup;

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

    // 构造与析构。
    CO2WaveWidget(const QString &waveName, const QString &title);
    ~CO2WaveWidget();

protected:
    void resizeEvent(QResizeEvent *e);
    void focusInEvent(QFocusEvent */*e*/);
    void hideEvent(QHideEvent *e);

private slots:
    void _releaseHandle(IWidget *);
    void _zoomChangeSlot(IWidget *);
    void _popupDestroyed(void);

private:
//    WaveWidgetLabel *_name;        // 名称
    WaveWidgetLabel *_zoom;        // 标尺
    ComboListPopup *_gainList;     // 增益列表
    CO2WaveRuler *_ruler;          // 标尺对象
};
