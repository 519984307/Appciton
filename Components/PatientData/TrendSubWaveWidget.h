#pragma once
#include "IWidget.h"
#include "TrendDataDefine.h"

class TrendSubWaveWidget : public IWidget
{
    Q_OBJECT
public:
    TrendSubWaveWidget(SubParamID id, int xHead, int xTail, int yTop, int yBottom, int downRuler = 0, int upRuler = 100);
    ~TrendSubWaveWidget();

    void demoData(void);

public:
    int yToValue(int y);
    int valueToY(int value);
    int xToIndex(int x);
    int indexToX(int index);

    void beginTrendData(int xHead, int xTail, int trendHead);
    void setThemeColor(QColor color);
    void setRulerRange(int down, int up);
    void cursorMove(int position);

    void loadParamData();

protected:
    void paintEvent(QPaintEvent *e);

private:
    int _yTop;                      // 趋势顶坐标
    int _yBottom;                   // 趋势低坐标
    int _xHead;                     // 趋势开始坐标
    int _xTail;                     // 趋势结束坐标
    int _xSize;                     // 趋势x坐标长度
    int _ySize;                     // 趋势y坐标长度
    int _trendDataHead;             // 趋势数据开始位置
    int _size;                      // 趋势坐标序列容量
    QColor _color;                  // 窗体主题颜色
    QString _paramName;             // 参数名称
    QString _paramUnit;             // 参数单位
    int _upRulerValue;              // 上标尺值
    int _downRulerValue;            // 下标尺值
    int _rulerSize;                 // 标尺大小
    unsigned _startTime;            // 窗口开始时间
    ResolutionRatio _timeInterval;  // 时间间隔

    int *_dataBuf;                  // 原始趋势数据缓存
    QPoint *_trendWaveBuf;          // 趋势坐标序列缓存
    SubParamID _paramID;
    int _cursorPosition;     // 游标位置

};
