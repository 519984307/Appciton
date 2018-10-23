/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/18
 **/

#include "TimeEditWindow.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "IConfig.h"
#include "SpinBox.h"
#include "TimeSymbol.h"
#include "TimeDate.h"
#include <QProcess>
#include <QVBoxLayout>
#include "TimeManager.h"
#include "SystemManager.h"

class TimeEditWindowPrivate
{
public:
    enum MenuItem
    {
        ITEM_SPB_YEAR,
        ITEM_SPB_MONTH,
        ITEM_SPB_DAY,
        ITEM_SPB_HOUR,
        ITEM_SPB_MINUTE,
        ITEM_SPB_SECOND,
        ITEM_CBO_DATE_FORMAT,
        ITEM_CBO_TIME_FORMAT,
        ITEM_CBO_DISPLAY_SEC
    };

    TimeEditWindowPrivate()
        : isChangeTime(false)
    {}

    /**
     * @brief loadOptions
     */
    void loadOptions();
    /**
     * @brief getMaxDay
     * @param year
     * @param month
     * @return
     */
    int getMaxDay(int year, int month);
    /**
     * @brief setSysTime
     */
    void setSysTime();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, SpinBox *> spinBoxs;
    bool isChangeTime;
};

void TimeEditWindowPrivate::loadOptions()
{
    spinBoxs[ITEM_SPB_YEAR]->setValue(timeDate.getDateYear());
    spinBoxs[ITEM_SPB_MONTH]->setValue(timeDate.getDateMonth());
    spinBoxs[ITEM_SPB_DAY]->setValue(timeDate.getDateDay());
    spinBoxs[ITEM_SPB_HOUR]->setValue(timeDate.getTimeHour());
    spinBoxs[ITEM_SPB_MINUTE]->setValue(timeDate.getTimeMinute());
    spinBoxs[ITEM_SPB_SECOND]->setValue(timeDate.getTimeSenonds());

    if (timeDate.getDateYear() < 1970)
    {
        QString cmd("date -s ");
        cmd += "\"1970-01-01 00:00:00\"";

        QProcess::execute(cmd);
        QProcess::execute("hwclock --systohc");
        QProcess::execute("./etc/init.d/save-rtc.sh");
        QProcess::execute("sync");
    }
    else if (timeDate.getDateYear() > 2037)
    {
        QString cmd("date -s ");
        cmd += "\"2037-12-27 20:00:00\"";

        QProcess::execute(cmd);
        QProcess::execute("hwclock --systohc");
        QProcess::execute("./etc/init.d/save-rtc.sh");
        QProcess::execute("sync");
    }

    int value = 0;
    currentConfig.getNumValue("DateTime|DateFormat", value);
    combos[ITEM_CBO_DATE_FORMAT]->setCurrentIndex(value);

    currentConfig.getNumValue("DateTime|TimeFormat", value);
    combos[ITEM_CBO_TIME_FORMAT]->setCurrentIndex(value);

    currentConfig.getNumValue("DateTime|DisplaySecond", value);
    combos[ITEM_CBO_DISPLAY_SEC]->setCurrentIndex(value);
}

int TimeEditWindowPrivate::getMaxDay(int year, int month)
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

void TimeEditWindowPrivate::setSysTime()
{
    QString cmd("date -s ");
    cmd += "\"";
    cmd += QString::number(spinBoxs[ITEM_SPB_YEAR]->getValue());
    cmd += "-";
    cmd += QString::number(spinBoxs[ITEM_SPB_MONTH]->getValue());
    cmd += "-";
    cmd += QString::number(spinBoxs[ITEM_SPB_DAY]->getValue());
    cmd += " ";
    cmd += QString::number(spinBoxs[ITEM_SPB_HOUR]->getValue());
    cmd += ":";
    cmd += QString::number(spinBoxs[ITEM_SPB_MINUTE]->getValue());
    cmd += ":";
    cmd += QString::number(spinBoxs[ITEM_SPB_SECOND]->getValue());
    cmd += "\"";

    QProcess::execute(cmd);
    QProcess::execute("hwclock --systohc");
    QProcess::execute("./etc/init.d/save-rtc.sh");
    QProcess::execute("sync");
}

TimeEditWindow::TimeEditWindow()
    : Window(),
      d_ptr(new TimeEditWindowPrivate)
{
    layoutExec();
    readyShow();
}

TimeEditWindow::~TimeEditWindow()
{
    delete d_ptr;
}

