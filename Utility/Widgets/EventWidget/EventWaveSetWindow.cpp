/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/2
 **/

#include "EventWaveSetWindow.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QGridLayout>
#include "EventDataSymbol.h"
#include "EventWindow.h"

EventWaveSetWindow *EventWaveSetWindow::selfObj = NULL;

class EventWaveSetWindowPrivate
{
public:
    EventWaveSetWindowPrivate()
        : gainCbx(NULL), speedCbx(NULL)
    {}

    ComboBox *gainCbx;
    ComboBox *speedCbx;
};

EventWaveSetWindow::~EventWaveSetWindow()
{
}

void EventWaveSetWindow::waveGainReleased(int index)
{
    EventWindow::getInstance()->setWaveGain(index);
}

void EventWaveSetWindow::waveSpeedReleased(int index)
{
    EventWindow::getInstance()->setWaveSpeed(index);
}

EventWaveSetWindow::EventWaveSetWindow()
    : Dialog(), d_ptr(new EventWaveSetWindowPrivate())
{
    setFixedSize(340, 240);

    setWindowTitle(trs("Set"));

    QGridLayout *layout = new QGridLayout();

    QLabel *label = new QLabel(trs("Gain"));
    layout->addWidget(label, 0, 0);
    d_ptr->gainCbx = new ComboBox();
    for (int i = 0; i < ECG_EVENT_GAIN_NR; i ++)
    {
        d_ptr->gainCbx->addItem(EventDataSymbol::convert((ECGEventGain)i));
    }
    d_ptr->gainCbx->setCurrentIndex(3);
    layout->addWidget(d_ptr->gainCbx, 0, 1);
    connect(d_ptr->gainCbx, SIGNAL(currentIndexChanged(int)), this, SLOT(waveGainReleased(int)));

    label = new QLabel(trs("Speed"));
    layout->addWidget(label, 1, 0);
    d_ptr->speedCbx = new ComboBox();
    d_ptr->speedCbx->addItems(QStringList()
                              << "6.25mm/s"
                              << "12.5mm/s"
                              << "25mm/s"
                              << "50mm/s");
    d_ptr->speedCbx->setCurrentIndex(1);
    layout->addWidget(d_ptr->speedCbx, 1, 1);
    connect(d_ptr->speedCbx, SIGNAL(currentIndexChanged(int)), this, SLOT(waveSpeedReleased(int)));

    setWindowLayout(layout);
}
