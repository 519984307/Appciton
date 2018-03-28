#pragma once
#include "WaveWidget.h"
#include "SPO2Define.h"

class WaveWidgetLabel;

class ComboListPopup;
class SPO2WaveWidget : public WaveWidget
{
    Q_OBJECT

public:
    void setNotify(bool enable, QString str = " ");

    SPO2WaveWidget(const QString &waveName, const QString &title);
    ~SPO2WaveWidget();

    // 设置波形增益。
    void setGain(SPO2Gain gain);

    // 获取不同增益下的极值
    static void getGainToValue(SPO2Gain gain, int &min, int &max);

    bool waveEnable();

protected:
    // 窗体大小调整事件
    virtual void resizeEvent(QResizeEvent *e);
    virtual void focusInEvent(QFocusEvent */*e*/);

private slots:
    void _releaseHandle(IWidget *);
    void _spo2Gain(IWidget *widget);
    void _popupDestroyed();

private:
    void _initValueRange(SPO2Gain gain);

    void _loadConfig(void);
//    WaveWidgetLabel *_name;
    WaveWidgetLabel *_gain;         // 增益标签
    WaveWidgetLabel *_notify;
    ComboListPopup *_gainList;      // 增益列表
};
