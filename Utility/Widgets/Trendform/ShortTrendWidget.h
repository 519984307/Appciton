#pragma once
#include "IWidget.h"
#include "ParamDefine.h"

class QHBoxLayout;
class QVBoxLayout;
class TrendCanvasWidget;
class ShortTrendWidgetLabel;
class ShortTrendWidget: public IWidget
{
    Q_OBJECT
public:
    ShortTrendWidget();
    ~ShortTrendWidget();

    void addItem(SubParamID id);
    void addWidget(void);
    void removeLabel(void);
    void getSubParamList(QList<SubParamID> &list);
    void getFocusWidgetItem(QList<QWidget*> &itemWidget);
    void refresh();
    // 聚焦到指定的波形窗体。
    void focusTrendformWidget(const SubParamID &id);

protected:
    void showEvent(QShowEvent *e);
    // 焦点进入
    void focusInEvent(QFocusEvent */*e*/);

private slots:
    void _releaseHandle(IWidget *widget);

private:
    TrendCanvasWidget *_trendCanvas;
    QVBoxLayout *titleLayout;

    int _interval;                         // 时间间隔。
    int _30sStartTime;                    // the time of the first item that align to 30s ( t % 30 = 0)
    int _totalData;                        // 数据的总个数。

    QMap<SubParamID, ShortTrendWidgetLabel *> _labelItems;

    void _calcInfo(void);
    int _readBlockData(int index, char *buf, int length);
    void _loadData(void);
};
