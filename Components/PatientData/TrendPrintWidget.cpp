#include "TrendPrintWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "TimeDate.h"
#include "TrendDataWidget.h"

#define ITEM_HEIGHT     30
#define ITEM_WIDTH      60

TrendPrintWidget *TrendPrintWidget::_selfObj = NULL;

TrendPrintWidget::TrendPrintWidget()
{
    setFixedSize(400, 300);

    int fontSize = fontManager.getFontSize(1);
    QFont font = fontManager.textFont(fontSize);

    setTitleBarText("Print Set");
    setTitleBarFont(font);

    _startBox = new TimeGroupBox();
    _startBox->setTitle("Start Time");
    _startBox->setFont(font);
    _startBox->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    connect(_startBox->yearBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_startTimeChange(QString,int)));
    connect(_startBox->monthBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_startTimeChange(QString,int)));
    connect(_startBox->dayBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_startTimeChange(QString,int)));
    connect(_startBox->hourBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_startTimeChange(QString,int)));
    connect(_startBox->minBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_startTimeChange(QString,int)));
    connect(_startBox->secondBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_startTimeChange(QString,int)));


    _endBox = new TimeGroupBox();
    _endBox->setTitle("End Time");
    _endBox->setFont(font);
    _endBox->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    connect(_endBox->yearBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_endTimeChange(QString,int)));
    connect(_endBox->monthBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_endTimeChange(QString,int)));
    connect(_endBox->dayBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_endTimeChange(QString,int)));
    connect(_endBox->hourBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_endTimeChange(QString,int)));
    connect(_endBox->minBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_endTimeChange(QString,int)));
    connect(_endBox->secondBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_endTimeChange(QString,int)));

    _durationLbl = new QLabel("Length of time");
    _durationLbl->setFont(font);
    _printBtn = new IButton(trs("Print"));
    _printBtn->setFont(font);
    _printBtn->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    connect(_printBtn, SIGNAL(realReleased()), this, SLOT(_printReleased()));

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(_durationLbl);
    hLayout->addWidget(_printBtn);
    hLayout->setMargin(5);

    contentLayout->addWidget(_startBox);
    contentLayout->addWidget(_endBox);
    contentLayout->addStretch();
    contentLayout->addLayout(hLayout);
    contentLayout->addStretch();
}

TrendPrintWidget::~TrendPrintWidget()
{

}

void TrendPrintWidget::initPrintTime(unsigned start, unsigned end)
{
    _startBox->yearBtn->setValue((int)timeDate.getDateYear(start));
    _startBox->monthBtn->setValue((int)timeDate.getDateMonth(start));
    _startBox->dayBtn->setValue((int)timeDate.getDateDay(start));
    _startBox->hourBtn->setValue((int)timeDate.getTimeHour(start));
    _startBox->minBtn->setValue((int)timeDate.getTimeMinute(start));
    _startBox->secondBtn->setValue((int)timeDate.getTimeSenonds(start));

    _endBox->yearBtn->setValue((int)timeDate.getDateYear(end));
    _endBox->monthBtn->setValue((int)timeDate.getDateMonth(end));
    _endBox->dayBtn->setValue((int)timeDate.getDateDay(end));
    _endBox->hourBtn->setValue((int)timeDate.getTimeHour(end));
    _endBox->minBtn->setValue((int)timeDate.getTimeMinute(end));
    _endBox->secondBtn->setValue((int)timeDate.getTimeSenonds(end));

    _printStartTime = start;
    _printEndTime = end;

    _difftimeInfo();
}

void TrendPrintWidget::printTimeRange(unsigned startLimit, unsigned endLimit)
{
    _timeStartLimit = startLimit;
    _timeEndLimit = endLimit;
}

void TrendPrintWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
}

