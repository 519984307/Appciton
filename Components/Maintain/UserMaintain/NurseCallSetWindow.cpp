/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/29
 **/

#include "NurseCallSetWindow.h"
#include <QGridLayout>
#include <QLabel>
#include "ComboBox.h"
#include "Button.h"
#include "LanguageManager.h"
#include <QMap>
#include "IConfig.h"
#include "NurseCallManagerInterface.h"

#define SELECT_BUTTON_WIDTH     (48)

class NurseCallSetWindowPrivate
{
public:
    enum AlarmSelect
    {
        ALARM_LEVEL_HIGH,
        ALARM_LEVEL_MED,
        ALARM_LEVEL_LOW,
        ALARM_TYPE_TECH,
        ALARM_TYPE_PHY
    };

    NurseCallSetWindowPrivate()
        : signalCbo(NULL), contactCbo(NULL)
    {}

    void loadOptions();

    ComboBox *signalCbo;
    ComboBox *contactCbo;
    QMap<AlarmSelect, Button *> btns;
};

NurseCallSetWindow::NurseCallSetWindow()
    : Dialog(), d_ptr(new NurseCallSetWindowPrivate)
{
    setFixedSize(400, 350);
    setWindowTitle(trs("NurseCallSetup"));

    QGridLayout *layout = new QGridLayout(this);
    setWindowLayout(layout);
    layout->setMargin(10);

    QLabel *label;
    ComboBox *combo;
    int row = 0;

    // 信号类型
    label = new QLabel(trs("SignalType"));
    layout->addWidget(label, row, 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs("Continuous")
                    << trs("Pulse"));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(signalTypeComboBoxChange(int)));
    layout->addWidget(combo, row++, 1);
    d_ptr->signalCbo = combo;

    // 触发类型
    label = new QLabel(trs("ContactType"));
    layout->addWidget(label, row, 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs("NormallyClosed")
                    << trs("NormallyOpen"));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(contactTypeComboBoxChange(int)));
    layout->addWidget(combo, row++, 1);
    d_ptr->contactCbo = combo;

    label = new QLabel(trs("AlarmLev"));
    layout->addWidget(label, row, 0);

    label = new QLabel(trs("AlarmType"));
    layout->addWidget(label, row++, 1);

    int itemID = 0;
    QHBoxLayout *hLayout;
    Button *btn;
    QSize iconSize(SELECT_BUTTON_WIDTH / 10 * 9, SELECT_BUTTON_WIDTH);

    hLayout = new QHBoxLayout();
    btn = new Button();
    btn->setIconSize(iconSize);
    itemID = NurseCallSetWindowPrivate::ALARM_LEVEL_HIGH;
    btn->setProperty("Item", qVariantFromValue(itemID));
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    d_ptr->btns.insert(NurseCallSetWindowPrivate::ALARM_LEVEL_HIGH, btn);
    label = new QLabel(trs("High"));
    hLayout->addWidget(btn, 1);
    hLayout->addWidget(label, 1);
    hLayout->addStretch(2);
    layout->addLayout(hLayout, row, 0);

    hLayout = new QHBoxLayout();
    btn = new Button();
    btn->setIconSize(iconSize);
    itemID = NurseCallSetWindowPrivate::ALARM_TYPE_TECH;
    btn->setProperty("Item", qVariantFromValue(itemID));
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    d_ptr->btns.insert(NurseCallSetWindowPrivate::ALARM_TYPE_TECH, btn);
    label = new QLabel(trs("Technology"));
    hLayout->addWidget(btn, 1);
    hLayout->addWidget(label, 1);
    hLayout->addStretch(2);
    layout->addLayout(hLayout, row++, 1);

    hLayout = new QHBoxLayout();
    btn = new Button();
    btn->setIconSize(iconSize);
    itemID = NurseCallSetWindowPrivate::ALARM_LEVEL_MED;
    btn->setProperty("Item", qVariantFromValue(itemID));
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    d_ptr->btns.insert(NurseCallSetWindowPrivate::ALARM_LEVEL_MED, btn);
    label = new QLabel(trs("Med"));
    hLayout->addWidget(btn, 1);
    hLayout->addWidget(label, 1);
    hLayout->addStretch(2);
    layout->addLayout(hLayout, row, 0);

    hLayout = new QHBoxLayout();
    btn = new Button();
    btn->setIconSize(iconSize);
    itemID = NurseCallSetWindowPrivate::ALARM_TYPE_PHY;
    btn->setProperty("Item", qVariantFromValue(itemID));
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    d_ptr->btns.insert(NurseCallSetWindowPrivate::ALARM_TYPE_PHY, btn);
    label = new QLabel(trs("Physiology"));
    hLayout->addWidget(btn, 1);
    hLayout->addWidget(label, 1);
    hLayout->addStretch(2);
    layout->addLayout(hLayout, row++, 1);

    hLayout = new QHBoxLayout();
    btn = new Button();
    btn->setIconSize(iconSize);
    itemID = NurseCallSetWindowPrivate::ALARM_LEVEL_LOW;
    btn->setProperty("Item", qVariantFromValue(itemID));
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    d_ptr->btns.insert(NurseCallSetWindowPrivate::ALARM_LEVEL_LOW, btn);
    label = new QLabel(trs("Low"));
    hLayout->addWidget(btn, 1);
    hLayout->addWidget(label, 1);
    hLayout->addStretch(2);
    layout->addLayout(hLayout, row++, 0);

    layout->setRowStretch(row, 1);
}

