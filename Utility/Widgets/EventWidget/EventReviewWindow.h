#pragma once
#include "PopupWidget.h"
#include <QScopedPointer>
#include "EventDataDefine.h"

class EventReviewWindowPrivate;
class EventReviewWindow : public PopupWidget
{
    Q_OBJECT
public:
    EventReviewWindow();
    ~EventReviewWindow();

private:
    QScopedPointer<EventReviewWindowPrivate> d_ptr;
};
