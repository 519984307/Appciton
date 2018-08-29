/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/10
 **/

#include "AGMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "AGSymbol.h"
#include "IConfig.h"
#include "AGParam.h"
#include "CO2SetAGMenu.h"
#include "N2OSetAGMenu.h"
#include "HalSetAGMenu.h"
#include "Button.h"
#include "MainMenuWindow.h"

class AGMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_MODULE_SWITCH = 1,
        ITEM_CBO_CO2_SET,
        ITEM_CBO_N2O_SET,
        ITEM_CBO_AA1_SET,
        ITEM_CBO_AA2_SET,
        ITEM_CBO_O2_SET,
        ITEM_CBO_SWEEP_SPEED
    };

    AGMenuContentPrivate()
        : isEnable(true)
    {}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    bool isEnable;                      //模块使能状态位
};

void AGMenuContentPrivate::loadOptions()
{
    combos[ITEM_CBO_MODULE_SWITCH]->setCurrentIndex(0);
    combos[ITEM_CBO_CO2_SET]->setCurrentIndex(0);
    combos[ITEM_CBO_N2O_SET]->setCurrentIndex(0);
    combos[ITEM_CBO_AA1_SET]->setCurrentIndex(0);
    combos[ITEM_CBO_AA2_SET]->setCurrentIndex(0);
    combos[ITEM_CBO_O2_SET]->setCurrentIndex(0);
    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(agParam.getSweepSpeed());
}

AGMenuContent::AGMenuContent()
    : MenuContent(trs("AGMenu"), trs("AGMenuDesc")),
    d_ptr(new AGMenuContentPrivate)
{
}

AGMenuContent::~AGMenuContent()
{
    delete d_ptr;
}

void AGMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void AGMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // Module Switch
    label = new QLabel(trs("AGModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                      << trs(AGSymbol::convert(AG_MODULE_SWITCH_ENABLE))
                      << trs(AGSymbol::convert(AG_MODULE_SWITCH_DISABLE))
                      );
    itemID = static_cast<int>(AGMenuContentPrivate::ITEM_CBO_MODULE_SWITCH);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AGMenuContentPrivate::ITEM_CBO_MODULE_SWITCH, comboBox);

    // CO2 Set
    label = new QLabel(trs("CO2"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                      << trs(AGSymbol::convert(AG_MODULE_CO2_OPTION_SET))
                      );
    itemID = static_cast<int>(AGMenuContentPrivate::ITEM_CBO_CO2_SET);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AGMenuContentPrivate::ITEM_CBO_CO2_SET, comboBox);

    // N2O Set
    label = new QLabel(trs("N2O"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                      << trs(AGSymbol::convert(AG_MODULE_N2O_OPTION_SET))
                      );
    itemID = static_cast<int>(AGMenuContentPrivate::ITEM_CBO_N2O_SET);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AGMenuContentPrivate::ITEM_CBO_N2O_SET, comboBox);

    // AA1 Set
    label = new QLabel(trs("AA1"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                      << trs(AGSymbol::convert(AG_MODULE_AA1_OPTION_SET))
                      );
    itemID = static_cast<int>(AGMenuContentPrivate::ITEM_CBO_AA1_SET);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AGMenuContentPrivate::ITEM_CBO_AA1_SET, comboBox);

    // AA2 Set
    label = new QLabel(trs("AA2"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                      << trs(AGSymbol::convert(AG_MODULE_AA2_OPTION_SET))
                      );
    itemID = static_cast<int>(AGMenuContentPrivate::ITEM_CBO_AA2_SET);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AGMenuContentPrivate::ITEM_CBO_AA2_SET, comboBox);

    // O2 Set
    label = new QLabel(trs("O2"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                      << trs(AGSymbol::convert(AG_MODULE_O2_OPTION_SET))
                      );
    itemID = static_cast<int>(AGMenuContentPrivate::ITEM_CBO_O2_SET);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AGMenuContentPrivate::ITEM_CBO_O2_SET, comboBox);

    // Sweep Speed
    label = new QLabel(trs("AGSweepSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                      << trs(AGSymbol::convert(AG_SWEEP_SPEED_62_5))
                      << trs(AGSymbol::convert(AG_SWEEP_SPEED_125))
                      << trs(AGSymbol::convert(AG_SWEEP_SPEED_250))
                      << trs(AGSymbol::convert(AG_SWEEP_SPEED_500))
                      );
    itemID = static_cast<int>(AGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(AGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED, comboBox);

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, d_ptr->combos.count(), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch((d_ptr->combos.count() + 1), 1);
}

void AGMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    CO2SetAGMenu co2SetAGMenu;
    N2OSetAGMenu n2oSetAGMenu;
    HalSetAGMenu halSetAGMenu;
    if (box)
    {
        AGMenuContentPrivate::MenuItem item
            = (AGMenuContentPrivate::MenuItem) box->property("Item").toInt();
        switch (item)
        {
        case AGMenuContentPrivate::ITEM_CBO_MODULE_SWITCH:
            if ((!index) != d_ptr->isEnable)
            {
                d_ptr->combos.value(AGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED)->setEnabled(!index);
                d_ptr->combos.value(AGMenuContentPrivate::ITEM_CBO_CO2_SET)->setEnabled(!index);
                d_ptr->combos.value(AGMenuContentPrivate::ITEM_CBO_N2O_SET)->setEnabled(!index);
                d_ptr->combos.value(AGMenuContentPrivate::ITEM_CBO_AA1_SET)->setEnabled(!index);
                d_ptr->combos.value(AGMenuContentPrivate::ITEM_CBO_AA2_SET)->setEnabled(!index);
                d_ptr->combos.value(AGMenuContentPrivate::ITEM_CBO_O2_SET)->setEnabled(!index);
                d_ptr->isEnable = (!index);
            }
            systemConfig.setNumValue("PrimaryCfg|AG|AGModule", index);
            break;
        case AGMenuContentPrivate::ITEM_CBO_CO2_SET:
            co2SetAGMenu.exec();
            break;
        case AGMenuContentPrivate::ITEM_CBO_N2O_SET:
            n2oSetAGMenu.exec();
            break;
        case AGMenuContentPrivate::ITEM_CBO_AA1_SET:
            halSetAGMenu.exec();
            break;
        case AGMenuContentPrivate::ITEM_CBO_AA2_SET:
            break;
        case AGMenuContentPrivate::ITEM_CBO_O2_SET:
            break;
        case AGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED:
            agParam.setSweepSpeed((AGSweepSpeed)index);
            break;
        default:
            break;
        }
    }
}

void AGMenuContent::onAlarmBtnReleased()
{
    MenuWindow *w = this->getMenuWindow();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_ETCO2, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}
