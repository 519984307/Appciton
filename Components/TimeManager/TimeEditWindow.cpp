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
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "IConfig.h"
#include "SpinBox.h"
#include "Framework/Language/LanguageManager.h"
#include "Framework/TimeDate/TimeSymbol.h"
#include "Framework/TimeDate/TimeDate.h"
#include <QProcess>
#include <QVBoxLayout>
#include "TimeManager.h"
#include "SystemManager.h"
#include "SystemTick.h"
#include <QDateTime>
#include "PatientManager.h"
#include "NIBPCountdownTime.h"
#include "NIBPParam.h"
#include "TrendDataStorageManager.h"
#include "MessageBox.h"
#include "WindowManager.h"
#include "ThemeManager.h"

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
        : oldTime(0)
    {}

    /**
     * @brief loadOptions
     */
    void loadOptions();

    /**
     * @brief setSysTime
     */
    void setSysTime();

    /**
     * @brief getSetupTime 获取设置的时间
     * @return
     */
    QDateTime getSetupTime();

    /**
     * @brief updateHourItem
     */
    void updateHourItem();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, SpinBox *> spinBoxs;
    unsigned oldTime;
};

void TimeEditWindowPrivate::loadOptions()
{
    oldTime = timeDate->time();
    spinBoxs[ITEM_SPB_YEAR]->setValue(timeDate->getDateYear());
    spinBoxs[ITEM_SPB_MONTH]->setValue(timeDate->getDateMonth());
    spinBoxs[ITEM_SPB_DAY]->setValue(timeDate->getDateDay());
    spinBoxs[ITEM_SPB_HOUR]->setValue(timeDate->getTimeHour());
    spinBoxs[ITEM_SPB_MINUTE]->setValue(timeDate->getTimeMinute());
    spinBoxs[ITEM_SPB_SECOND]->setValue(timeDate->getTimeSeconds());

    int value = 0;
    value = systemManager.getSystemDateFormat();
    combos[ITEM_CBO_DATE_FORMAT]->setCurrentIndex(value);

    value = systemManager.getSystemTimeFormat();
    combos[ITEM_CBO_TIME_FORMAT]->setCurrentIndex(value);

    systemConfig.getNumValue("DateTime|DisplaySecond", value);
    combos[ITEM_CBO_DISPLAY_SEC]->setCurrentIndex(value);
}

void TimeEditWindowPrivate::setSysTime()
{
    timeManager.setSystemTime(getSetupTime());
}

QDateTime TimeEditWindowPrivate::getSetupTime()
{
    int y = spinBoxs[ITEM_SPB_YEAR]->getValue();
    int mon = spinBoxs[ITEM_SPB_MONTH]->getValue();
    int d = spinBoxs[ITEM_SPB_DAY]->getValue();
    int h = spinBoxs[ITEM_SPB_HOUR]->getValue();
    int m = spinBoxs[ITEM_SPB_MINUTE]->getValue();
    int s = spinBoxs[ITEM_SPB_SECOND]->getValue();

    QDateTime dt(QDate(y, mon, d), QTime(h, m, s));
    return dt;
}

void TimeEditWindowPrivate::updateHourItem()
{
    QStringList hourList;
    TimeFormat timeFormat = static_cast<TimeFormat>(combos[ITEM_CBO_TIME_FORMAT]->currentIndex());
    if (timeFormat == TIME_FORMAT_12)
    {
        // 设置时间的字符串列表 12AM-11AM 12PM-11PM
        hourList.append("12 AM");
        for (int i = 1; i < 12; i++)
        {
            hourList.append(QString("%1 AM").arg(QString::number(i)));
        }
        hourList.append("12 PM");
        for (int i = 1; i < 12; i++)
        {
            hourList.append(QString("%1 PM").arg(QString::number(i)));
        }

        spinBoxs[ITEM_SPB_HOUR]->setSpinBoxStyle(SpinBox::SPIN_BOX_STYLE_STRING);
        spinBoxs[ITEM_SPB_HOUR]->setStringList(hourList);
    }
    else
    {
        spinBoxs[ITEM_SPB_HOUR]->setSpinBoxStyle(SpinBox::SPIN_BOX_STYLE_NUMBER);
    }
    spinBoxs[ITEM_SPB_HOUR]->update();
}

