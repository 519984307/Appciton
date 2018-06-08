#pragma once
#include "WaveWidget.h"
#include "AGDefine.h"
#include "ParamInfo.h"

class AGWaveRuler;
class ComboListPopup;
class AGWaveWidget: public WaveWidget
{
    Q_OBJECT

public:
    // 设置麻醉剂类型
    void setAnestheticType(AGAnaestheticType type);

    // 添加波形数据。
    void addWaveformData(short data, int flag = 0);

    // 设置波形上下限
    void setLimit(int low, int high);

    // 设置标尺。
    void setRuler(AGDisplayZoom zoom);

    // 构造与析构。
    AGWaveWidget(WaveformID id, const QString &waveName, const AGTypeGas gasType);
    ~AGWaveWidget();

protected:
    // 窗体大小调整事件
    virtual void resizeEvent(QResizeEvent *e);

    // 焦点进入
    virtual void focusInEvent(QFocusEvent */*e*/);

private slots:
    void _releaseHandle(IWidget *);
    void _zoomChangeSlot(IWidget *);
    void _popupDestroyed(void);

private:
    AGWaveRuler *_ruler;               // 标尺对象
    WaveWidgetLabel *_zoom;             // 标尺
    ComboListPopup *_gainList;

};