void TimeEditWindow::readyShow()
{
    d_ptr->loadOptions();

    bool timeEditable = systemManager.getCurWorkMode() != WORK_MODE_DEMO;
    for (int i = TimeEditWindowPrivate::ITEM_SPB_YEAR; i <= TimeEditWindowPrivate::ITEM_SPB_SECOND; i++ )
    {
        d_ptr->spinBoxs[static_cast<TimeEditWindowPrivate::MenuItem>(i)]->setEnabled(timeEditable);
    }
}

void TimeEditWindow::layoutExec()
{
    setWindowTitle(trs("SupervisorTimeAndDataMenu"));

    QVBoxLayout *vlayout = new QVBoxLayout;
    QGridLayout *layout = new QGridLayout;

    vlayout->addLayout(layout);

    ComboBox *comboBox;
    QLabel *label;
    SpinBox *spinBox;
    int itemID;

    // date
    label = new QLabel(trs("Date"));
    layout->addWidget(label, (d_ptr->combos.count() + d_ptr->spinBoxs.count())/3, 0);

    // year
    spinBox = new SpinBox;
    spinBox->setRange(1970, 2037);
    spinBox->setScale(1);
    spinBox->setStep(1);
    spinBox->setArrow(false);
    itemID = static_cast<int>(TimeEditWindowPrivate::ITEM_SPB_YEAR);
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    layout->addWidget(spinBox, (d_ptr->combos.count() + d_ptr->spinBoxs.count())/3, 1);
    d_ptr->spinBoxs.insert(TimeEditWindowPrivate::ITEM_SPB_YEAR, spinBox);

    // month
    spinBox = new SpinBox;
    spinBox->setRange(1, 12);
    spinBox->setScale(1);
    spinBox->setStep(1);
    spinBox->setArrow(false);
    itemID = static_cast<int>(TimeEditWindowPrivate::ITEM_SPB_MONTH);
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    layout->addWidget(spinBox, (d_ptr->combos.count() + d_ptr->spinBoxs.count())/3, 2);
    d_ptr->spinBoxs.insert(TimeEditWindowPrivate::ITEM_SPB_MONTH, spinBox);

    // day
    spinBox = new SpinBox;
    spinBox->setRange(1, 30);
    spinBox->setScale(1);
    spinBox->setStep(1);
    spinBox->setArrow(false);
    itemID = static_cast<int>(TimeEditWindowPrivate::ITEM_SPB_DAY);
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    layout->addWidget(spinBox, (d_ptr->combos.count() + d_ptr->spinBoxs.count())/3, 3);
    d_ptr->spinBoxs.insert(TimeEditWindowPrivate::ITEM_SPB_DAY, spinBox);

    // time
    label = new QLabel(trs("Time"));
    layout->addWidget(label, (d_ptr->combos.count() + d_ptr->spinBoxs.count())/3, 0);

    // hour
    spinBox = new SpinBox;
    spinBox->setScale(1);
    spinBox->setStep(1);
    spinBox->setArrow(false);
    itemID = static_cast<int>(TimeEditWindowPrivate::ITEM_SPB_HOUR);
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    layout->addWidget(spinBox, (d_ptr->combos.count() + d_ptr->spinBoxs.count())/3, 1);
    d_ptr->spinBoxs.insert(TimeEditWindowPrivate::ITEM_SPB_HOUR, spinBox);

    // minute
    spinBox = new SpinBox;
    spinBox->setRange(0, 59);
    spinBox->setScale(1);
    spinBox->setStep(1);
    spinBox->setArrow(false);
    itemID = static_cast<int>(TimeEditWindowPrivate::ITEM_SPB_MINUTE);
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    layout->addWidget(spinBox, (d_ptr->combos.count() + d_ptr->spinBoxs.count())/3, 2);
    d_ptr->spinBoxs.insert(TimeEditWindowPrivate::ITEM_SPB_MINUTE, spinBox);

    // second
    spinBox = new SpinBox;
    spinBox->setRange(0, 59);
    spinBox->setScale(1);
    spinBox->setStep(1);
    spinBox->setArrow(false);
    itemID = static_cast<int>(TimeEditWindowPrivate::ITEM_SPB_SECOND);
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    layout->addWidget(spinBox, (d_ptr->combos.count() + d_ptr->spinBoxs.count())/3, 3);
    d_ptr->spinBoxs.insert(TimeEditWindowPrivate::ITEM_SPB_SECOND, spinBox);

    layout->setRowStretch(d_ptr->combos.count() + d_ptr->spinBoxs.count(), 1);

    // date format
    layout = new QGridLayout;
    label = new QLabel(trs("SupervisorDateFormat"));
    layout->addWidget(label, 0, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(TimeSymbol::convert(DATE_FORMAT_Y_M_D))
                       << trs(TimeSymbol::convert(DATE_FORMAT_M_D_Y))
                       << trs(TimeSymbol::convert(DATE_FORMAT_D_M_Y))
                      );
    itemID = static_cast<int>(TimeEditWindowPrivate::ITEM_CBO_DATE_FORMAT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, 0, 1);
    d_ptr->combos.insert(TimeEditWindowPrivate::ITEM_CBO_DATE_FORMAT, comboBox);

    // time format
    label = new QLabel(trs("SupervisorTimeFormat"));
    layout->addWidget(label, 1, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(TimeSymbol::convert(TIME_FORMAT_12))
                       << trs(TimeSymbol::convert(TIME_FORMAT_24))
                      );
    itemID = static_cast<int>(TimeEditWindowPrivate::ITEM_CBO_TIME_FORMAT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, 1, 1);
    d_ptr->combos.insert(TimeEditWindowPrivate::ITEM_CBO_TIME_FORMAT, comboBox);

    // is display second
    label = new QLabel(trs("SupervisorDisplaySec"));
    layout->addWidget(label, 2, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("No")
                       << trs("Yes")
                      );
    itemID = static_cast<int>(TimeEditWindowPrivate::ITEM_CBO_DISPLAY_SEC);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, 2, 1);
    d_ptr->combos.insert(TimeEditWindowPrivate::ITEM_CBO_DISPLAY_SEC, comboBox);
    vlayout->addLayout(layout);
    vlayout->addStretch();

    setWindowLayout(vlayout);

    setFixedSize(580, 350);
}