TimeEditWindow::TimeEditWindow()
    : Dialog(),
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
    d_ptr->updateHourItem();
    bool timeEditable = systemManager.getCurWorkMode() != WORK_MODE_DEMO;
    for (int i = TimeEditWindowPrivate::ITEM_SPB_YEAR; i <= TimeEditWindowPrivate::ITEM_SPB_SECOND; i++ )
    {
        d_ptr->spinBoxs[static_cast<TimeEditWindowPrivate::MenuItem>(i)]->setEnabled(timeEditable);
    }
}

void TimeEditWindow::layoutExec()
{
    setWindowTitle(trs("SupervisorTimeAndDataMenu"));

    QGridLayout *layout = new QGridLayout;
    layout->setMargin(10);
    layout->setSpacing(10);

    ComboBox *comboBox;
    QLabel *label;
    SpinBox *spinBox;
    int itemID;

    // date
    label = new QLabel(trs("Date"));
    layout->addWidget(label, (d_ptr->combos.count() + d_ptr->spinBoxs.count())/3, 0);

    // year
    spinBox = new SpinBox;
    spinBox->setFixedHeight(themeManger.getAcceptableControlHeight());
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
    spinBox->setFixedHeight(themeManger.getAcceptableControlHeight());
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
    spinBox->setFixedHeight(themeManger.getAcceptableControlHeight());
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
    spinBox->setFixedHeight(themeManger.getAcceptableControlHeight());
    spinBox->setRange(0, 23);
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
    spinBox->setFixedHeight(themeManger.getAcceptableControlHeight());
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
    spinBox->setFixedHeight(themeManger.getAcceptableControlHeight());
    spinBox->setRange(0, 59);
    spinBox->setScale(1);
    spinBox->setStep(1);
    spinBox->setArrow(false);
    itemID = static_cast<int>(TimeEditWindowPrivate::ITEM_SPB_SECOND);
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    layout->addWidget(spinBox, (d_ptr->combos.count() + d_ptr->spinBoxs.count())/3, 3);
    d_ptr->spinBoxs.insert(TimeEditWindowPrivate::ITEM_SPB_SECOND, spinBox);

    // date format
    label = new QLabel(trs("SupervisorDateFormat"));
    layout->addWidget(label, (d_ptr->combos.count() + d_ptr->spinBoxs.count())/3, 0);
    comboBox = new ComboBox();
    comboBox->setFixedHeight(themeManger.getAcceptableControlHeight());
    comboBox->addItems(QStringList()
                       << trs(TimeSymbol::convert(DATE_FORMAT_Y_M_D))
                       << trs(TimeSymbol::convert(DATE_FORMAT_M_D_Y))
                       << trs(TimeSymbol::convert(DATE_FORMAT_D_M_Y)));
    itemID = static_cast<int>(TimeEditWindowPrivate::ITEM_CBO_DATE_FORMAT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, (d_ptr->combos.count() + d_ptr->spinBoxs.count()) / 3, 1, 1, 3);
    d_ptr->combos.insert(TimeEditWindowPrivate::ITEM_CBO_DATE_FORMAT, comboBox);

    // time format
    label = new QLabel(trs("SupervisorTimeFormat"));
    layout->addWidget(label, (d_ptr->combos.count() + d_ptr->spinBoxs.count()) / 3 + 1, 0);
    comboBox = new ComboBox();
    comboBox->setFixedHeight(themeManger.getAcceptableControlHeight());
    comboBox->addItems(QStringList()
                       << trs(TimeSymbol::convert(TIME_FORMAT_12))
                       << trs(TimeSymbol::convert(TIME_FORMAT_24)));
    itemID = static_cast<int>(TimeEditWindowPrivate::ITEM_CBO_TIME_FORMAT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, (d_ptr->combos.count() + d_ptr->spinBoxs.count()) / 3 + 1, 1, 1, 3);
    d_ptr->combos.insert(TimeEditWindowPrivate::ITEM_CBO_TIME_FORMAT, comboBox);

    // is display second
    label = new QLabel(trs("SupervisorDisplaySec"));
    layout->addWidget(label, (d_ptr->combos.count() + d_ptr->spinBoxs.count()) / 3 + 2, 0);
    comboBox = new ComboBox();
    comboBox->setFixedHeight(themeManger.getAcceptableControlHeight());
    comboBox->addItems(QStringList()
                       << trs("No")
                       << trs("Yes"));
    itemID = static_cast<int>(TimeEditWindowPrivate::ITEM_CBO_DISPLAY_SEC);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, (d_ptr->combos.count() + d_ptr->spinBoxs.count()) / 3 + 2, 1, 1, 3);
    d_ptr->combos.insert(TimeEditWindowPrivate::ITEM_CBO_DISPLAY_SEC, comboBox);

    layout->setRowStretch(layout->rowCount(), 1);
    layout->setColumnStretch(0, 2);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(2, 1);
    layout->setColumnStretch(3, 1);
    setWindowLayout(layout);

    setFixedSize(windowManager.getPopWindowWidth(), windowManager.getPopWindowHeight());
}

