#pragma once
#include <QWidget>
#include "UnitManager.h"

class QLabel;
class CPRBarWidget : public QWidget
{
public:
    // 更新数据。
    void updateValue(short value[], int len);

    // 设置数据采样率。
    void setSampleRate(short sample);

    // 设置数据的范围。
    void setRange(int min, int max);

    // 设置显示单位。
    void setUnit(UnitType t);

    // 构造与析构。
    CPRBarWidget();
    ~CPRBarWidget();

protected:
    void _paintRuler(QPainter &painter);
    void _paintBar(QPainter &painter);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    QVector<short> _barValue;
    int _minValue;
    int _maxValue;
    UnitType _unit;

private:
    // 速度固定为25mm/s
    static const char _speed = 25;

    // 空白区域宽度，像素为单位。
    static const char _blankBarWidth = 5;

    // Bar的宽度，毫米为单位。
    static const char _barWidth = 2;

    short _xPos;                // 当前的绘制位置。
    bool _isDrawBar;            // 绘制Bar或者Blank标志。
    double _pixelPerBar;        // 绘制1个Bar需要的点数。
    double _dataNRPerBar;       // 多少个数据走1个Bar。
    double _drawError;          // 绘制误差。

    // 可绘制区域。
    QRect _drawRect;
    QPixmap *_background;       // 用于恢复被檫掉的棒图。
    bool _resetBackground;
};
