#pragma once
#include "PopupWidget.h"
#include "IMoveButton.h"
#include "ParamDefine.h"

class TrendWaveWidget;
class IButton;
class TrendGraphWidget : public PopupWidget
{
    Q_OBJECT

public:
    static TrendGraphWidget &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new TrendGraphWidget();
        }

        return *_selfObj;
    }

    static TrendGraphWidget *_selfObj;
    ~TrendGraphWidget();

public:
    void waveNumberChange(int num);
    void timeIntervalChange(int timeInterval);
    void upDateTrendGraph();
    void setSubWidgetRulerLimit(SubParamID, int, int);

private slots:
    void _leftMoveCoordinate();
    void _rightMoveCoordinate();
    void _leftMoveCursor();
    void _rightMoveCursor();
    void _leftMoveEvent();
    void _rightMoveEvent();
    void _trendGraphSetReleased();
    void _upReleased(void);
    void _downReleased(void);
    void _printReleased(void);

private:
    TrendGraphWidget();

    TrendWaveWidget *_waveWidget;
    IMoveButton *_moveCoordinate;
    IMoveButton *_moveCursor;
    IMoveButton *_moveEvent;
    IButton *_print;
    IButton *_set;
    IButton *_up;
    IButton *_down;

    int _maxWidth;
    int _maxHeight;
};
#define trendGraphWidget (TrendGraphWidget::construction())
#define deleteTrendGraphWidget() (delete TrendGraphWidget::_selfObj)
