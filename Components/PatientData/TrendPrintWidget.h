#pragma once
#include "PopupWidget.h"
#include <QGroupBox>
#include <QLabel>
#include "ISpinBox.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

struct SubGroupBox
{
    QLabel *dateLbl;
    QLabel *timeLbl;
    ISpinBox *yearBtn;
    ISpinBox *monthBtn;
    ISpinBox *dayBtn;
    ISpinBox *hourBtn;
    ISpinBox *minBtn;
    ISpinBox *secondBtn;
};

class TrendPrintWidget : public PopupWidget
{
    Q_OBJECT

public:
    TrendPrintWidget();
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
    void _initGroupBox(QGroupBox *groupBox, SubGroupBox &subBox);
    void _setSubGroupBoxSize(SubGroupBox &subBox, int width, int height);
    void _difftimeInfo(void);

private:
    QGroupBox *_startBox;
    QGroupBox *_endBox;

    SubGroupBox _startSubBox;
    SubGroupBox _endSubBox;

    QLabel *_durationLbl;
    IButton *_printBtn;
    unsigned _timeStartLimit;
    unsigned _timeEndLimit;
    unsigned _printStartTime;
    unsigned _printEndTime;
};
