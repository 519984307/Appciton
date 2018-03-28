#pragma once
#include "WaveWidget.h"
#include "RESPDefine.h"

class WaveWidgetLabel;
class ComboListPopup;
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

private:
    void _loadConfig();
//    WaveWidgetLabel *_name;
    WaveWidgetLabel *_notify;
    WaveWidgetLabel *_gain;
    ComboListPopup *_gainList;
};
