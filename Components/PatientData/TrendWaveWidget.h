#pragma once
#include "IWidget.h"
#include "TrendSubWaveWidget.h"
#include "TrendDataDefine.h"
#include <QScrollArea>

class QHBoxLayout;
class QVBoxLayout;
class TrendWaveWidget : public IWidget
{
    Q_OBJECT
public:
    TrendWaveWidget();
    ~TrendWaveWidget();

public:
    void changeTrendDisplay();
    void setWidgetSize(int w, int h);
    void leftMoveCoordinate();
    void rightMoveCoordinate();
    void leftMoveCursor();
    void rightMoveCursor();
    void leftMoveEvent();
    void rightMoveEvent();

    void pageUpParam();
    void pageDownParam();

    void updateTrendGraph();
    void setTimeInterval(ResolutionRatio timeInterval);
    void setWaveNumber(int num);
    void setRulerLimit(SubParamID, int down, int up);

    void loadTrendData(SubParamID id);
    void updateTimeRange(void);

    //test
    const QList<TrendDataPackage*> getTrendDatapacketList() {return _trendDataPack;}

protected:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent *e);

private:
    void _trendLayout(void);
    void _clearLayout(void);
    void _getTrendData(void);
    void _initWaveSubWidget(void);
    double _getCursorPos(unsigned t);               // 获取坐标位置
    void _calculationPage(void);                    // 计算当前数据量的页数

private:
    QVBoxLayout *_mainLayout;
    QVBoxLayout *_hLayoutTrend;

    ResolutionRatio _timeInterval;
    int _waveRegionWidth;
    int _oneFrameWidth;

    unsigned _initTime;
    int _cursorPosIndex;                            //游标位置在存储数据结构容器中的索引值
    unsigned _currentCursorTime;

    IWidget *_subWidget;
    QScrollArea *_subWidgetScrollArea;

    int _displayGraphNum;                           // 一屏显示的波形数
    int _totalGraphNum;                             // 趋势组显示的总波形数
    int _curVScroller;

    QList<TrendDataPackage *> _trendDataPack;       // 趋势数据包
    int _totalPage;                                 // 总数据页数 *
    int _currentPage;                               // 当前页数 *

    unsigned _leftTime;                             // 趋势图左边时间 *
    unsigned _rightTime;                            // 趋势图右边时间 *

    QMap<SubParamID, TrendSubWaveWidget *> _subWidgetMap;       // 子波形窗口容器；
    TrendGraphInfo _trendGraphInfo;                 // 趋势图数据集合
};
