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
    /**
     * @brief waveNumberChange set waveform number
     * @param num display waveform number
     */
    void waveNumberChange(int num);

    /**
     * @brief timeIntervalChange set waveform time interval
     * @param timeInterval time interval
     */
    void timeIntervalChange(int timeInterval);


    /**
     * @brief updateTrendGraph  update trend graph
     */
    void updateTrendGraph();

    /**
     * @brief setSubWidgetRulerLimit set subwaveform widget ruler up and down limit
     * @param id subwaveform widget subparam id
     * @param down down limit
     * @param up up limit
     */
    void setSubWidgetRulerLimit(SubParamID id, int down, int up);

private slots:
    /**
     * @brief _leftMoveCoordinate left move coordinate
     */
    void _leftMoveCoordinate();

    /**
     * @brief _rightMoveCoordinate right move coordinate
     */
    void _rightMoveCoordinate();

    /**
     * @brief _leftMoveCursor left move cursor
     */
    void _leftMoveCursor();

    /**
     * @brief _rightMoveCursor right move cursor
     */
    void _rightMoveCursor();

    /**
     * @brief _leftMoveEvent left move event
     */
    void _leftMoveEvent();

    /**
     * @brief _rightMoveEvent right move event
     */
    void _rightMoveEvent();

    /**
     * @brief _trendGraphSetReleased open trend graph set widget slot funtion
     */
    void _trendGraphSetReleased();

    /**
     * @brief _upReleased page up slot funtion
     */
    void _upReleased(void);

    /**
     * @brief _downReleased page down slot funtion
     */
    void _downReleased(void);

    /**
     * @brief _printReleased print slot funtion
     */
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
