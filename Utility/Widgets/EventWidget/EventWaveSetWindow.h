/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/2
 **/

#pragma once
#include "Framework/UI/Dialog.h"
#include <QScopedPointer>

class EventWaveSetWindowPrivate;
class EventWaveSetWindow : public Dialog
{
    Q_OBJECT
public:
    static EventWaveSetWindow &construction()
    {
        if (selfObj == NULL)
        {
            selfObj = new EventWaveSetWindow();
        }
        return *selfObj;
    }
    ~EventWaveSetWindow();

private slots:
    void waveGainReleased(int);
    void waveSpeedReleased(int);

private:
    EventWaveSetWindow();
    static EventWaveSetWindow *selfObj;
    QScopedPointer<EventWaveSetWindowPrivate> d_ptr;
};
#define eventWaveSetWindow              (EventWaveSetWindow::construction())
#define deleteEventWaveSetWindow        (delete EventWaveSetWindow::selfObj)

