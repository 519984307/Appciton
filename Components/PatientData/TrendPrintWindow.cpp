/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/26
 **/

#include "TrendPrintWindow.h"
#include "LanguageManager.h"
#include <QLabel>
#include <QGroupBox>
#include "Button.h"
#include "SpinBox.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "TimeDate.h"
#include <QDateTime>
#include "TrendTableWindow.h"

class TrendPrintWindowPrivate
{
public:
    struct SubGroupBox
    {
        QLabel *dateLbl;
        QLabel *timeLbl;
        SpinBox *yearSbx;
        SpinBox *monthSbx;
        SpinBox *daySbx;
        SpinBox *hourSbx;
        SpinBox *minSbx;
        SpinBox *secondSbx;
    };

    TrendPrintWindowPrivate()
        : startBox(NULL), endBox(NULL), startSubBox(NULL),
          endSubBox(NULL), durationLbl(NULL), printBtn(NULL),
          timeStartLimit(0), timeEndLimit(0), printStartTime(0),
          printEndTime(0)
    {}

    void initGroupBox(QGroupBox *groupBox, SubGroupBox *subBox);
    void difftimeInfo(void);
public:
    QGroupBox *startBox;
    QGroupBox *endBox;
    SubGroupBox *startSubBox;
    SubGroupBox *endSubBox;
    QLabel *durationLbl;
    Button *printBtn;

    unsigned timeStartLimit;
    unsigned timeEndLimit;
    unsigned printStartTime;
    unsigned printEndTime;
};
TrendPrintWindow::TrendPrintWindow()
    : Window(), d_ptr(new TrendPrintWindowPrivate())
{
    setWindowTitle(trs("PrintSetup"));

    QPalette pal = palette();
    d_ptr->startBox = new QGroupBox();
    d_ptr->startBox->setPalette(pal);
    d_ptr->startBox->setTitle(trs("StartTime"));
    d_ptr->startSubBox = new TrendPrintWindowPrivate::SubGroupBox();
    d_ptr->initGroupBox(d_ptr->startBox, d_ptr->startSubBox);
    connect(d_ptr->startSubBox->yearSbx, SIGNAL(valueChange(int, int)),
            this, SLOT(startTimeChangeSlot(int, int)));
    connect(d_ptr->startSubBox->monthSbx, SIGNAL(valueChange(int, int)),
            this, SLOT(startTimeChangeSlot(int, int)));
    connect(d_ptr->startSubBox->daySbx, SIGNAL(valueChange(int, int)),
            this, SLOT(startTimeChangeSlot(int, int)));
    connect(d_ptr->startSubBox->hourSbx, SIGNAL(valueChange(int, int)),
            this, SLOT(startTimeChangeSlot(int, int)));
    connect(d_ptr->startSubBox->minSbx, SIGNAL(valueChange(int, int)),
            this, SLOT(startTimeChangeSlot(int, int)));
    connect(d_ptr->startSubBox->secondSbx, SIGNAL(valueChange(int, int)),
            this, SLOT(startTimeChangeSlot(int, int)));

    d_ptr->endBox = new QGroupBox();
    d_ptr->endBox->setPalette(pal);
    d_ptr->endBox->setTitle(trs("EndTime"));
    d_ptr->endSubBox = new TrendPrintWindowPrivate::SubGroupBox();
    d_ptr->initGroupBox(d_ptr->endBox, d_ptr->endSubBox);
    connect(d_ptr->endSubBox->yearSbx, SIGNAL(valueChange(int, int)),
            this, SLOT(endTimeChangeSlot(int, int)));
    connect(d_ptr->endSubBox->monthSbx, SIGNAL(valueChange(int, int)),
            this, SLOT(endTimeChangeSlot(QString, int)));
    connect(d_ptr->endSubBox->daySbx, SIGNAL(valueChange(int, int)),
            this, SLOT(endTimeChangeSlot(int, int)));
    connect(d_ptr->endSubBox->hourSbx, SIGNAL(valueChange(int, int)),
            this, SLOT(endTimeChangeSlot(int, int)));
    connect(d_ptr->endSubBox->minSbx, SIGNAL(valueChange(int, int)),
            this, SLOT(endTimeChangeSlot(int, int)));
    connect(d_ptr->endSubBox->secondSbx, SIGNAL(valueChange(int, int)),
            this, SLOT(endTimeChangeSlot(int, int)));

    d_ptr->durationLbl = new QLabel();

    d_ptr->printBtn = new Button(trs("Print"));
    d_ptr->printBtn->setFixedWidth(100);
    d_ptr->printBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->printBtn, SIGNAL(released()), this, SLOT(printReleased()));

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(d_ptr->durationLbl);
    hLayout->addWidget(d_ptr->printBtn);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(d_ptr->startBox);
    layout->addWidget(d_ptr->endBox);
    layout->addStretch();
    layout->addLayout(hLayout);
    layout->addStretch();

    setWindowLayout(layout);
}

TrendPrintWindow::~TrendPrintWindow()
{
}