void TimeEditWindow::hideEvent(QHideEvent *ev)
{
    Dialog::hideEvent(ev);
    QDateTime dt = d_ptr->getSetupTime();
    if (d_ptr->oldTime != dt.toTime_t())
    {
        MessageBox msg(trs("Prompt"), trs("ChangeTime"), true, true);
        windowManager.showWindow(&msg, WindowManager::ShowBehaviorModal);
        if (msg.result() == QDialog::Accepted)
        {
            trendDataStorageManager.stopPeriodRun();
            d_ptr->setSysTime();
            systemTick.resetLastTime();
            patientManager.newPatient();
            if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
            {
                nibpCountdownTime.timeChange(true);
            }
            trendDataStorageManager.restartPeriodRun();
        }
    }
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
            systemManager.setSystemDateFormat(static_cast<DateFormat>(index));
            break;
        case TimeEditWindowPrivate::ITEM_CBO_TIME_FORMAT:
            systemManager.setSystemTimeFormat(static_cast<TimeFormat>(index));
            d_ptr->updateHourItem();
            break;
        case TimeEditWindowPrivate::ITEM_CBO_DISPLAY_SEC:
            systemConfig.setNumValue("DateTime|DisplaySecond", index);
            timeManager.roloadConfig();
            break;
        default:
            break;
        }
    }

    // 限制最小时间为1970-1-1 8:0:0
    QDate curDate(d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_YEAR]->getValue(),
               d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_MONTH]->getValue(),
               d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->getValue());
    if (curDate == QDate(1970, 1, 1))
    {
        d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_HOUR]->setRange(8, 23);
        if (d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_HOUR]->getValue() < 8)
        {
            d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_HOUR]->setValue(8);
        }
    }
    else
    {
        d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_HOUR]->setRange(0, 23);
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
                QDate date(val, 2, 1);
                int curMax = date.daysInMonth();
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
            break;
        }
        case TimeEditWindowPrivate::ITEM_SPB_MONTH:
        {
            int min = 0;
            int max = 0;
            d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->getRange(min, max);
            QDate date(d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_YEAR]->getValue(), val, 1);
            int curMax = date.daysInMonth();
            int curVal = d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->getValue();
            if (max != curMax)
            {
                d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->setRange(min, curMax);
            }

            if (curVal > curMax)
            {
                d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->setValue(curMax);
            }
            break;
        }
        case TimeEditWindowPrivate::ITEM_SPB_DAY:
        case TimeEditWindowPrivate::ITEM_SPB_HOUR:
        case TimeEditWindowPrivate::ITEM_SPB_MINUTE:
        case TimeEditWindowPrivate::ITEM_SPB_SECOND:
            break;
        default:
            break;
        }

        // 限制最小时间为1970-1-1 8:0:0
        QDate curDate(d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_YEAR]->getValue(),
                   d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_MONTH]->getValue(),
                   d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_DAY]->getValue());
        if (curDate == QDate(1970, 1, 1))
        {
            d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_HOUR]->setRange(8, 23);
            if (d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_HOUR]->getValue() < 8)
            {
                d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_HOUR]->setValue(8);
            }
        }
        else
        {
            d_ptr->spinBoxs[TimeEditWindowPrivate::ITEM_SPB_HOUR]->setRange(0, 23);
        }
    }
}
