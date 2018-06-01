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
    IMoveButton *_moveCoordinateBtn;
    IMoveButton *_moveCursorBtn;
    IMoveButton *_moveEventBtn;
    IButton *_printBtn;
    IButton *_setBtn;
    IButton *_upBtn;
    IButton *_downBtn;

    int _maxWidth;
    int _maxHeight;
};
#define trendGraphWidget (TrendGraphWidget::construction())
#define deleteTrendGraphWidget() (delete TrendGraphWidget::_selfObj)
