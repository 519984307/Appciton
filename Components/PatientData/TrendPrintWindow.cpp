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
#include "Framework/UI/Button.h"
#include "Framework/UI/SpinBox.h"
#include "Framework/Language/LanguageManager.h"
#include "Framework/TimeDate/TimeDate.h"
#include "Framework/TimeDate/TimeSymbol.h"
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include "TrendTableWindow.h"
#include "IConfig.h"
#include "SystemManager.h"

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
          printEndTime(0), timeFormat(TIME_FORMAT_24)
    {}

    void initGroupBox(QGroupBox *groupBox, SubGroupBox *subBox);
    void difftimeInfo(void);

    /**
     * @brief adjustPrintTime  adjust print time
     * @param printTime  print time
     * @param start  if is start print time
     */
    void adjustPrintTime(unsigned printTime, bool start);

    /**
     * @brief blockSignal block spinbox signal
     * @param flag
     */
    void blockSignal(bool flag, bool start);

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

    QVector<BlockEntry> blockList;            // 趋势数据类型时间信息

    TimeFormat timeFormat;

    QStringList hourList;
};
TrendPrintWindow::TrendPrintWindow(const QVector<BlockEntry> blockList)
    : Dialog(), d_ptr(new TrendPrintWindowPrivate())
{
    d_ptr->timeFormat = systemManager.getSystemTimeFormat();
    if (d_ptr->timeFormat == TIME_FORMAT_12)
    {
        // 设置时间的字符串列表 12AM-11AM 12PM-11PM
        d_ptr->hourList.append("12 AM");
        for (int i = 1; i < 12; i++)
        {
            d_ptr->hourList.append(QString("%1 AM").arg(QString::number(i)));
        }
        d_ptr->hourList.append("12 PM");
        for (int i = 1; i < 12; i++)
        {
            d_ptr->hourList.append(QString("%1 PM").arg(QString::number(i)));
        }
    }
    else
    {
        d_ptr->hourList.clear();
    }
    setWindowTitle(trs("PrintSetup"));
    // set the window width as 520 for showing all character string
    setFixedSize(520, 450);

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
            this, SLOT(endTimeChangeSlot(int, int)));
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
    d_ptr->printBtn->setFixedWidth(80);
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

    d_ptr->blockList = blockList;
}

TrendPrintWindow::~TrendPrintWindow()
{
}

