/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/6
 **/

#include "OxyCRGEventSetWindow.h"
#include "ComboBox.h"
#include "LanguageManager.h"
#include <QGridLayout>
#include <QLabel>
#include "OxyCRGEventWindow.h"
#include "ParamDefine.h"

OxyCRGEventSetWindow *OxyCRGEventSetWindow::selfObj = NULL;

class OxyCRGEventSetWindowPrivate
{
public:
  OxyCRGEventSetWindowPrivate()
    : trendCbo(NULL), compressedWaveCbo(NULL)
  {}

  ComboBox *trendCbo;
  ComboBox *compressedWaveCbo;
};

OxyCRGEventSetWindow::~OxyCRGEventSetWindow()
{
}

void OxyCRGEventSetWindow::trend1Slot(int index)
{
    bool isRR;
    if (index == 0)
    {
        isRR = false;
    }
    else
    {
        isRR = true;
    }
    OxyCRGEventWindow::getInstance()->waveWidgetTrend1(isRR);
}

void OxyCRGEventSetWindow::compressedSlot(int index)
{
    WaveformID waveID;
    if (index == 0)
    {
        waveID = WAVE_RESP;
    }
    else
    {
        waveID = WAVE_CO2;
    }
    OxyCRGEventWindow::getInstance()->waveWidgetCompressed(waveID);
}

OxyCRGEventSetWindow::OxyCRGEventSetWindow()
    : Dialog(), d_ptr(new OxyCRGEventSetWindowPrivate())
{
    setWindowTitle(trs("Set"));

    QGridLayout *layout = new QGridLayout();

    QLabel *label = new QLabel(trs("Trend"));
    layout->addWidget(label, 0, 0);
    d_ptr->trendCbo = new ComboBox();
    d_ptr->trendCbo->addItem("HR");
    d_ptr->trendCbo->addItem("HR+RR");
    layout->addWidget(d_ptr->trendCbo, 0, 1);
    connect(d_ptr->trendCbo, SIGNAL(currentIndexChanged(int)), this, SLOT(trend1Slot(int)));

    label = new QLabel(trs("CompressedWave"));
    layout->addWidget(label, 1, 0);
    d_ptr->compressedWaveCbo = new ComboBox();
    d_ptr->compressedWaveCbo->addItem("Resp Waveform");
    d_ptr->compressedWaveCbo->addItem("CO2 Waveform");
    layout->addWidget(d_ptr->compressedWaveCbo, 1, 1);
    connect(d_ptr->compressedWaveCbo, SIGNAL(currentIndexChanged(int)), this, SLOT(compressedSlot(int)));

    setWindowLayout(layout);
}
