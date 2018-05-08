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

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _upMoveEventReleased(void);
    void _downMoveEventReleased(void);

private:
    OxyCRGEventWidget();
    QScopedPointer<OxyCRGEventWidgetPrivate> d_ptr;
};
#define oxyCRGEventWidget (OxyCRGEventWidget::construction())
#define deleteOxyCRGEventWidget() (delete OxyCRGEventWidget::_selfObj)