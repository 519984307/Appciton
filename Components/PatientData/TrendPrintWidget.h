#pragma once
#include "PopupWidget.h"
#include <QGroupBox>
#include <QLabel>
#include "ISpinBox.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

class TimeGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    TimeGroupBox()
    {
        dateLbl = new QLabel("(year-month-day)");
        timeLbl = new QLabel("(24 hour system)");
        yearBtn = new ISpinBox();
        monthBtn = new ISpinBox();
        dayBtn = new ISpinBox();
        hourBtn = new ISpinBox();
        minBtn = new ISpinBox();
        secondBtn = new ISpinBox();

        yearBtn->enableArrow(false);
        yearBtn->setMode(ISPIN_MODE_INT);
        yearBtn->setRange(2000, 2099);
        yearBtn->setStep(1);
        yearBtn->setID(1);

        monthBtn->enableArrow(false);
        monthBtn->setMode(ISPIN_MODE_INT);
        monthBtn->setRange(1, 12);
        monthBtn->setStep(1);
        monthBtn->setID(2);

        dayBtn->enableArrow(false);
        dayBtn->setMode(ISPIN_MODE_INT);
        dayBtn->setRange(1, 30);
        dayBtn->setStep(1);
        dayBtn->setID(3);

        hourBtn->enableArrow(false);
        hourBtn->setMode(ISPIN_MODE_INT);
        hourBtn->setRange(0, 23);
        hourBtn->setStep(1);
        hourBtn->setID(4);

        minBtn->enableArrow(false);
        minBtn->setMode(ISPIN_MODE_INT);
        minBtn->setRange(0, 59);
        minBtn->setStep(1);
        minBtn->setID(5);

        secondBtn->enableArrow(false);
        secondBtn->setMode(ISPIN_MODE_INT);
        secondBtn->setRange(0, 59);
        secondBtn->setStep(1);
        secondBtn->setID(6);

        dateLayout = new QHBoxLayout();
        dateLayout->addWidget(dateLbl);
        dateLayout->addWidget(yearBtn);
        dateLayout->addWidget(monthBtn);
        dateLayout->addWidget(dayBtn);
        dateLayout->setSpacing(2);

        timeLayout = new QHBoxLayout();
        timeLayout->addWidget(timeLbl);
        timeLayout->addWidget(hourBtn);
        timeLayout->addWidget(minBtn);
        timeLayout->addWidget(secondBtn);
        timeLayout->setSpacing(2);

        mainLayout = new QVBoxLayout();
        mainLayout->addLayout(dateLayout);
        mainLayout->addLayout(timeLayout);

        setLayout(mainLayout);
    }

    void setFixedSize(int w, int h)
    {
        dateLbl->setFixedSize(w * 2, h);
        timeLbl->setFixedSize(w * 2, h);
        yearBtn->setFixedSize(w, h);
        monthBtn->setFixedSize(w, h);
        dayBtn->setFixedSize(w, h);
        hourBtn->setFixedSize(w, h);
        minBtn->setFixedSize(w, h);
        secondBtn->setFixedSize(w, h);
    }
    QLabel *dateLbl;
    QLabel *timeLbl;
    ISpinBox *yearBtn;
    ISpinBox *monthBtn;
    ISpinBox *dayBtn;
    ISpinBox *hourBtn;
    ISpinBox *minBtn;
    ISpinBox *secondBtn;
    QHBoxLayout *dateLayout;
    QHBoxLayout *timeLayout;
    QVBoxLayout *mainLayout;

};

class TrendPrintWidget : public PopupWidget
{
    Q_OBJECT

public:
    static TrendPrintWidget &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new TrendPrintWidget();
        }

        return *_selfObj;
    }

    static TrendPrintWidget *_selfObj;
    ~TrendPrintWidget();

public:
    void initPrintTime(unsigned start, unsigned end);
    void printTimeRange(unsigned startLimit, unsigned endLimit);

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _startTimeChange(QString, int);
    void _endTimeChange(QString, int);
    void _printReleased(void);

private:
    void _difftimeInfo(void);

private:
    TrendPrintWidget();

    TimeGroupBox *_startBox;
    TimeGroupBox *_endBox;
    QLabel *_durationLbl;
    IButton *_printBtn;
    unsigned _timeStartLimit;
    unsigned _timeEndLimit;
    unsigned _printStartTime;
    unsigned _printEndTime;
};
#define trendPrintWidget (TrendPrintWidget::construction())
#define deleteTrendPrintWidget() (delete TrendPrintWidget::_selfObj)
