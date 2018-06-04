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
    /**
     * @brief setWidgetSize 设置波形区窗口的宽度和高度
     * @param w 宽度
     * @param h 高度
     */
    void setWidgetSize(int w, int h);

    /**
     * @brief pageUpParam 上翻页
     */
    void pageUpParam();

    /**
     * @brief pageDownParam 下翻页
     */
    void pageDownParam();

    /**
     * @brief setTimeInterval 设置时间间隔
     * @param timeInterval 时间间隔
     */
    void setTimeInterval(ResolutionRatio timeInterval);

    /**
     * @brief setWaveNumber 设置显示波形数
     * @param num 波形数
     */
    void setWaveNumber(int num);

    /**
     * @brief setRulerLimit 设置子窗口上下标尺限
     * @param down 下标尺限
     * @param up 上标尺限
     */
    void setRulerLimit(SubParamID, int down, int up);

    /**
     * @brief loadTrendData 根据子参数ID将一页趋势数据载入容器
     * @param id 子参数ID
     * @param startIndex 开始数据的索引
     * @param endIndex 结束数据的索引
     */
    void loadTrendData(SubParamID id, const int startIndex, const int endIndex);

    /**
     * @brief dataIndex 获取开始与结束数据的索引
     * @param startIndex 开始数据的索引
     * @param endIndex 结束数据的索引
     */
    void dataIndex(int &startIndex, int &endIndex);

    /**
     * @brief updateTimeRange 根据当前数据页和时间间隔更新显示数据时间范围
     */
    void updateTimeRange(void);

    //test
    const QList<TrendDataPackage*> getTrendDatapacketList() {return _trendDataPack;}

    const QList<TrendGraphInfo> &getTrendGraphPrint(){return _infosList;}

public slots:
    /**
     * @brief leftMoveCoordinate 左移坐标
     */
    void leftMoveCoordinate();

    /**
     * @brief rightMoveCoordinate 右移坐标
     */
    void rightMoveCoordinate();

    /**
     * @brief leftMoveCursor 左移游标
     */
    void leftMoveCursor();

    /**
     * @brief rightMoveCursor 右移游标
     */
    void rightMoveCursor();

    /**
     * @brief leftMoveEvent 左移事件
     */
    void leftMoveEvent();

    /**
     * @brief rightMoveEvent 右移事件
     */
    void rightMoveEvent();

protected:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent *e);

private:
    /**
     * @brief _trendLayout 将趋势数据加载入子窗口
     */
    void _trendLayout(void);

    /**
     * @brief _getTrendData 获取趋势图数据
     */
    void _getTrendData(void);

    /**
     * @brief _initWaveSubWidget 初始化一次波形窗口中子窗口控件
     */
    void _initWaveSubWidget(void);

    /**
     * @brief _getCursorPos 根据此时游标索引的时间获取在窗口的像素位置
     * @param t 游标索引的时间
     * @return 像素位置
     */
    double _getCursorPos(unsigned t);               // 获取坐标位置

    /**
     * @brief _calculationPage 获取当前时间间隔的总数据页并设置当前页为1
     */
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
    QList<unsigned> _alarmTimeList;                 // 报警发生时间
    QList<TrendGraphInfo> _infosList;                    // 打印趋势图数据链表
};