void TimeEditWindow::hideEvent(QHideEvent *ev)
{
    if (d_ptr->isChangeTime)
    {
        d_ptr->setSysTime();
    }
    timeManager.roloadConfig();
    Window::hideEvent(ev);
}

void TimeEditWindow::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        TimeEditWindowPrivate::MenuItem item
            = (TimeEditWindowPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case TimeEditWindowPrivate::ITEM_CBO_DATE_FORMAT:
            currentConfig.setNumValue("DateTime|DateFormat", index);
            break;
        case TimeEditWindowPrivate::ITEM_CBO_TIME_FORMAT:
            currentConfig.setNumValue("DateTime|TimeFormat", index);
            break;
        case TimeEditWindowPrivate::ITEM_CBO_DISPLAY_SEC:
            currentConfig.setNumValue("DateTime|DisplaySecond", index);
            break;
        default:
            break;
        }
    }
}

void TimeEditWindow::onSpinBoxValueChanged(int value, int scale)
{
    SpinBox *spinBox = qobject_cast<SpinBox *>(sender());
    if (spinBox)
    {
        int val = value / scale;
        TimeEditWindowPrivate::MenuItem item
            = (TimeEditWindowPrivate::MenuItem)spinBox->property("Item").toInt();
        switch (item)
        {
        case TimeEditWindowPrivate::ITEM_SPB_YEAR:
        {
            if (2 == d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_MONTH]->getValue())
            {
                int min = 0;
                int max = 0;
                d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->getRange(min, max);
                int curMax = d_ptr->getMaxDay(val, 2);
                int curVal = d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->getValue();
                if (max != curMax)
                {
                    d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->setRange(min, curMax);
                }

                if (curVal > curMax)
                {
                    d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->setValue(curMax);
                }
            }
            d_ptr->isChangeTime = true;
            break;
        }
        case TimeEditWindowPrivate::ITEM_SPB_MONTH:
        {
            int min = 0;
            int max = 0;
            d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->getRange(min, max);
            int curMax = d_ptr->getMaxDay(d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_YEAR]->getValue(), val);
            int curVal = d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->getValue();
            if (max != curMax)
            {
                d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->setRange(min, curMax);
            }

            if (curVal > curMax)
            {
                d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->setValue(curMax);
            }
            d_ptr->isChangeTime = true;
            break;
        }
        case TimeEditWindowPrivate::ITEM_SPB_DAY:
        case TimeEditWindowPrivate::ITEM_SPB_HOUR:
        case TimeEditWindowPrivate::ITEM_SPB_MINUTE:
        case TimeEditWindowPrivate::ITEM_SPB_SECOND:
            d_ptr->isChangeTime = true;
            break;
        default:
            break;
        }
    }
}
