#pragma once
#include "IWidget.h"
#include "TrendDataDefine.h"

struct TrendConvertDesc
{
    TrendConvertDesc()
    {
        max = 0;
        min = 0;
        start = 0;
        end = 0;
    }
    int max;        // 数据范围最大值
    int min;        // 数据范围最小值
    int start;      // 坐标开始像素点
    int end;        // 坐标结束像素点
};

class TrendSubWaveWidget : public IWidget
{
    Q_OBJECT
public:
    TrendSubWaveWidget(SubParamID id, TrendGraphType type);
    ~TrendSubWaveWidget();

    void trendDataInfo(TrendGraphInfo info);

    void updateTrendGraph(void);
    void loadTrendSubWidgetInfo(TrendSubWidgetInfo info);

public:
    void setThemeColor(QColor color);
    void setRulerRange(int down, int up);
    void setTimeRange(unsigned leftTime, unsigned rightTime);
    void cursorMove(int position);

    SubParamID getSubParamID();

protected:
    void paintEvent(QPaintEvent *e);

private:
    double _mapValue(TrendConvertDesc desc, int data);

private:
    TrendSubWidgetInfo _info;
    SubParamID _id;
    TrendGraphType _type;
    TrendGraphInfo _trendInfo;      // 趋势图数据信息
    TrendConvertDesc _timeX;
    TrendConvertDesc _valueY;
    int _xSize;                     // 趋势x坐标长度
    int _ySize;                     // 趋势y坐标长度
    int _trendDataHead;             // 趋势数据开始位置
    QColor _color;                  // 窗体主题颜色
    QString _paramName;             // 参数名称
    QString _paramUnit;             // 参数单位
    int _cursorPosIndex;            // 游标位置
//    int _size;                      // 趋势坐标序列容量

//    int _upRulerValue;              // 上标尺值*
//    int _downRulerValue;            // 下标尺值*
//    int _rulerSize;                 // 标尺大小
//    unsigned _startTime;            // 窗口开始时间
//    ResolutionRatio _timeInterval;  // 时间间隔

//    int _cursorPosition;            // 游标位置


};
