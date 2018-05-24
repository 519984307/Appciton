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

    void pageUpParam();
    void pageDownParam();

    void updateTrendGraph();
    void setTimeInterval(ResolutionRatio timeInterval);
    void setWaveNumber(int num);
    void setRulerLimit(SubParamID, int down, int up);

    void loadTrendData(SubParamID);

    //test purpose
    const QList<TrendDataPackage *> &getTrendDatapacketList() {return _trendDataPack;}

protected:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent *e);

private:
    void _trendLayout(void);
    void _clearLayout(void);
    void _getTrendData(void);

private:
    QVBoxLayout *_mainLayout;
    QVBoxLayout *_hLayoutTrend;

    ResolutionRatio _timeInterval;
    int _waveRegionWidth;
    int _oneFrameWidth;

    unsigned _initTime;
    int _cursorPosition;
    unsigned _currentCursorTime;

    IWidget *_subWidget;
    QScrollArea *_subWidgetScrollArea;

    int _displayGraphNum;                           // 一屏显示的波形数
    int _totalGraphNum;                             // 趋势组显示的总波形数
    int _curVScroller;

    QList<TrendDataPackage *> _trendDataPack;       // 趋势数据包
    int *_dataBuf;
    int _dataSize;
    int _currentDisplayNum;                         // 当前显示数据量
    int _totalPage;                                 // 总数据页数
    int _currentPage;                               // 当前页数
};