NurseCallSetWindow::~NurseCallSetWindow()
{
    delete d_ptr;
}

void NurseCallSetWindow::showEvent(QShowEvent *ev)
{
    Q_UNUSED(ev)
    d_ptr->loadOptions();
}

void NurseCallSetWindow::signalTypeComboBoxChange(int index)
{
    systemConfig.setNumValue("Others|SignalType", index);
    NurseCallManagerInterface *nurseCallManager = NurseCallManagerInterface::getNurseCallManagerInterface();
    if (nurseCallManager)
    {
        nurseCallManager->upDateCallSta();
    }
}

void NurseCallSetWindow::contactTypeComboBoxChange(int index)
{
    systemConfig.setNumValue("Others|TriggerMode", index);
}

void NurseCallSetWindow::onBtnReleased()
{
    Button *btn = qobject_cast<Button *>(sender());
    int itemID = btn->property("Item").toInt();
    QIcon icon("/usr/local/nPM/icons/Checked.png");
    QString itemPath;

    switch (itemID)
    {
    case NurseCallSetWindowPrivate::ALARM_LEVEL_HIGH:
        itemPath = "Others|AlarmLevel|High";
        break;
    case NurseCallSetWindowPrivate::ALARM_LEVEL_MED:
        itemPath = "Others|AlarmLevel|Med";
        break;
    case NurseCallSetWindowPrivate::ALARM_LEVEL_LOW:
        itemPath = "Others|AlarmLevel|Low";
        break;
    case NurseCallSetWindowPrivate::ALARM_TYPE_TECH:
        itemPath = "Others|AlarmType|Technology";
        break;
    case NurseCallSetWindowPrivate::ALARM_TYPE_PHY:
        itemPath = "Others|AlarmType|Physiology";
        break;
    default:
        break;
    }
    int index = !btn->property("Index").toInt();
    if (index)
    {
        btn->setIcon(icon);
    }
    else
    {
        btn->setIcon(QIcon());
    }
    btn->setProperty("Index", qVariantFromValue(index));
    systemConfig.setNumValue(itemPath, index);
    NurseCallManagerInterface *nurseCallManager = NurseCallManagerInterface::getNurseCallManagerInterface();
    if (nurseCallManager)
    {
        nurseCallManager->upDateCallSta();
    }
}

void NurseCallSetWindowPrivate::loadOptions()
{
    int index = 0;
    systemConfig.getNumValue("Others|SignalType", index);
    signalCbo->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("Others|TriggerMode", index);
    contactCbo->setCurrentIndex(index);

    QIcon icon("/usr/local/nPM/icons/Checked.png");

    index = 0;
    systemConfig.getNumValue("Others|AlarmLevel|High", index);
    btns[ALARM_LEVEL_HIGH]->setProperty("Index", qVariantFromValue(index));
    if (index)
    {
        btns[ALARM_LEVEL_HIGH]->setIcon(icon);
    }
    else
    {
        btns[ALARM_LEVEL_HIGH]->setIcon(QIcon());
    }

    index = 0;
    systemConfig.getNumValue("Others|AlarmLevel|Med", index);
    btns[ALARM_LEVEL_MED]->setProperty("Index", qVariantFromValue(index));
    if (index)
    {
        btns[ALARM_LEVEL_MED]->setIcon(icon);
    }
    else
    {
        btns[ALARM_LEVEL_MED]->setIcon(QIcon());
    }

    index = 0;
    systemConfig.getNumValue("Others|AlarmLevel|Low", index);
    btns[ALARM_LEVEL_LOW]->setProperty("Index", qVariantFromValue(index));
    if (index)
    {
        btns[ALARM_LEVEL_LOW]->setIcon(icon);
    }
    else
    {
        btns[ALARM_LEVEL_LOW]->setIcon(QIcon());
    }

    index = 0;
    systemConfig.getNumValue("Others|AlarmType|Technology", index);
    btns[ALARM_TYPE_TECH]->setProperty("Index", qVariantFromValue(index));
    if (index)
    {
        btns[ALARM_TYPE_TECH]->setIcon(icon);
    }
    else
    {
        btns[ALARM_TYPE_TECH]->setIcon(QIcon());
    }

    index = 0;
    systemConfig.getNumValue("Others|AlarmType|Physiology", index);
    btns[ALARM_TYPE_PHY]->setProperty("Index", qVariantFromValue(index));
    if (index)
    {
        btns[ALARM_TYPE_PHY]->setIcon(icon);
    }
    else
    {
        btns[ALARM_TYPE_PHY]->setIcon(QIcon());
    }
}
