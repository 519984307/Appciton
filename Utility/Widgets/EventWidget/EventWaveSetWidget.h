#pragma once
#include "PopupWidget.h"

class IComboList;
class IButton;
class EventWaveSetWidget : public PopupWidget
{
    Q_OBJECT

public:
    static EventWaveSetWidget &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new EventWaveSetWidget();
        }
        return *_selfObj;
    }
    static EventWaveSetWidget *_selfObj;
    ~EventWaveSetWidget();

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _waveGainReleased(int);
    void _waveSpeedReleased(int);

private:
    EventWaveSetWidget();

    IComboList *_gain;
    IComboList *_speed;
};
#define eventWaveSetWidget (EventWaveSetWidget::construction())
#define deleteEventWaveSetWidget() (delete EventWaveSetWidget::_selfObj)
