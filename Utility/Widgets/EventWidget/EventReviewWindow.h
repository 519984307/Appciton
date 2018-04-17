#pragma once
#include "PopupWidget.h"
#include <QScopedPointer>
#include <QStackedLayout>
#include "EventDataDefine.h"

class EventReviewWindowPrivate;
class EventReviewWindow : public PopupWidget
{
    Q_OBJECT
public:
    EventReviewWindow();
    ~EventReviewWindow();

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _waveInfoReleased(void);
    void _eventListReleased(void);

private:
    void _loadEventData(void);

private:
    QScopedPointer<EventReviewWindowPrivate> d_ptr;
};
