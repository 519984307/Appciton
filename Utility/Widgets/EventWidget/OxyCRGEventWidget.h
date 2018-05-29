#pragma once
#include "PopupWidget.h"

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

    void waveWidgetTrend1(bool isRR);
    void waveWidgetCompressed(bool isCO2);

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
