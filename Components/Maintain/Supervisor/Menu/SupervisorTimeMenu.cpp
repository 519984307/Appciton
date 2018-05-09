#include <QVBoxLayout>
#include <QProcess>
#include <QDateTime>
#include "FontManager.h"
#include "LanguageManager.h"
#include "IComboList.h"
#include "ISpinBox.h"
#include "SupervisorTimeMenu.h"
#include "SupervisorConfigManager.h"
#include "SupervisorMenuManager.h"
#include "TimeDefine.h"
#include "TimeSymbol.h"
#include "QDebug"

SupervisorTimeMenu *SupervisorTimeMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SupervisorTimeMenu::SupervisorTimeMenu() : SubMenu(trs("SupervisorTimeAndDataMenu"))
{
    setDesc(trs("SupervisorTimeAndDataMenuDesc"));
    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void SupervisorTimeMenu::readyShow()
{
    QDateTime curDateTime = QDateTime::currentDateTime();
    int year = curDateTime.date().year();
    int month = curDateTime.date().month();
    int day = curDateTime.date().day();
    int hour = curDateTime.time().hour();
    int minute = curDateTime.time().minute();
    int sec = curDateTime.time().second();
    int value = 0;
    if (year < 1970)
    {
        setSysTimeTo1970_01_01_00();
    }
    else if (year > 2037)
    {
        setSysTimeTo2037_12_27_20();
    }
    else
    {
        _year->setValue(year);
        _month->setValue(month);
        _day->setRange(1, _getMaxDay(year, month));
        _day->setValue(day);
        _hour->setValue(hour);
        _minute->setValue(minute);
        _second->setValue(sec);
    }

    currentConfig.getNumValue("DateTime|DateFormat", value);
    _dateFormat->setCurrentIndex(value);

    currentConfig.getNumValue("DateTime|TimeFormat", value);
    _timeFormat->setCurrentIndex(value);

    currentConfig.getNumValue("DateTime|DisplaySecond", value);
    _displaySec->setCurrentIndex(value);
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void SupervisorTimeMenu::layoutExec()
{
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    //年
    _year = new ISpinBox(trs("SupervisorYear"));
    _year->setFixedHeight(ITEM_H);
    _year->setRange(1970, 2037);
    _year->setStep(1);
    _year->setMode(ISPIN_MODE_INT);
    _year->setValueWidth(btnWidth);
    _year->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _year->setLayoutSpacing(ICOMBOLIST_SPACE);
    _year->enableArrow(false);
    _year->setFont(fontManager.textFont(fontSize));
    connect(_year, SIGNAL(valueChange(QString,int)), this, SLOT(_yearChangeSlot(QString)));
    mainLayout->addWidget(_year);

    //月
    _month = new ISpinBox(trs("SupervisorMonth"));
    _month->setFixedHeight(ITEM_H);
    _month->setRange(1, 12);
    _month->setStep(1);
    _month->setMode(ISPIN_MODE_INT);
    _month->setValueWidth(btnWidth);
    _month->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _month->setLayoutSpacing(ICOMBOLIST_SPACE);
    _month->enableArrow(false);
    _month->setFont(fontManager.textFont(fontSize));
    connect(_month, SIGNAL(valueChange(QString,int)), this, SLOT(_monthChangeSlot(QString)));
    mainLayout->addWidget(_month);

    //日
    _day = new ISpinBox(trs("SupervisorDay"));
    _day->setFixedHeight(ITEM_H);
    _day->setStep(1);
    _day->setMode(ISPIN_MODE_INT);
    _day->setValueWidth(btnWidth);
    _day->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _day->setLayoutSpacing(ICOMBOLIST_SPACE);
    _day->enableArrow(false);
    _day->setFont(fontManager.textFont(fontSize));
    connect(_day, SIGNAL(valueChange(QString,int)), this, SLOT(_timeChangeSlot(QString)));
    mainLayout->addWidget(_day);

    //时
    _hour = new ISpinBox(trs("SupervisorHour"));
    _hour->setFixedHeight(ITEM_H);
    _hour->setStep(1);
    _hour->setMode(ISPIN_MODE_INT);
    _hour->setValueWidth(btnWidth);
    _hour->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _hour->setLayoutSpacing(ICOMBOLIST_SPACE);
    _hour->enableArrow(false);
    _hour->setRange(0, 23);
    _hour->setFont(fontManager.textFont(fontSize));
    connect(_hour, SIGNAL(valueChange(QString,int)), this, SLOT(_timeChangeSlot(QString)));
    mainLayout->addWidget(_hour);

    //分
    _minute = new ISpinBox(trs("SupervisorMinute"));
    _minute->setFixedHeight(ITEM_H);
    _minute->setRange(0, 59);
    _minute->setStep(1);
    _minute->setMode(ISPIN_MODE_INT);
    _minute->setValueWidth(btnWidth);
    _minute->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _minute->setLayoutSpacing(ICOMBOLIST_SPACE);
    _minute->enableArrow(false);
    _minute->setFont(fontManager.textFont(fontSize));
    connect(_minute, SIGNAL(valueChange(QString,int)), this, SLOT(_timeChangeSlot(QString)));
    mainLayout->addWidget(_minute);

    //秒
    _second = new ISpinBox(trs("SupervisorSecond"));
    _second->setFixedHeight(ITEM_H);
    _second->setRange(0, 59);
    _second->setStep(1);
    _second->setMode(ISPIN_MODE_INT);
    _second->setValueWidth(btnWidth);
    _second->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _second->setLayoutSpacing(ICOMBOLIST_SPACE);
    _second->enableArrow(false);
    _second->setFont(fontManager.textFont(fontSize));
    connect(_second, SIGNAL(valueChange(QString,int)), this, SLOT(_timeChangeSlot(QString)));
    mainLayout->addWidget(_second);

    //日期格式
    _dateFormat = new IComboList(trs("SupervisorDateFormat"));
    _dateFormat->setFont(fontManager.textFont(fontSize));
    _dateFormat->label->setFixedSize(labelWidth, ITEM_H);
    _dateFormat->combolist->setFixedSize(btnWidth, ITEM_H);
    for (int i = 0; i < DATE_FORMAT_NR; ++i)
    {
        _dateFormat->addItem(trs(TimeSymbol::convert((DateFormat)i)));
    }
    connect(_dateFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(_dateFormatSlot(int)));
    mainLayout->addWidget(_dateFormat);

    //时间格式
    _timeFormat = new IComboList(trs("SupervisorTimeFormat"));
    _timeFormat->setFont(fontManager.textFont(fontSize));
    _timeFormat->label->setFixedSize(labelWidth, ITEM_H);
    _timeFormat->combolist->setFixedSize(btnWidth, ITEM_H);
    for (int i = 0; i < TIME_FORMAT_NR; ++i)
    {
        _timeFormat->addItem(trs(TimeSymbol::convert((TimeFormat)i)));
    }
    connect(_timeFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(_timeFormatSlot(int)));
    mainLayout->addWidget(_timeFormat);

    //是否显示秒
    _displaySec = new IComboList(trs("SupervisorDisplaySec"));
    _displaySec->setFont(fontManager.textFont(fontSize));
    _displaySec->label->setFixedSize(labelWidth, ITEM_H);
    _displaySec->combolist->setFixedSize(btnWidth, ITEM_H);
    _displaySec->addItem(trs("No"));
    _displaySec->addItem(trs("Yes"));
    connect(_displaySec, SIGNAL(currentIndexChanged(int)), this, SLOT(_displaySecSlot(int)));
    mainLayout->addWidget(_displaySec);
}

/**************************************************************************************************
 * 年改变。
 *************************************************************************************************/
void SupervisorTimeMenu::_yearChangeSlot(QString valueStr)
{
    if (2 == _month->getText().toInt())
    {
        int min = 0, max = 0;
        _day->getRange(min, max);
        int curMax = _getMaxDay(valueStr.toInt(), 2);
        int curValue = _day->getText().toInt();
        if (max != curMax)
        {
            _day->setRange(min, curMax);
        }

        if (curValue > curMax)
        {
            _day->setValue(curMax);
        }
    }

    _setSysTime();
}

/**************************************************************************************************
 * 月改变。
 *************************************************************************************************/
void SupervisorTimeMenu::_monthChangeSlot(QString valueStr)
{
    int min = 0, max = 0;
    _day->getRange(min, max);
    int curMax = _getMaxDay(_year->getText(), valueStr);
    int curValue = _day->getText().toInt();
    if (max != curMax)
    {
        _day->setRange(min, curMax);
    }

    if (curValue > curMax)
    {
        _day->setValue(curMax);
    }

    _setSysTime();
}

/**************************************************************************************************
 * 有时间项被改变改变。
 *************************************************************************************************/
void SupervisorTimeMenu::_timeChangeSlot(QString /*valueStr*/)
{
    _setSysTime();
}

/**************************************************************************************************
 * 日期格式改变。
 *************************************************************************************************/
void SupervisorTimeMenu::_dateFormatSlot(int index)
{
    currentConfig.setNumValue("DateTime|DateFormat", index);
}

/**************************************************************************************************
 * 时间改变。
 *************************************************************************************************/
void SupervisorTimeMenu::_timeFormatSlot(int index)
{
    currentConfig.setNumValue("DateTime|TimeFormat", index);
}

/**************************************************************************************************
 * 显示秒。
 *************************************************************************************************/
void SupervisorTimeMenu::_displaySecSlot(int index)
{
    currentConfig.setNumValue("DateTime|DisplaySecond", index);
}

/**************************************************************************************************
 * 获取最大的天数。
 *************************************************************************************************/
int SupervisorTimeMenu::_getMaxDay(QString yearStr, QString monthStr)
{
    int year = yearStr.toInt();
    int month = monthStr.toInt();

    return _getMaxDay(year, month);
}

/**************************************************************************************************
 * 设置系统时间。
 *************************************************************************************************/
void SupervisorTimeMenu::_setSysTime()
{
    QString cmd("date -s ");
    cmd += "\"";
    cmd += _year->getText();
    cmd += "-";
    cmd += _month->getText();
    cmd += "-";
    cmd += _day->getText();
    cmd += " ";
    cmd += _hour->getText();
    cmd += ":";
    cmd += _minute->getText();
    cmd += ":";
    cmd += _second->getText();
    cmd += "\"";

    QProcess::execute(cmd);
    QProcess::execute("hwclock --systohc");
    QProcess::execute("./etc/init.d/save-rtc.sh");
    QProcess::execute("sync");
}

/**************************************************************************************************
 * 设置系统时间到1970/01/01 00:00:00。
 *************************************************************************************************/
void SupervisorTimeMenu::setSysTimeTo1970_01_01_00()
{
    QString cmd("date -s ");
    cmd += "\"1970-01-01 00:00:00\"";

    QProcess::execute(cmd);
    QProcess::execute("hwclock --systohc");
    QProcess::execute("./etc/init.d/save-rtc.sh");
    QProcess::execute("sync");
}

/**************************************************************************************************
 * 设置系统时间到2037/12/27 20:00:00。
 *************************************************************************************************/
void SupervisorTimeMenu::setSysTimeTo2037_12_27_20()
{
    QString cmd("date -s ");
    cmd += "\"2037-12-27 20:00:00\"";

    QProcess::execute(cmd);
    QProcess::execute("hwclock --systohc");
    QProcess::execute("./etc/init.d/save-rtc.sh");
    QProcess::execute("sync");
}

/**************************************************************************************************
 * 获取最大的天数。
 *************************************************************************************************/
int SupervisorTimeMenu::_getMaxDay(int year, int month)
{
    int day31[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    bool isleapYear = false;
    if (0 == year % 100)
    {
        if (0 == year % 400)
        {
            isleapYear = true;
        }
    }
    else if (0 == year % 4)
    {
        isleapYear = true;
    }

    if (2 == month)
    {
        if (isleapYear)
        {
            return 29;
        }
    }

    return day31[month - 1];
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SupervisorTimeMenu::~SupervisorTimeMenu()
{

}