void TrendPrintWindow::initPrintTime(unsigned start, unsigned end)
{
    d_ptr->printStartTime = start;
    d_ptr->printEndTime = end;

    d_ptr->startSubBox->yearSbx->setValue(static_cast<int>(timeDate.getDateYear(start)));
    d_ptr->startSubBox->monthSbx->setValue(static_cast<int>(timeDate.getDateMonth(start)));
    d_ptr->startSubBox->daySbx->setValue(static_cast<int>(timeDate.getDateDay(start)));
    d_ptr->startSubBox->hourSbx->setValue(static_cast<int>(timeDate.getTimeHour(start)));
    d_ptr->startSubBox->minSbx->setValue(static_cast<int>(timeDate.getTimeMinute(start)));
    d_ptr->startSubBox->secondSbx->setValue(static_cast<int>(timeDate.getTimeSenonds(start)));

    d_ptr->endSubBox->yearSbx->setValue(static_cast<int>(timeDate.getDateYear(end)));
    d_ptr->endSubBox->monthSbx->setValue(static_cast<int>(timeDate.getDateMonth(end)));
    d_ptr->endSubBox->daySbx->setValue(static_cast<int>(timeDate.getDateDay(end)));
    d_ptr->endSubBox->hourSbx->setValue(static_cast<int>(timeDate.getTimeHour(end)));
    d_ptr->endSubBox->minSbx->setValue(static_cast<int>(timeDate.getTimeMinute(end)));
    d_ptr->endSubBox->secondSbx->setValue(static_cast<int>(timeDate.getTimeSenonds(end)));

    d_ptr->difftimeInfo();
}

void TrendPrintWindow::printTimeRange(unsigned startLimit, unsigned endLimit)
{
    d_ptr->timeStartLimit = startLimit;
    d_ptr->timeEndLimit = endLimit;
}

void TrendPrintWindow::startTimeChangeSlot(int, int)
{
    QDate date(d_ptr->startSubBox->yearSbx->getValue(),
               d_ptr->startSubBox->monthSbx->getValue(),
               d_ptr->startSubBox->daySbx->getValue());
    QTime time(d_ptr->startSubBox->hourSbx->getValue(),
               d_ptr->startSubBox->minSbx->getValue(),
               d_ptr->startSubBox->secondSbx->getValue());
    QDateTime dateTime(date, time);
    unsigned timeStamp = dateTime.toTime_t();
    if (timeStamp < d_ptr->timeStartLimit)
    {
        d_ptr->startSubBox->yearSbx->setValue(static_cast<int>(timeDate.getDateYear(d_ptr->timeStartLimit)));
        d_ptr->startSubBox->monthSbx->setValue(static_cast<int>(timeDate.getDateMonth(d_ptr->timeStartLimit)));
        d_ptr->startSubBox->daySbx->setValue(static_cast<int>(timeDate.getDateDay(d_ptr->timeStartLimit)));
        d_ptr->startSubBox->hourSbx->setValue(static_cast<int>(timeDate.getTimeHour(d_ptr->timeStartLimit)));
        d_ptr->startSubBox->minSbx->setValue(static_cast<int>(timeDate.getTimeMinute(d_ptr->timeStartLimit)));
        d_ptr->startSubBox->secondSbx->setValue(static_cast<int>(timeDate.getTimeSenonds(d_ptr->timeStartLimit)));
        d_ptr->printStartTime = d_ptr->timeStartLimit;
    }
    else if (timeStamp > d_ptr->printEndTime)
    {
        d_ptr->startSubBox->yearSbx->setValue(static_cast<int>(timeDate.getDateYear(d_ptr->printEndTime)));
        d_ptr->startSubBox->monthSbx->setValue(static_cast<int>(timeDate.getDateMonth(d_ptr->printEndTime)));
        d_ptr->startSubBox->daySbx->setValue(static_cast<int>(timeDate.getDateDay(d_ptr->printEndTime)));
        d_ptr->startSubBox->hourSbx->setValue(static_cast<int>(timeDate.getTimeHour(d_ptr->printEndTime)));
        d_ptr->startSubBox->minSbx->setValue(static_cast<int>(timeDate.getTimeMinute(d_ptr->printEndTime)));
        d_ptr->startSubBox->secondSbx->setValue(static_cast<int>(timeDate.getTimeSenonds(d_ptr->printEndTime)));
        d_ptr->printStartTime = d_ptr->printEndTime;
    }
    else
    {
        d_ptr->printStartTime = timeStamp;
    }

    d_ptr->difftimeInfo();
}

