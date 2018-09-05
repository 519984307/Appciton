/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/18
 **/

#include "SupervisorTimeWindow.h"
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

class SupervisorTimeWindowPrivate
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

    SupervisorTimeWindowPrivate() {}

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
};

void SupervisorTimeWindowPrivate::loadOptions()
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

int SupervisorTimeWindowPrivate::getMaxDay(int year, int month)
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

void SupervisorTimeWindowPrivate::setSysTime()
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

SupervisorTimeWindow::SupervisorTimeWindow()
    : Window(),
      d_ptr(new SupervisorTimeWindowPrivate)
{
    layoutExec();
    readyShow();
}

SupervisorTimeWindow::~SupervisorTimeWindow()
{
    delete d_ptr;
}

void SupervisorTimeWindow::readyShow()
{
    d_ptr->loadOptions();
}

void SupervisorTimeWindow::layoutExec()
{
    setWindowTitle(trs("SupervisorTimeAndDataMenu"));

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setMargin(10);

    QGridLayout *layout = new QGridLayout;

    vlayout->addStretch();
    vlayout->addLayout(layout);
    vlayout->addStretch();

    ComboBox *comboBox;
    QLabel *label;
    SpinBox *spinBox;
    int itemID;

    // year
    label = new QLabel(trs("SupervisorYear"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 0);
    spinBox = new SpinBox;
    spinBox->setRange(1970, 2037);
    spinBox->setScale(1);
    spinBox->setStep(1);
    itemID = static_cast<int>(SupervisorTimeWindowPrivate::ITEM_SPB_YEAR);
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    layout->addWidget(spinBox, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 1);
    d_ptr->spinBoxs.insert(SupervisorTimeWindowPrivate::ITEM_SPB_YEAR, spinBox);

    // month
    label = new QLabel(trs("SupervisorMonth"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 0);
    spinBox = new SpinBox;
    spinBox->setRange(1, 12);
    spinBox->setScale(1);
    spinBox->setStep(1);
    itemID = static_cast<int>(SupervisorTimeWindowPrivate::ITEM_SPB_MONTH);
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    layout->addWidget(spinBox, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 1);
    d_ptr->spinBoxs.insert(SupervisorTimeWindowPrivate::ITEM_SPB_MONTH, spinBox);

    // day
    label = new QLabel(trs("SupervisorDay"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 0);
    spinBox = new SpinBox;
    spinBox->setRange(1, 30);
    spinBox->setScale(1);
    spinBox->setStep(1);
    itemID = static_cast<int>(SupervisorTimeWindowPrivate::ITEM_SPB_DAY);
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    layout->addWidget(spinBox, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 1);
    d_ptr->spinBoxs.insert(SupervisorTimeWindowPrivate::ITEM_SPB_DAY, spinBox);

    // hour
    label = new QLabel(trs("SupervisorHour"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 0);
    spinBox = new SpinBox;
    spinBox->setScale(1);
    spinBox->setStep(1);
    itemID = static_cast<int>(SupervisorTimeWindowPrivate::ITEM_SPB_HOUR);
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    layout->addWidget(spinBox, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 1);
    d_ptr->spinBoxs.insert(SupervisorTimeWindowPrivate::ITEM_SPB_HOUR, spinBox);

    // minute
    label = new QLabel(trs("SupervisorMinute"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 0);
    spinBox = new SpinBox;
    spinBox->setRange(0, 59);
    spinBox->setScale(1);
    spinBox->setStep(1);
    itemID = static_cast<int>(SupervisorTimeWindowPrivate::ITEM_SPB_MINUTE);
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    layout->addWidget(spinBox, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 1);
    d_ptr->spinBoxs.insert(SupervisorTimeWindowPrivate::ITEM_SPB_MINUTE, spinBox);

    // second
    label = new QLabel(trs("SupervisorSecond"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 0);
    spinBox = new SpinBox;
    spinBox->setRange(0, 59);
    spinBox->setScale(1);
    spinBox->setStep(1);
    itemID = static_cast<int>(SupervisorTimeWindowPrivate::ITEM_SPB_SECOND);
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    layout->addWidget(spinBox, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 1);
    d_ptr->spinBoxs.insert(SupervisorTimeWindowPrivate::ITEM_SPB_SECOND, spinBox);

    // date format
    label = new QLabel(trs("SupervisorDateFormat"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(TimeSymbol::convert(DATE_FORMAT_Y_M_D))
                       << trs(TimeSymbol::convert(DATE_FORMAT_M_D_Y))
                       << trs(TimeSymbol::convert(DATE_FORMAT_D_M_Y))
                      );
    itemID = static_cast<int>(SupervisorTimeWindowPrivate::ITEM_CBO_DATE_FORMAT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 1);
    d_ptr->combos.insert(SupervisorTimeWindowPrivate::ITEM_CBO_DATE_FORMAT, comboBox);

    // time format
    label = new QLabel(trs("SupervisorTimeFormat"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(TimeSymbol::convert(TIME_FORMAT_12))
                       << trs(TimeSymbol::convert(TIME_FORMAT_24))
                      );
    itemID = static_cast<int>(SupervisorTimeWindowPrivate::ITEM_CBO_TIME_FORMAT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 1);
    d_ptr->combos.insert(SupervisorTimeWindowPrivate::ITEM_CBO_TIME_FORMAT, comboBox);

    // is display second
    label = new QLabel(trs("SupervisorDisplaySec"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("No")
                       << trs("Yes")
                      );
    itemID = static_cast<int>(SupervisorTimeWindowPrivate::ITEM_CBO_DISPLAY_SEC);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count() + d_ptr->spinBoxs.count(), 1);
    d_ptr->combos.insert(SupervisorTimeWindowPrivate::ITEM_CBO_DISPLAY_SEC, comboBox);

    layout->setRowStretch(d_ptr->combos.count() + d_ptr->spinBoxs.count(), 1);

    setWindowLayout(vlayout);

    setFixedSize(580, 580);
}

void SupervisorTimeWindow::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        SupervisorTimeWindowPrivate::MenuItem item
            = (SupervisorTimeWindowPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case SupervisorTimeWindowPrivate::ITEM_CBO_DATE_FORMAT:
            currentConfig.setNumValue("DateTime|DateFormat", index);
            break;
        case SupervisorTimeWindowPrivate::ITEM_CBO_TIME_FORMAT:
            currentConfig.setNumValue("DateTime|TimeFormat", index);
            break;
        case SupervisorTimeWindowPrivate::ITEM_CBO_DISPLAY_SEC:
            currentConfig.setNumValue("DateTime|DisplaySecond", index);
            break;
        default:
            break;
        }
    }
}

void SupervisorTimeWindow::onSpinBoxValueChanged(int value, int scale)
{
    SpinBox *spinBox = qobject_cast<SpinBox *>(sender());
    if (spinBox)
    {
        int val = value / scale;
        SupervisorTimeWindowPrivate::MenuItem item
            = (SupervisorTimeWindowPrivate::MenuItem)spinBox->property("Item").toInt();
        switch (item)
        {
        case SupervisorTimeWindowPrivate::ITEM_SPB_YEAR:
        {
            if (2 == d_ptr->spinBoxs[SupervisorTimeWindowPrivate::ITEM_SPB_MONTH]->getValue())
            {
                int min = 0;
                int max = 0;
                d_ptr->spinBoxs[SupervisorTimeWindowPrivate::ITEM_SPB_DAY]->getRange(min, max);
                int curMax = d_ptr->getMaxDay(val, 2);
                int curVal = d_ptr->spinBoxs[SupervisorTimeWindowPrivate::ITEM_SPB_DAY]->getValue();
                if (max != curMax)
                {
                    d_ptr->spinBoxs[SupervisorTimeWindowPrivate::ITEM_SPB_DAY]->setRange(min, curMax);
                }

                if (curVal > curMax)
                {
                    d_ptr->spinBoxs[SupervisorTimeWindowPrivate::ITEM_SPB_DAY]->setValue(curMax);
                }
            }
            d_ptr->setSysTime();
            break;
        }
        case SupervisorTimeWindowPrivate::ITEM_SPB_MONTH:
        {
            int min = 0;
            int max = 0;
            d_ptr->spinBoxs[SupervisorTimeWindowPrivate::ITEM_SPB_DAY]->getRange(min, max);
            int curMax = d_ptr->getMaxDay(d_ptr->spinBoxs[SupervisorTimeWindowPrivate::ITEM_SPB_YEAR]->getValue(), val);
            int curVal = d_ptr->spinBoxs[SupervisorTimeWindowPrivate::ITEM_SPB_DAY]->getValue();
            if (max != curMax)
            {
                d_ptr->spinBoxs[SupervisorTimeWindowPrivate::ITEM_SPB_DAY]->setRange(min, curMax);
            }

            if (curVal > curMax)
            {
                d_ptr->spinBoxs[SupervisorTimeWindowPrivate::ITEM_SPB_DAY]->setValue(curMax);
            }
            d_ptr->setSysTime();
            break;
        }
        case SupervisorTimeWindowPrivate::ITEM_SPB_DAY:
        case SupervisorTimeWindowPrivate::ITEM_SPB_HOUR:
        case SupervisorTimeWindowPrivate::ITEM_SPB_MINUTE:
        case SupervisorTimeWindowPrivate::ITEM_SPB_SECOND:
            d_ptr->setSysTime();
            break;
        default:
            break;
        }
    }
}
