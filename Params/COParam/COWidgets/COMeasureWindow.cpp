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
#include <QGridLayout>
#include <QLabel>
#include <QBoxLayout>
#include "COParam.h"
#include "FontManager.h"

/* support 6 measure reuslt at most */
#define MAX_MEASURE_RESULT_NUM   6

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
    for (int i = 0; i < MAX_MEASURE_RESULT_NUM; i++)
    {
        pimpl->resultWidget[i] = new COMeasureResultWidget();
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
