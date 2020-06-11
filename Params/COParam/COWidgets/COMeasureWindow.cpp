/**
 ** This file is part of the Project project. ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/10
 **/

#include "COMeasureWindow.h"
#include "Framework/UI/ThemeManager.h"
#include "Framework/Language/LanguageManager.h"
#include "Framework/UI/Button.h"
#include "COMeasureResultWidget.h"
#include "COMeasureWidget.h"
#include "ColorManager.h"
#include <QGridLayout>
#include <QLabel>
#include <QDateTime>
#include <QBoxLayout>
#include "COParam.h"
#include "FontManager.h"

/* support 6 measure reuslt at most */
#define MAX_MEASURE_RESULT_NUM   6


static short tbDemoWave[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03,
    0x03, 0x04, 0x06, 0x06, 0x07, 0x09, 0x09, 0x0b, 0x0c, 0x0d, 0x0f, 0x0f, 0x11, 0x12, 0x13, 0x15,
    0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x27, 0x29, 0x2b, 0x2d, 0x30, 0x32, 0x34, 0x37,
    0x39, 0x3c, 0x3e, 0x3f, 0x42, 0x43, 0x45, 0x47, 0x49, 0x4b, 0x4d, 0x4e, 0x51, 0x52, 0x54, 0x55,
    0x57, 0x5a, 0x5a, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x60, 0x60, 0x61, 0x62, 0x62, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x62, 0x62, 0x62, 0x61, 0x60, 0x60, 0x60, 0x60, 0x5f, 0x5e,
    0x5e, 0x5d, 0x5c, 0x5b, 0x5a, 0x59, 0x58, 0x57, 0x56, 0x54, 0x54, 0x52, 0x51, 0x50, 0x4e, 0x4e,
    0x4c, 0x4b, 0x4b, 0x4a, 0x49, 0x48, 0x47, 0x46, 0x45, 0x45, 0x44, 0x43, 0x42, 0x41, 0x40, 0x3f,
    0x3e, 0x3e, 0x3c, 0x3c, 0x3b, 0x3a, 0x39, 0x39, 0x37, 0x36, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31,
    0x30, 0x30, 0x2f, 0x2e, 0x2d, 0x2d, 0x2c, 0x2b, 0x2a, 0x2a, 0x2a, 0x29, 0x28, 0x27, 0x27, 0x26,
    0x25, 0x24, 0x24, 0x23, 0x22, 0x21, 0x21, 0x20, 0x1f, 0x1e, 0x1e, 0x1e, 0x1d, 0x1c, 0x1b, 0x1b,
    0x1b, 0x1a, 0x19, 0x19, 0x18, 0x18, 0x17, 0x17, 0x16, 0x15, 0x15, 0x15, 0x14, 0x14, 0x13, 0x13,
    0x12, 0x12, 0x12, 0x12, 0x11, 0x11, 0x10, 0x10, 0x10, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0e, 0x0e,
    0x0e, 0x0d, 0x0d, 0x0d, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0b, 0x0b, 0x0b, 0x0a, 0x0a, 0x09,
    0x09, 0x09, 0x09, 0x09, 0x09, 0x08, 0x08, 0x08, 0x07, 0x07, 0x07, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x05, 0x05, 0x05, 0x05, 0x04, 0x04, 0x04, 0x04, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

class COMeasureWindowPrivate
{
public:
    COMeasureWindowPrivate()
        : ctrlBtn(NULL), settingBtn(NULL), saveBtn(NULL), printBtn(NULL),
          calcBtn(NULL), measureWidget(NULL), coAvgLabel(NULL), coAvgVal(NULL),
          ciAvgLabel(NULL), ciAvgVal(NULL)
    {
        for (int i = 0; i < MAX_MEASURE_RESULT_NUM; i++)
        {
            resultWidget[i] = NULL;
        }
    }

    Button *ctrlBtn;
    Button *settingBtn;
    Button *saveBtn;
    Button *printBtn;
    Button *calcBtn;
    COMeasureWidget *measureWidget;
    COMeasureResultWidget *resultWidget[MAX_MEASURE_RESULT_NUM];
    QLabel *coAvgLabel;
    QLabel *coAvgVal;
    QLabel *ciAvgLabel;
    QLabel *ciAvgVal;
};

COMeasureWindow::COMeasureWindow()
    : Dialog(), pimpl(new COMeasureWindowPrivate())
{
    setFixedSize(themeManager.defaultWindowSize());
    setWindowTitle(trs("COMeasurement"));

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(4, 2, 4, 2);
    setWindowLayout(layout);

    pimpl->measureWidget =  new COMeasureWidget();
    layout->addWidget(pimpl->measureWidget, 0, 0, 2, 3);

    pimpl->ctrlBtn = new Button(trs("Start"));
    pimpl->ctrlBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(pimpl->ctrlBtn, 0, 3, 1, 1, Qt::AlignVCenter);

    pimpl->settingBtn = new Button(trs("Setting"));
    pimpl->settingBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(pimpl->settingBtn, 1, 3, 1, 1, Qt::AlignVCenter);

    int resultOnEachRow  = MAX_MEASURE_RESULT_NUM / 2;
    QColor color = colorManager.getColor(paramInfo.getParamName(PARAM_CO));
    for (int i = 0; i < MAX_MEASURE_RESULT_NUM; i++)
    {
        pimpl->resultWidget[i] = new COMeasureResultWidget();
        QPalette pal = pimpl->resultWidget[i]->palette();
        pal.setColor(QPalette::WindowText, color);
        pimpl->resultWidget[i]->setPalette(pal);
        layout->addWidget(pimpl->resultWidget[i], 2 + i / resultOnEachRow, i % resultOnEachRow, 1, 1);
    }

    QBoxLayout *coLayout = new QHBoxLayout();
    pimpl->coAvgLabel = new QLabel("C.O. Avg.\nL/min");
    pimpl->coAvgLabel->setFont(fontManager.textFont(fontManager.getFontSize(2)));
    coLayout->addWidget(pimpl->coAvgLabel);
    pimpl->coAvgVal = new QLabel(InvStr());
    pimpl->coAvgVal->setAlignment(Qt::AlignCenter);
    pimpl->coAvgVal->setFont(fontManager.textFont(fontManager.getFontSize(5)));
    coLayout->addWidget(pimpl->coAvgVal);
    layout->addLayout(coLayout, 2, 3);

    QBoxLayout *ciLayout = new QHBoxLayout();
    pimpl->ciAvgLabel = new QLabel(QString::fromUtf8("C.I. Avg.\nL/min/m²"));
    pimpl->ciAvgLabel->setFont(fontManager.textFont(fontManager.getFontSize(2)));
    ciLayout->addWidget(pimpl->ciAvgLabel);
    pimpl->ciAvgVal = new QLabel(InvStr());
    pimpl->ciAvgVal->setAlignment(Qt::AlignCenter);
    pimpl->ciAvgVal->setFont(fontManager.textFont(fontManager.getFontSize(5)));
    ciLayout->addWidget(pimpl->ciAvgVal);
    layout->addLayout(ciLayout, 3, 3);

    QBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch(1);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    pimpl->saveBtn = new Button(trs("Save"));
    pimpl->saveBtn->setButtonStyle(Button::ButtonTextOnly);
    btnLayout->addWidget(pimpl->saveBtn, 2);
    pimpl->printBtn = new Button(trs("Print"));
    pimpl->printBtn->setButtonStyle(Button::ButtonTextOnly);
    btnLayout->addWidget(pimpl->printBtn, 2);
    pimpl->calcBtn = new Button(trs("Calculate"));
    pimpl->calcBtn->setButtonStyle(Button::ButtonTextOnly);
    btnLayout->addWidget(pimpl->calcBtn, 2);
    btnLayout->addStretch(1);

    layout->addLayout(btnLayout, 4, 0, 1, 4);

    pimpl->saveBtn->setEnabled(false);
    pimpl->calcBtn->setEnabled(false);
    pimpl->printBtn->setEnabled(false);
}

COMeasureWindow::~COMeasureWindow()
{
}

void COMeasureWindow::setMeasureResult(short co, short ci)
{
}

void COMeasureWindow::setTb(short tb)
{
}

void COMeasureWindow::addMeasureWaveData(short wave)
{
}

void COMeasureWindow::showEvent(QShowEvent *ev)
{
    COMeasureData d;
    d.timestamp = QDateTime::currentDateTime().toTime_t();
    d.ci = 15;
    d.co = 38;
    d.dataRate = 25;
    for (int i = 0; i < sizeof(tbDemoWave) / sizeof(tbDemoWave[0]); ++i)
    {
        d.measureWave.append(tbDemoWave[i]);
    }

    pimpl->resultWidget[0]->setMeasureData(d);
    pimpl->resultWidget[0]->setChecked(true);
}
