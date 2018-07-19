/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/18
 **/

#include "SupervisorTimeMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "IConfig.h"
#include "NumberInput.h"
#include "IMessageBox.h"
#include "Button.h"
#include "TimeSymbol.h"

class SupervisorTimeMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_YEAR,
        ITEM_BTN_MONTH,
        ITEM_BTN_DAY,
        ITEM_BTN_HOUR,
        ITEM_BTN_MINUTE,
        ITEM_BTN_SECOND,
        ITEM_CBO_DATE_FORMAT,
        ITEM_CBO_TIME_FORMAT,
        ITEM_CBO_DISPLAY_SEC
    };

    SupervisorTimeMenuContentPrivate() {}

    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> buttons;
};

void SupervisorTimeMenuContentPrivate::loadOptions()
{
}

SupervisorTimeMenuContent::SupervisorTimeMenuContent()
    : MenuContent(trs("SupervisorTimeAndDataMenu"), trs("SupervisorTimeAndDataMenuDesc")),
      d_ptr(new SupervisorTimeMenuContentPrivate)
{
}

SupervisorTimeMenuContent::~SupervisorTimeMenuContent()
{
    delete d_ptr;
}

void SupervisorTimeMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void SupervisorTimeMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    Button *button;
    int itemID;

    // year
    label = new QLabel(trs("SupervisorYear"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(SupervisorTimeMenuContentPrivate::ITEM_BTN_YEAR);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(SupervisorTimeMenuContentPrivate::ITEM_BTN_YEAR, button);

    // month
    label = new QLabel(trs("SupervisorMonth"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(SupervisorTimeMenuContentPrivate::ITEM_BTN_MONTH);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(SupervisorTimeMenuContentPrivate::ITEM_BTN_MONTH, button);

    // day
    label = new QLabel(trs("SupervisorDay"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(SupervisorTimeMenuContentPrivate::ITEM_BTN_DAY);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(SupervisorTimeMenuContentPrivate::ITEM_BTN_DAY, button);

    // hour
    label = new QLabel(trs("SupervisorHour"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(SupervisorTimeMenuContentPrivate::ITEM_BTN_HOUR);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(SupervisorTimeMenuContentPrivate::ITEM_BTN_HOUR, button);

    // minute
    label = new QLabel(trs("SupervisorMinute"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(SupervisorTimeMenuContentPrivate::ITEM_BTN_MINUTE);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(SupervisorTimeMenuContentPrivate::ITEM_BTN_MINUTE, button);

    // second
    label = new QLabel(trs("SupervisorSecond"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(SupervisorTimeMenuContentPrivate::ITEM_BTN_SECOND);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(SupervisorTimeMenuContentPrivate::ITEM_BTN_SECOND, button);

    // date format
    label = new QLabel(trs("SupervisorDateFormat"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(TimeSymbol::convert(DATE_FORMAT_Y_M_D))
                       << trs(TimeSymbol::convert(DATE_FORMAT_M_D_Y))
                       << trs(TimeSymbol::convert(DATE_FORMAT_D_M_Y))
                      );
    itemID = static_cast<int>(SupervisorTimeMenuContentPrivate::ITEM_CBO_DATE_FORMAT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->combos.insert(SupervisorTimeMenuContentPrivate::ITEM_CBO_DATE_FORMAT, comboBox);

    // time format
    label = new QLabel(trs("SupervisorTimeFormat"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(TimeSymbol::convert(TIME_FORMAT_12))
                       << trs(TimeSymbol::convert(TIME_FORMAT_24))
                      );
    itemID = static_cast<int>(SupervisorTimeMenuContentPrivate::ITEM_CBO_TIME_FORMAT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->combos.insert(SupervisorTimeMenuContentPrivate::ITEM_CBO_TIME_FORMAT, comboBox);

    // is display second
    label = new QLabel(trs("SupervisorDisplaySec"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("No")
                       << trs("Yes")
                      );
    itemID = static_cast<int>(SupervisorTimeMenuContentPrivate::ITEM_CBO_DISPLAY_SEC);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->combos.insert(SupervisorTimeMenuContentPrivate::ITEM_CBO_DISPLAY_SEC, comboBox);

    layout->setRowStretch(d_ptr->combos.count() + d_ptr->buttons.count(), 1);
}

void SupervisorTimeMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        SupervisorTimeMenuContentPrivate::MenuItem item
                = (SupervisorTimeMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case SupervisorTimeMenuContentPrivate::ITEM_CBO_DATE_FORMAT:
            currentConfig.setNumValue("DateTime|DateFormat", index);
            break;
        case SupervisorTimeMenuContentPrivate::ITEM_CBO_TIME_FORMAT:
            currentConfig.setNumValue("DateTime|TimeFormat", index);
            break;
        case SupervisorTimeMenuContentPrivate::ITEM_CBO_DISPLAY_SEC:
            currentConfig.setNumValue("DateTime|DisplaySecond", index);
            break;
        default:
            break;
        }
    }
}

void SupervisorTimeMenuContent::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        SupervisorTimeMenuContentPrivate::MenuItem item
                = (SupervisorTimeMenuContentPrivate::MenuItem)button->property("Item").toInt();
        switch (item)
        {
        case SupervisorTimeMenuContentPrivate::ITEM_BTN_YEAR:

            break;
        case SupervisorTimeMenuContentPrivate::ITEM_BTN_MONTH:

            break;
        case SupervisorTimeMenuContentPrivate::ITEM_BTN_DAY:

            break;
        case SupervisorTimeMenuContentPrivate::ITEM_BTN_HOUR:

            break;
        case SupervisorTimeMenuContentPrivate::ITEM_BTN_MINUTE:

            break;
        case SupervisorTimeMenuContentPrivate::ITEM_BTN_SECOND:

            break;
        default:
            break;
        }
    }
}
