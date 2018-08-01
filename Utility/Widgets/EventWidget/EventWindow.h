/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/1
 **/

#pragma once
#include "Window.h"
#include <QScopedPointer>

class EventWindowPrivate;
class EventWindow : public Window
{
    Q_OBJECT
public:
    static EventWindow &construction()
    {
        if (NULL == selfObj)
        {
            selfObj = new EventWindow();
        }
        return *selfObj;
    }
    ~EventWindow();

protected:
    void showEvent(QShowEvent *ev);

private:
    EventWindow();
    static EventWindow *selfObj;
    QScopedPointer<EventWindowPrivate> d_ptr;
};
#define eventWindow             (EventWindow::construction())
#define deleteEventWindow       (delete EventWindow::selfObj)
