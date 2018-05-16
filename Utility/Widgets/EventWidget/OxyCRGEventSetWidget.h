#pragma once
#include "PopupWidget.h"

class IComboList;
class OxyCRGEventSetWidget : public PopupWidget
{
    Q_OBJECT
public:
    static  OxyCRGEventSetWidget &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new OxyCRGEventSetWidget();
        }
        return *_selfObj;
    }
    static OxyCRGEventSetWidget *_selfObj;
    ~OxyCRGEventSetWidget();

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _trend1Slot(int);
    void _compressedSlot(int);

private:
    OxyCRGEventSetWidget();

    IComboList *_trend1;
    IComboList *_compressed;
};
#define oxyCRGEventSetWidget (OxyCRGEventSetWidget::construction())
#define deleteOxyCRGEventSetWidget() (delete OxyCRGEventSetWidget::_selfObj)