void TrendPrintWindow::initPrintTime(unsigned start, unsigned end)
{
    d_ptr->printStartTime = start;
    d_ptr->printEndTime = end;

    d_ptr->blockSignal(true, true);
    d_ptr->startSubBox->yearSbx->setValue(static_cast<int>(timeDate->getDateYear(start)));
    d_ptr->startSubBox->monthSbx->setValue(static_cast<int>(timeDate->getDateMonth(start)));
    d_ptr->startSubBox->daySbx->setValue(static_cast<int>(timeDate->getDateDay(start)));
    d_ptr->startSubBox->hourSbx->setValue(static_cast<int>(timeDate->getTimeHour(start)));
    d_ptr->startSubBox->minSbx->setValue(static_cast<int>(timeDate->getTimeMinute(start)));
    d_ptr->startSubBox->secondSbx->setValue(static_cast<int>(timeDate->getTimeSeconds(start)));
    d_ptr->blockSignal(false, true);

    d_ptr->blockSignal(true, false);
    d_ptr->endSubBox->yearSbx->setValue(static_cast<int>(timeDate->getDateYear(end)));
    d_ptr->endSubBox->monthSbx->setValue(static_cast<int>(timeDate->getDateMonth(end)));
    d_ptr->endSubBox->daySbx->setValue(static_cast<int>(timeDate->getDateDay(end)));
    d_ptr->endSubBox->hourSbx->setValue(static_cast<int>(timeDate->getTimeHour(end)));
    d_ptr->endSubBox->minSbx->setValue(static_cast<int>(timeDate->getTimeMinute(end)));
    d_ptr->endSubBox->secondSbx->setValue(static_cast<int>(timeDate->getTimeSeconds(end)));
    d_ptr->blockSignal(false, false);

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
    int h = d_ptr->startSubBox->hourSbx->getValue();
    QTime time(h, d_ptr->startSubBox->minSbx->getValue(),
               d_ptr->startSubBox->secondSbx->getValue());
    QDateTime dateTime(date, time);
    if (!date.isValid() || !time.isValid())
    {
        return;
    }
    unsigned timeStamp = dateTime.toTime_t();
    if (timeStamp < d_ptr->timeStartLimit)
    {
        d_ptr->blockSignal(true, true);
        d_ptr->startSubBox->yearSbx->setValue(static_cast<int>(timeDate->getDateYear(d_ptr->timeStartLimit)));
        d_ptr->startSubBox->monthSbx->setValue(static_cast<int>(timeDate->getDateMonth(d_ptr->timeStartLimit)));
        d_ptr->startSubBox->daySbx->setValue(static_cast<int>(timeDate->getDateDay(d_ptr->timeStartLimit)));
        d_ptr->startSubBox->hourSbx->setValue(static_cast<int>(timeDate->getTimeHour(d_ptr->timeStartLimit)));
        d_ptr->startSubBox->minSbx->setValue(static_cast<int>(timeDate->getTimeMinute(d_ptr->timeStartLimit)));
        d_ptr->startSubBox->secondSbx->setValue(static_cast<int>(timeDate->getTimeSeconds(d_ptr->timeStartLimit)));
        d_ptr->blockSignal(false, true);
        d_ptr->printStartTime = d_ptr->timeStartLimit;
    }
    else if (timeStamp > d_ptr->printEndTime)
    {
        d_ptr->blockSignal(true, true);
        d_ptr->startSubBox->yearSbx->setValue(static_cast<int>(timeDate->getDateYear(d_ptr->printEndTime)));
        d_ptr->startSubBox->monthSbx->setValue(static_cast<int>(timeDate->getDateMonth(d_ptr->printEndTime)));
        d_ptr->startSubBox->daySbx->setValue(static_cast<int>(timeDate->getDateDay(d_ptr->printEndTime)));
        d_ptr->startSubBox->hourSbx->setValue(static_cast<int>(timeDate->getTimeHour(d_ptr->printEndTime)));
        d_ptr->startSubBox->minSbx->setValue(static_cast<int>(timeDate->getTimeMinute(d_ptr->printEndTime)));
        d_ptr->startSubBox->secondSbx->setValue(static_cast<int>(timeDate->getTimeSeconds(d_ptr->printEndTime)));
        d_ptr->blockSignal(false, true);
        d_ptr->printStartTime = d_ptr->printEndTime;
    }
    else
    {
        d_ptr->adjustPrintTime(timeStamp, true);
    }

    d_ptr->difftimeInfo();
}

