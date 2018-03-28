#pragma once
#include <QWidget>

// Spo2棒图控件。
class SPO2BarWidget: public QWidget
{
    Q_OBJECT
public:
    SPO2BarWidget(int minValue, int maxValue, QWidget *parent = NULL);
    ~SPO2BarWidget();

    void setCellHeight(int cellHeight);
    void setValue(int value);

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    int _minValue;                         // 最小值
    int _maxValue;                         // 最大值
    int _cellHeight;                       // 单元高度
    int _cellNumber;                       // 单元数目
    int _curShowCellMaxIndex;              // 当前显示单元的最大索引
    int _lastShowCellMaxIndex;             // 上一次显示单元的最大索引
};
