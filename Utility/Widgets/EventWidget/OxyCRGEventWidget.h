#pragma once
#include "PopupWidget.h"
#include "ParamDefine.h"

class OxyCRGEventWidgetPrivate;
class OxyCRGEventWidget : public PopupWidget
{
    Q_OBJECT
public:
    static OxyCRGEventWidget &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new OxyCRGEventWidget();
        }
        return *_selfObj;
    }
    static OxyCRGEventWidget *_selfObj;
    ~OxyCRGEventWidget();

    void eventInfoUpdate(void);
    void eventWaveUpdate(void);
    void loadTrendData();
    void loadWaveformData();

    void waveWidgetTrend1(bool isRR);
    void waveWidgetCompressed(WaveformID id);

    /**
     * @brief setHistoryDataPath 设置历史回顾数据的文件路径
     * @param path 文件路径
     */
    void setHistoryDataPath(QString path);

    /**
     * @brief isHistoryData 此刻的趋势表是否为历史数据
     * @param flag 标志
     */
    void isHistoryData(bool flag);

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _upMoveEventReleased(void);
    void _downMoveEventReleased(void);
    void _detailReleased(void);
    void _eventListReleased(void);
    void _leftMoveEvent(void);
    void _rightMoveEvent(void);
    void _setReleased(void);
    void _printReleased(void);

private:
    void _loadOxyCRGEventData(void);

private:
    OxyCRGEventWidget();
    QScopedPointer<OxyCRGEventWidgetPrivate> d_ptr;
};
#define oxyCRGEventWidget (OxyCRGEventWidget::construction())
#define deleteOxyCRGEventWidget() (delete OxyCRGEventWidget::_selfObj)
