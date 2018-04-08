#pragma once
#include "PopupWidget.h"
#include "ParamInfo.h"
#include "IBPDefine.h"
#include "TrendDataSymbol.h"
#include "TrendDataDefine.h"

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
    QLabel *label;

public:
    void isIBPSubParamVisible(IBPPressureName, bool);

    void loadDemoData(void);
    void loadTrendData(void);

    void loadCurParam(int index = 0);

    void setTimeInterval(ResolutionRatio);

protected:
    void showEvent(QShowEvent *event);

private slots:
    void _trendDataSetReleased();
    void _upReleased(void);
    void _downReleased(void);
    void _leftReleased(void);
    void _rightReleased(void);

private:
    TrendDataWidget();
    void _loadTableTitle(void);
    void _trendParamInit();
    void _updateHeaderDate(unsigned t = 0);
    void _getTrendData();

private:
    int _curVScroller;

    IButton *_up;
    IButton *_down;
    IButton *_left;
    IButton *_right;
    IButton *_incidentMove;
    IDropList *_incident;
    IDropList *_printParam;
    IButton *_set;

    int _subParamCount;                    // total sub param than need to display in this widget

    typedef QMap<ParamID, SubParamID> TrendParamMap;
    QMultiMap<ParamID, SubParamID> _orderMap;            // 参数列表

    QMap<IBPPressureName, bool> _ibpNameMap;                // IBP压力标名对应是否开启
    QMap<SubParamID, IBPPressureName> _ibpSubParamMap;      // 找到压力标名对应的字参数ID

    typedef QList<SubParamID> TrendParamList;
    QList<SubParamID> _curList;              // 当前列表
    QList<SubParamID> _displayList;             // 显示列表

    QList<TrendDataPackage *> _trendDataPack;

    QString _curDate;
    int _curDisplayParamRow;

    int _maxWidth;
    int _maxHeight;

    ResolutionRatio _timeInterval;          // 时间间隔
    int _currentMoveCount;                  // 当前数据移动数目；
    int _hideColumn;
};
#define trendDataWidget (TrendDataWidget::construction())
#define deleteTrendDataWidget() (delete TrendDataWidget::_selfObj)
