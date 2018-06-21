#include "TrendPrintWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "TimeDate.h"
#include "TrendDataWidget.h"

#define ITEM_HEIGHT     30
#define ITEM_WIDTH      60

TrendPrintWidget::TrendPrintWidget()
{
    setFixedSize(400, 300);

    int fontSize = fontManager.getFontSize(1);
    QFont font = fontManager.textFont(fontSize);

    setTitleBarText(trs("PrintSet"));
    setTitleBarFont(font);

    _startBox = new QGroupBox();
    _startBox->setTitle(trs("StartTime"));
    _startBox->setFont(font);
    _initGroupBox(_startBox, _startSubBox);
    _setSubGroupBoxSize(_startSubBox, ITEM_WIDTH, ITEM_HEIGHT);
    connect(_startSubBox.yearBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_startTimeChange(QString,int)));
    connect(_startSubBox.monthBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_startTimeChange(QString,int)));
    connect(_startSubBox.dayBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_startTimeChange(QString,int)));
    connect(_startSubBox.hourBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_startTimeChange(QString,int)));
    connect(_startSubBox.minBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_startTimeChange(QString,int)));
    connect(_startSubBox.secondBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_startTimeChange(QString,int)));


    _endBox = new QGroupBox();
    _endBox->setTitle(trs("EndTime"));
    _endBox->setFont(font);
    _initGroupBox(_endBox, _endSubBox);
    _setSubGroupBoxSize(_endSubBox, ITEM_WIDTH, ITEM_HEIGHT);
    connect(_endSubBox.yearBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_endTimeChange(QString,int)));
    connect(_endSubBox.monthBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_endTimeChange(QString,int)));
    connect(_endSubBox.dayBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_endTimeChange(QString,int)));
    connect(_endSubBox.hourBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_endTimeChange(QString,int)));
    connect(_endSubBox.minBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_endTimeChange(QString,int)));
    connect(_endSubBox.secondBtn, SIGNAL(valueChange(QString,int)),
            this, SLOT(_endTimeChange(QString,int)));

    _durationLbl = new QLabel();
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
    _startSubBox.yearBtn->setValue((int)timeDate.getDateYear(start));
    _startSubBox.monthBtn->setValue((int)timeDate.getDateMonth(start));
    _startSubBox.dayBtn->setValue((int)timeDate.getDateDay(start));
    _startSubBox.hourBtn->setValue((int)timeDate.getTimeHour(start));
    _startSubBox.minBtn->setValue((int)timeDate.getTimeMinute(start));
    _startSubBox.secondBtn->setValue((int)timeDate.getTimeSenonds(start));

    _endSubBox.yearBtn->setValue((int)timeDate.getDateYear(end));
    _endSubBox.monthBtn->setValue((int)timeDate.getDateMonth(end));
    _endSubBox.dayBtn->setValue((int)timeDate.getDateDay(end));
    _endSubBox.hourBtn->setValue((int)timeDate.getTimeHour(end));
    _endSubBox.minBtn->setValue((int)timeDate.getTimeMinute(end));
    _endSubBox.secondBtn->setValue((int)timeDate.getTimeSenonds(end));

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
    QDate date(_startSubBox.yearBtn->getText().toInt(), _startSubBox.monthBtn->getText().toInt(), _startSubBox.dayBtn->getText().toInt());
    QTime time(_startSubBox.hourBtn->getText().toInt(), _startSubBox.minBtn->getText().toInt(), _startSubBox.secondBtn->getText().toInt());
    QDateTime dateTime(date, time);
    unsigned timeStamp = dateTime.toTime_t();
    if (timeStamp < _timeStartLimit)
    {
        _startSubBox.yearBtn->setValue((int)timeDate.getDateYear(_timeStartLimit));
        _startSubBox.monthBtn->setValue((int)timeDate.getDateMonth(_timeStartLimit));
        _startSubBox.dayBtn->setValue((int)timeDate.getDateDay(_timeStartLimit));
        _startSubBox.hourBtn->setValue((int)timeDate.getTimeHour(_timeStartLimit));
        _startSubBox.minBtn->setValue((int)timeDate.getTimeMinute(_timeStartLimit));
        _startSubBox.secondBtn->setValue((int)timeDate.getTimeSenonds(_timeStartLimit));
        _printStartTime = _timeStartLimit;
    }
    else if (timeStamp > _printEndTime)
    {
        _startSubBox.yearBtn->setValue((int)timeDate.getDateYear(_printEndTime));
        _startSubBox.monthBtn->setValue((int)timeDate.getDateMonth(_printEndTime));
        _startSubBox.dayBtn->setValue((int)timeDate.getDateDay(_printEndTime));
        _startSubBox.hourBtn->setValue((int)timeDate.getTimeHour(_printEndTime));
        _startSubBox.minBtn->setValue((int)timeDate.getTimeMinute(_printEndTime));
        _startSubBox.secondBtn->setValue((int)timeDate.getTimeSenonds(_printEndTime));
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
    QDate date(_endSubBox.yearBtn->getText().toInt(), _endSubBox.monthBtn->getText().toInt(), _endSubBox.dayBtn->getText().toInt());
    QTime time(_endSubBox.hourBtn->getText().toInt(), _endSubBox.minBtn->getText().toInt(), _endSubBox.secondBtn->getText().toInt());
    QDateTime dateTime(date, time);
    unsigned timeStamp = dateTime.toTime_t();
    if (timeStamp > _timeEndLimit)
    {
        _endSubBox.yearBtn->setValue((int)timeDate.getDateYear(_timeEndLimit));
        _endSubBox.monthBtn->setValue((int)timeDate.getDateMonth(_timeEndLimit));
        _endSubBox.dayBtn->setValue((int)timeDate.getDateDay(_timeEndLimit));
        _endSubBox.hourBtn->setValue((int)timeDate.getTimeHour(_timeEndLimit));
        _endSubBox.minBtn->setValue((int)timeDate.getTimeMinute(_timeEndLimit));
        _endSubBox.secondBtn->setValue((int)timeDate.getTimeSenonds(_timeEndLimit));
        _printEndTime = _timeEndLimit;
    }
    else if (timeStamp < _printStartTime)
    {
        _endSubBox.yearBtn->setValue((int)timeDate.getDateYear(_printStartTime));
        _endSubBox.monthBtn->setValue((int)timeDate.getDateMonth(_printStartTime));
        _endSubBox.dayBtn->setValue((int)timeDate.getDateDay(_printStartTime));
        _endSubBox.hourBtn->setValue((int)timeDate.getTimeHour(_printStartTime));
        _endSubBox.minBtn->setValue((int)timeDate.getTimeMinute(_printStartTime));
        _endSubBox.secondBtn->setValue((int)timeDate.getTimeSenonds(_printStartTime));
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

void TrendPrintWidget::_initGroupBox(QGroupBox *groupBox, SubGroupBox &subBox)
{
    subBox.dateLbl = new QLabel(trs("YearMonthDay"));
    subBox.timeLbl = new QLabel(trs("HourSystem"));
    subBox.yearBtn = new ISpinBox();
    subBox.monthBtn = new ISpinBox();
    subBox.dayBtn = new ISpinBox();
    subBox.hourBtn = new ISpinBox();
    subBox.minBtn = new ISpinBox();
    subBox.secondBtn = new ISpinBox();

    subBox.yearBtn->enableArrow(false);
    subBox.yearBtn->setMode(ISPIN_MODE_INT);
    subBox.yearBtn->setRange(2000, 2099);
    subBox.yearBtn->setStep(1);
    subBox.yearBtn->setID(1);

    subBox.monthBtn->enableArrow(false);
    subBox.monthBtn->setMode(ISPIN_MODE_INT);
    subBox.monthBtn->setRange(1, 12);
    subBox.monthBtn->setStep(1);
    subBox.monthBtn->setID(2);

    subBox.dayBtn->enableArrow(false);
    subBox.dayBtn->setMode(ISPIN_MODE_INT);
    subBox.dayBtn->setRange(1, 30);
    subBox.dayBtn->setStep(1);
    subBox.dayBtn->setID(3);

    subBox.hourBtn->enableArrow(false);
    subBox.hourBtn->setMode(ISPIN_MODE_INT);
    subBox.hourBtn->setRange(0, 23);
    subBox.hourBtn->setStep(1);
    subBox.hourBtn->setID(4);

    subBox.minBtn->enableArrow(false);
    subBox.minBtn->setMode(ISPIN_MODE_INT);
    subBox.minBtn->setRange(0, 59);
    subBox.minBtn->setStep(1);
    subBox.minBtn->setID(5);

    subBox.secondBtn->enableArrow(false);
    subBox.secondBtn->setMode(ISPIN_MODE_INT);
    subBox.secondBtn->setRange(0, 59);
    subBox.secondBtn->setStep(1);
    subBox.secondBtn->setID(6);

    QHBoxLayout *dateLayout = new QHBoxLayout();
    dateLayout->addWidget(subBox.dateLbl);
    dateLayout->addWidget(subBox.yearBtn);
    dateLayout->addWidget(subBox.monthBtn);
    dateLayout->addWidget(subBox.dayBtn);
    dateLayout->setSpacing(2);

    QHBoxLayout *timeLayout = new QHBoxLayout();
    timeLayout->addWidget(subBox.timeLbl);
    timeLayout->addWidget(subBox.hourBtn);
    timeLayout->addWidget(subBox.minBtn);
    timeLayout->addWidget(subBox.secondBtn);
    timeLayout->setSpacing(2);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(dateLayout);
    mainLayout->addLayout(timeLayout);

    groupBox->setLayout(mainLayout);
}

void TrendPrintWidget::_setSubGroupBoxSize(SubGroupBox &subBox, int w, int h)
{
    subBox.dateLbl->setFixedSize(w * 2, h);
    subBox.timeLbl->setFixedSize(w * 2, h);
    subBox.yearBtn->setFixedSize(w, h);
    subBox.monthBtn->setFixedSize(w, h);
    subBox.dayBtn->setFixedSize(w, h);
    subBox.hourBtn->setFixedSize(w, h);
    subBox.minBtn->setFixedSize(w, h);
    subBox.secondBtn->setFixedSize(w, h);
}

void TrendPrintWidget::_difftimeInfo()
{
    QString infoStr = trs("PrintLength");
    infoStr += " ";
    unsigned diffTime = qAbs(_printEndTime - _printStartTime);
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
    _durationLbl->setText(infoStr);
}