void TrendPrintWindow::endTimeChangeSlot(int, int)
{
    QDate date(d_ptr->endSubBox->yearSbx->getValue(),
               d_ptr->endSubBox->monthSbx->getValue(),
               d_ptr->endSubBox->daySbx->getValue());
    QTime time(d_ptr->endSubBox->hourSbx->getValue(),
               d_ptr->endSubBox->minSbx->getValue(),
               d_ptr->endSubBox->secondSbx->getValue());
    QDateTime dateTime(date, time);
    unsigned timeStamp = dateTime.toTime_t();
    if (timeStamp > d_ptr->timeEndLimit)
    {
        d_ptr->endSubBox->yearSbx->setValue(static_cast<int>(timeDate.getDateYear(d_ptr->timeEndLimit)));
        d_ptr->endSubBox->monthSbx->setValue(static_cast<int>(timeDate.getDateMonth(d_ptr->timeEndLimit)));
        d_ptr->endSubBox->daySbx->setValue(static_cast<int>(timeDate.getDateDay(d_ptr->timeEndLimit)));
        d_ptr->endSubBox->hourSbx->setValue(static_cast<int>(timeDate.getTimeHour(d_ptr->timeEndLimit)));
        d_ptr->endSubBox->minSbx->setValue(static_cast<int>(timeDate.getTimeMinute(d_ptr->timeEndLimit)));
        d_ptr->endSubBox->secondSbx->setValue(static_cast<int>(timeDate.getTimeSenonds(d_ptr->timeEndLimit)));
        d_ptr->printEndTime = d_ptr->timeEndLimit;
    }
    else if (timeStamp < d_ptr->printStartTime)
    {
        d_ptr->endSubBox->yearSbx->setValue(static_cast<int>(timeDate.getDateYear(d_ptr->printStartTime)));
        d_ptr->endSubBox->monthSbx->setValue(static_cast<int>(timeDate.getDateMonth(d_ptr->printStartTime)));
        d_ptr->endSubBox->daySbx->setValue(static_cast<int>(timeDate.getDateDay(d_ptr->printStartTime)));
        d_ptr->endSubBox->hourSbx->setValue(static_cast<int>(timeDate.getTimeHour(d_ptr->printStartTime)));
        d_ptr->endSubBox->minSbx->setValue(static_cast<int>(timeDate.getTimeMinute(d_ptr->printStartTime)));
        d_ptr->endSubBox->secondSbx->setValue(static_cast<int>(timeDate.getTimeSenonds(d_ptr->printStartTime)));
        d_ptr->printEndTime = d_ptr->printStartTime;
    }
    else
    {
        d_ptr->printEndTime = timeStamp;
    }

    d_ptr->difftimeInfo();
}

void TrendPrintWindow::printReleased()
{
    TrendTableWindow::getInstance()->printTrendData(d_ptr->printStartTime, d_ptr->printEndTime);
    hide();
}

void TrendPrintWindowPrivate::initGroupBox(QGroupBox *groupBox, TrendPrintWindowPrivate::SubGroupBox *subBox)
{
    subBox->dateLbl = new QLabel(trs("YearMonthDay"));
    subBox->timeLbl = new QLabel(trs("HourSystem"));
    subBox->yearSbx = new SpinBox();
    subBox->monthSbx = new SpinBox();
    subBox->daySbx = new SpinBox();
    subBox->hourSbx = new SpinBox();
    subBox->minSbx = new SpinBox();
    subBox->secondSbx = new SpinBox();

    subBox->yearSbx->setRange(2000, 2099);
    subBox->yearSbx->setStep(1);
    subBox->yearSbx->setArrow(false);

    subBox->monthSbx->setRange(1, 12);
    subBox->monthSbx->setStep(1);
    subBox->monthSbx->setArrow(false);

    subBox->daySbx->setRange(1, 30);
    subBox->daySbx->setStep(1);
    subBox->daySbx->setArrow(false);

    subBox->hourSbx->setRange(0, 23);
    subBox->hourSbx->setStep(1);
    subBox->hourSbx->setArrow(false);

    subBox->minSbx->setRange(0, 59);
    subBox->minSbx->setStep(1);
    subBox->minSbx->setArrow(false);

    subBox->secondSbx->setRange(0, 59);
    subBox->secondSbx->setStep(1);
    subBox->secondSbx->setArrow(false);

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addWidget(subBox->dateLbl, 0, 0);
    mainLayout->addWidget(subBox->yearSbx, 0, 1);
    mainLayout->addWidget(subBox->monthSbx, 0, 2);
    mainLayout->addWidget(subBox->daySbx, 0, 3);
    mainLayout->addWidget(subBox->timeLbl, 1, 0);
    mainLayout->addWidget(subBox->hourSbx, 1, 1);
    mainLayout->addWidget(subBox->minSbx, 1, 2);
    mainLayout->addWidget(subBox->secondSbx, 1, 3);
    groupBox->setLayout(mainLayout);
}

void TrendPrintWindowPrivate::difftimeInfo()
{
    QString infoStr = trs("PrintLength");
    infoStr += " ";
    unsigned diffTime = qAbs(printEndTime - printStartTime);
    infoStr += QString::number(diffTime / 3600);
    infoStr += " ";
    infoStr += trs("Hour");
    infoStr += " ";
    infoStr += QString::number(diffTime % 3600 / 60);
    infoStr += " ";
    infoStr += trs("Minute");
    infoStr += " ";
    infoStr += QString::number(diffTime % 60);
    infoStr += " ";
    infoStr += trs("Second");
    durationLbl->setText(infoStr);
}
