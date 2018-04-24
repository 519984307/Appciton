#pragma once
#include "PopupWidget.h"
#include <QScopedPointer>
#include <QStackedLayout>
#include "EventDataDefine.h"
#include "Alarm.h"

class EventReviewWindowPrivate;
class EventReviewWindow : public PopupWidget
{
    Q_OBJECT
public:
    EventReviewWindow();
    ~EventReviewWindow();

    void eventInfoUpdate(void);
    void eventTrendUpdate(void);
    void eventWaveUpdate(void);

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _waveInfoReleased(void);
    void _eventListReleased(void);
    void _upMoveEventReleased(void);
    void _downMoveEventReleased(void);
    void _eventTypeSelect(int);
    void _eventLevelSelect(int);

private:
    void _loadEventData(void);
    AlarmPriority _levelToPriority(EventLevel);

private:
    QScopedPointer<EventReviewWindowPrivate> d_ptr;
};