void TrendPrintWidget::_startTimeChange(QString, int)
{
    QDate date(_startBox->yearBtn->getText().toInt(), _startBox->monthBtn->getText().toInt(), _startBox->dayBtn->getText().toInt());
    QTime time(_startBox->hourBtn->getText().toInt(), _startBox->minBtn->getText().toInt(), _startBox->secondBtn->getText().toInt());
    QDateTime dateTime(date, time);
    unsigned timeStamp = dateTime.toTime_t();
    if (timeStamp < _timeStartLimit)
    {
        _startBox->yearBtn->setValue((int)timeDate.getDateYear(_timeStartLimit));
        _startBox->monthBtn->setValue((int)timeDate.getDateMonth(_timeStartLimit));
        _startBox->dayBtn->setValue((int)timeDate.getDateDay(_timeStartLimit));
        _startBox->hourBtn->setValue((int)timeDate.getTimeHour(_timeStartLimit));
        _startBox->minBtn->setValue((int)timeDate.getTimeMinute(_timeStartLimit));
        _startBox->secondBtn->setValue((int)timeDate.getTimeSenonds(_timeStartLimit));
        _printStartTime = _timeStartLimit;
    }
    else if (timeStamp > _printEndTime)
    {
        _startBox->yearBtn->setValue((int)timeDate.getDateYear(_printEndTime));
        _startBox->monthBtn->setValue((int)timeDate.getDateMonth(_printEndTime));
        _startBox->dayBtn->setValue((int)timeDate.getDateDay(_printEndTime));
        _startBox->hourBtn->setValue((int)timeDate.getTimeHour(_printEndTime));
        _startBox->minBtn->setValue((int)timeDate.getTimeMinute(_printEndTime));
        _startBox->secondBtn->setValue((int)timeDate.getTimeSenonds(_printEndTime));
        _printStartTime = _printEndTime;
    }
    else
    {
        _printStartTime = timeStamp;
    }

    _difftimeInfo();
}

void TrendPrintWidget::_endTimeChange(QString, int)
{
    QDate date(_endBox->yearBtn->getText().toInt(), _endBox->monthBtn->getText().toInt(), _endBox->dayBtn->getText().toInt());
    QTime time(_endBox->hourBtn->getText().toInt(), _endBox->minBtn->getText().toInt(), _endBox->secondBtn->getText().toInt());
    QDateTime dateTime(date, time);
    unsigned timeStamp = dateTime.toTime_t();
    if (timeStamp > _timeEndLimit)
    {
        _endBox->yearBtn->setValue((int)timeDate.getDateYear(_timeEndLimit));
        _endBox->monthBtn->setValue((int)timeDate.getDateMonth(_timeEndLimit));
        _endBox->dayBtn->setValue((int)timeDate.getDateDay(_timeEndLimit));
        _endBox->hourBtn->setValue((int)timeDate.getTimeHour(_timeEndLimit));
        _endBox->minBtn->setValue((int)timeDate.getTimeMinute(_timeEndLimit));
        _endBox->secondBtn->setValue((int)timeDate.getTimeSenonds(_timeEndLimit));
        _printEndTime = _timeEndLimit;
    }
    else if (timeStamp < _printStartTime)
    {
        _endBox->yearBtn->setValue((int)timeDate.getDateYear(_printStartTime));
        _endBox->monthBtn->setValue((int)timeDate.getDateMonth(_printStartTime));
        _endBox->dayBtn->setValue((int)timeDate.getDateDay(_printStartTime));
        _endBox->hourBtn->setValue((int)timeDate.getTimeHour(_printStartTime));
        _endBox->minBtn->setValue((int)timeDate.getTimeMinute(_printStartTime));
        _endBox->secondBtn->setValue((int)timeDate.getTimeSenonds(_printStartTime));
        _printEndTime = _printStartTime;
    }
    else
    {
        _printEndTime = timeStamp;
    }

    _difftimeInfo();
}

void TrendPrintWidget::_printReleased()
{
    trendDataWidget.printTrendData(_printStartTime, _printEndTime);
    hide();
}

void TrendPrintWidget::_difftimeInfo()
{
    QString infoStr = "Record time ";
    unsigned diffTime = qAbs(_printEndTime - _printStartTime);
    infoStr += QString::number(diffTime / 3600);
    infoStr += " hour ";
    infoStr += QString::number(diffTime % 3600 / 60);
    infoStr += " minute ";
    infoStr += QString::number(diffTime % 60);
    infoStr += " second";
    _durationLbl->setText(infoStr);
}
