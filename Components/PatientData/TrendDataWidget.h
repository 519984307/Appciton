#pragma once
#include "PopupWidget.h"
#include "ParamInfo.h"
#include "IBPDefine.h"
#include "TrendDataSymbol.h"
#include "TrendDataDefine.h"
#include "IMoveButton.h"

class ITableWidget;
class IButton;
class IDropList;
class FreezeTableWidget;
class TrendDataWidget : public PopupWidget
{
    Q_OBJECT

public:
    static TrendDataWidget &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new TrendDataWidget();
        }

        return *_selfObj;
    }

    static TrendDataWidget *_selfObj;
    ~TrendDataWidget();

    ITableWidget *table;

public:
    void isIBPSubParamVisible(IBPPressureName, bool);

    void loadTrendData(void);

    void loadCurParam(int index = 0);

    void setTimeInterval(ResolutionRatio);

    void printTrendData(unsigned startTime, unsigned endTime);

    /**
     * @brief setHistoryDataPath 设置历史回顾数据的文件路径
     * @param path 文件路径
     */
    void setHistoryDataPath(QString path);

    /**
     * @brief setHistoryData 此刻的趋势表是否为历史数据
     * @param flag  标志
     */
    void setHistoryData(bool flag);

protected:
    void showEvent(QShowEvent *event);

private slots:
    void _trendDataSetReleased();
    void _upReleased(void);
    void _downReleased(void);
    void _leftReleased(void);
    void _rightReleased(void);
    void _printWidgetRelease();
    void _leftMoveEvent(void);
    void _rightMoveEvent(void);

private:
    TrendDataWidget();
    void _loadTableTitle(void);
    void _trendParamInit();
    void _getTrendData();

    void _dataIndex(int &startIndex, int &endIndex);
    void _updateDisplayTime();

    void _updateEventIndex();

private:
    int _curVScroller;

    IButton *_up;
    IButton *_down;
    IButton *_left;
    IButton *_right;
    IMoveButton *_incidentMove;
    IDropList *_incident;
    IDropList *_printParam;
    IButton *_set;

    int _subParamCount;                    // total sub param than need to display in this widget

    typedef QMap<ParamID, SubParamID> TrendParamMap;
    QMultiMap<ParamID, SubParamID> _orderMap;            // 参数列表

    QMap<IBPPressureName, bool> _ibpNameMap;                // IBP压力标名对应是否开启

    typedef QList<SubParamID> TrendParamList;
    QList<SubParamID> _curList;              // 当前列表
    QList<SubParamID> _displayList;             // 显示列表

    QList<TrendDataPackage *> _trendDataPack;

    QString _curDate;
    int _curDisplayParamRow;

    int _maxWidth;
    int _maxHeight;

    ResolutionRatio _timeInterval;          // 时间间隔

    unsigned _startTime;                    // 打印的开始时间
    unsigned _endTime;                      // 打印的结束时间

    unsigned _leftTime;                     // 趋势表左侧时间
    unsigned _rightTime;                    // 趋势表右侧时间
    int _totalCol;                          // 总列数
    int _curSecCol;                         // 当前选中列
    QList<int> _eventList;                  // 表格数据是否存在事件发生
    int _curSecIndex;                       // 当前选中索引
    int _maxDataNum;                        // 趋势表最大数据量

    bool _isHistory;                        // 历史回顾标志
    QString _historyDataPath;               // 历史数据路径
};
#define trendDataWidget (TrendDataWidget::construction())
#define deleteTrendDataWidget() (delete TrendDataWidget::_selfObj)