void TrendPrintWindow::endTimeChangeSlot(int, int)
{
    QDate date(d_ptr->endSubBox->yearSbx->getValue(),
               d_ptr->endSubBox->monthSbx->getValue(),
               d_ptr->endSubBox->daySbx->getValue());
    int h = d_ptr->endSubBox->hourSbx->getValue();
    QTime time(h, d_ptr->endSubBox->minSbx->getValue(),
               d_ptr->endSubBox->secondSbx->getValue());
    QDateTime dateTime(date, time);
    if (!date.isValid() || !time.isValid())
    {
        return;
    }
    unsigned timeStamp = dateTime.toTime_t();
    if (timeStamp > d_ptr->timeEndLimit)
    {
        d_ptr->blockSignal(true, false);
        d_ptr->endSubBox->yearSbx->setValue(static_cast<int>(timeDate->getDateYear(d_ptr->timeEndLimit)));
        d_ptr->endSubBox->monthSbx->setValue(static_cast<int>(timeDate->getDateMonth(d_ptr->timeEndLimit)));
        d_ptr->endSubBox->daySbx->setValue(static_cast<int>(timeDate->getDateDay(d_ptr->timeEndLimit)));
        d_ptr->endSubBox->hourSbx->setValue(static_cast<int>(timeDate->getTimeHour(d_ptr->timeEndLimit)));
        d_ptr->endSubBox->minSbx->setValue(static_cast<int>(timeDate->getTimeMinute(d_ptr->timeEndLimit)));
        d_ptr->endSubBox->secondSbx->setValue(static_cast<int>(timeDate->getTimeSeconds(d_ptr->timeEndLimit)));
        d_ptr->blockSignal(false, false);
        d_ptr->printEndTime = d_ptr->timeEndLimit;
    }
    else if (timeStamp < d_ptr->printStartTime)
    {
        d_ptr->blockSignal(true, false);
        d_ptr->endSubBox->yearSbx->setValue(static_cast<int>(timeDate->getDateYear(d_ptr->printStartTime)));
        d_ptr->endSubBox->monthSbx->setValue(static_cast<int>(timeDate->getDateMonth(d_ptr->printStartTime)));
        d_ptr->endSubBox->daySbx->setValue(static_cast<int>(timeDate->getDateDay(d_ptr->printStartTime)));
        d_ptr->endSubBox->hourSbx->setValue(static_cast<int>(timeDate->getTimeHour(d_ptr->printStartTime)));
        d_ptr->endSubBox->minSbx->setValue(static_cast<int>(timeDate->getTimeMinute(d_ptr->printStartTime)));
        d_ptr->endSubBox->secondSbx->setValue(static_cast<int>(timeDate->getTimeSeconds(d_ptr->printStartTime)));
        d_ptr->blockSignal(false, false);
        d_ptr->printEndTime = d_ptr->printStartTime;
    }
    else
    {
        d_ptr->adjustPrintTime(timeStamp, false);
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
    subBox->dateLbl = new QLabel();
    subBox->timeLbl = new QLabel(trs("Time"));
    subBox->yearSbx = new SpinBox();
    subBox->monthSbx = new SpinBox();
    subBox->daySbx = new SpinBox();
    subBox->hourSbx = new SpinBox();
    subBox->minSbx = new SpinBox();
    subBox->secondSbx = new SpinBox();

    subBox->yearSbx->setRange(1970, 2037);
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
    if (timeFormat == TIME_FORMAT_12)
    {
        // 时间格式为12制时，设置spinbox为字符串类型
        subBox->hourSbx->setSpinBoxStyle(SpinBox::SPIN_BOX_STYLE_STRING);
        subBox->hourSbx->setStringList(hourList);
    }
    else
    {
        // 时间格式为24制时，设置spinbox为数字类型
        subBox->hourSbx->setSpinBoxStyle(SpinBox::SPIN_BOX_STYLE_NUMBER);
    }

    subBox->minSbx->setRange(0, 59);
    subBox->minSbx->setStep(1);
    subBox->minSbx->setArrow(false);

    subBox->secondSbx->setRange(0, 59);
    subBox->secondSbx->setStep(1);
    subBox->secondSbx->setArrow(false);

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addWidget(subBox->dateLbl, 0, 0);
    mainLayout->addWidget(subBox->timeLbl, 1, 0);
    groupBox->setLayout(mainLayout);

    // adjust the name of the date format
    DateFormat dateFormat = systemManager.getSystemDateFormat();
    QString dateFormatName =  QString("%1(%2)").arg(trs("Date"))
            .arg(trs(TimeSymbol::convert(dateFormat)));
    subBox->dateLbl->setText(dateFormatName);

    // adjust the position of the date
    SpinBox *dateSpinBoxArr[3] = {subBox->yearSbx, subBox->monthSbx, subBox->daySbx};
    switch (dateFormat)
    {
        case DATE_FORMAT_M_D_Y:
            dateSpinBoxArr[0] = subBox->monthSbx;
            dateSpinBoxArr[1] = subBox->daySbx;
            dateSpinBoxArr[2] = subBox->yearSbx;
        break;
        case DATE_FORMAT_D_M_Y:
            dateSpinBoxArr[0] = subBox->daySbx;
            dateSpinBoxArr[1] = subBox->monthSbx;
            dateSpinBoxArr[2] = subBox->yearSbx;
        break;
    default:
        break;
    }
    mainLayout->addWidget(dateSpinBoxArr[0], 0, 1);
    mainLayout->addWidget(dateSpinBoxArr[1], 0, 2);
    mainLayout->addWidget(dateSpinBoxArr[2], 0, 3);

    mainLayout->addWidget(subBox->hourSbx, 1, 1);
    mainLayout->addWidget(subBox->minSbx, 1, 2);
    mainLayout->addWidget(subBox->secondSbx, 1, 3);
}

void TrendPrintWindowPrivate::difftimeInfo()
{
    QString infoStr = trs("PrintLength");
    infoStr += " ";
    unsigned diffTime = qAbs(printEndTime - printStartTime);
    infoStr += QString::number(diffTime / 3600);
    infoStr += " ";
    infoStr += trs("Hours");
    infoStr += " ";
    infoStr += QString::number(diffTime % 3600 / 60);
    infoStr += " ";
    infoStr += trs("Minutes");
    infoStr += " ";
    infoStr += QString::number(diffTime % 60);
    infoStr += " ";
    infoStr += trs("Seconds");
    durationLbl->setText(infoStr);
}

void TrendPrintWindowPrivate::adjustPrintTime(unsigned printTime, bool start)
{
    int count = blockList.count();
    if (count == 0)
    {
        if (start)
        {
            printStartTime = printTime;
        }
        else
        {
            printEndTime = printTime;
        }
        return;
    }

    unsigned adjustTime = printTime;
    for (int i = 1; i < count; i++)
    {
        unsigned curTime = blockList.at(i - 1).extraData;
        unsigned nextTime = blockList.at(i).extraData;
        if (printTime == curTime)
        {
            adjustTime = curTime;
            break;
        }
        else if (printTime > curTime && printTime < nextTime)
        {
            // 使得打印的时间区间向中间聚拢缩短
            if (start)
            {
                adjustTime = nextTime;
            }
            else
            {
                adjustTime = curTime;
            }
            break;
        }
    }

    SubGroupBox *subBox = NULL;
    if (start)
    {
        printStartTime = adjustTime;
        subBox = startSubBox;
    }
    else
    {
        printEndTime = adjustTime;
        subBox = endSubBox;
    }

    subBox->yearSbx->blockSignals(true);
    subBox->yearSbx->setValue(static_cast<int>(timeDate->getDateYear(adjustTime)));
    subBox->yearSbx->blockSignals(false);

    subBox->monthSbx->blockSignals(true);
    subBox->monthSbx->setValue(static_cast<int>(timeDate->getDateMonth(adjustTime)));
    subBox->monthSbx->blockSignals(false);

    subBox->daySbx->blockSignals(true);
    subBox->daySbx->setValue(static_cast<int>(timeDate->getDateDay(adjustTime)));
    subBox->daySbx->blockSignals(false);

    subBox->hourSbx->blockSignals(true);
    subBox->hourSbx->setValue(static_cast<int>(timeDate->getTimeHour(adjustTime)));
    subBox->hourSbx->blockSignals(false);

    subBox->minSbx->blockSignals(true);
    subBox->minSbx->setValue(static_cast<int>(timeDate->getTimeMinute(adjustTime)));
    subBox->minSbx->blockSignals(false);

    subBox->secondSbx->blockSignals(true);
    subBox->secondSbx->setValue(static_cast<int>(timeDate->getTimeSeconds(adjustTime)));
    subBox->secondSbx->blockSignals(false);
}

void TrendPrintWindowPrivate::blockSignal(bool flag, bool start)
{
    SubGroupBox *subBox = NULL;
    if (start)
    {
        subBox = startSubBox;
    }
    else
    {
        subBox = endSubBox;
    }
    subBox->yearSbx->blockSignals(flag);
    subBox->monthSbx->blockSignals(flag);
    subBox->daySbx->blockSignals(flag);
    subBox->hourSbx->blockSignals(flag);
    subBox->minSbx->blockSignals(flag);
    subBox->secondSbx->blockSignals(flag);
}
