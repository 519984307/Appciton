/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include "ConfigEditRespMenu.h"
#include "ConfigEditMenuGrp.h"
#include "IComboList.h"
#include "RESPSymbol.h"
#include "ConfigEditAlarmLimitMenu.h"
#include "LabelButton.h"
#include "ConfigManager.h"
class ConfigEditRespMenuPrivate
{
public:
    enum ComboListId
    {
        ApneaDelay,
        BreathLead,
        Gain,
        SweepSpeed,
        ComboListMax,
    };

    ConfigEditRespMenuPrivate():combos(NULL),
        comboLabels(NULL),
        _alarmLbtn(NULL)
    {
        memset(combos, 0, sizeof(combos));

        //常量存放在静态变量区
        comboLabels[ApneaDelay] = "ApneaDelay";
        comboLabels[BreathLead] = "BreathLead";
        comboLabels[Gain] = "Gain";
        comboLabels[SweepSpeed] = "SweepSpeed";
    }

    void loadOptions();

    IComboList *combos[ComboListMax];
    const char *comboLabels[ComboListMax];
    LabelButton *_alarmLbtn;            //跳到报警项设置按钮
};

void ConfigEditRespMenuPrivate::loadOptions()
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    int index  = 0;

    if (combos[ApneaDelay]->count() == 0)
    {
        combos[ApneaDelay]->addItem(trs("OFF"));
        for (int i = 1; i < RESP_APNEA_TIME_NR; i++)
        {
            int time = 15 + 5 * i;
            combos[ApneaDelay]->addItem(QString::number(time) + " s");
        }
        config->getNumValue("RESP|ApneaTime", index);
        combos[ApneaDelay]->setCurrentIndex(index);
    }

    if (combos[BreathLead]->count() == 0)
    {
        for (int i = 0; i < RESP_LEAD_NR; i++)
        {
            combos[BreathLead]->addItem(RESPSymbol::convert((RESPLead)i));
        }
        config->getNumValue("RESP|BreathLead", index);
        combos[BreathLead]->setCurrentIndex(index);
    }

    if (combos[Gain]->count() == 0)
    {
        for (int i = 0; i < RESP_ZOOM_NR; i++)
        {
            combos[Gain]->addItem(RESPSymbol::convert((RESPZoom)i));
        }
        config->getNumValue("RESP|Gain", index);
        combos[Gain]->setCurrentIndex(index);
    }

    if (combos[SweepSpeed]->count() == 0)
    {
        for (int i = 0; i < RESP_SWEEP_SPEED_NR; i++)
        {
            combos[SweepSpeed]->addItem(RESPSymbol::convert((RESPSweepSpeed)i));
        }
        config->getNumValue("RESP|SweepSpeed", index);
        combos[SweepSpeed]->setCurrentIndex(index);
    }
}

ConfigEditRespMenu::ConfigEditRespMenu()
    : SubMenu(trs("RespMenu")),
      d_ptr(new ConfigEditRespMenuPrivate())
{
    setDesc(trs("RespMenuDesc"));
    startLayout();
}

ConfigEditRespMenu::~ConfigEditRespMenu()
{
}

void ConfigEditRespMenu::layoutExec()
{
    int submenuW = configEditMenuGrp.getSubmenuWidth();
    int submenuH = configEditMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    for (int i = 0; i < ConfigEditRespMenuPrivate::ComboListMax; i++)
    {
        IComboList *combo = new IComboList(trs(d_ptr->comboLabels[i]));
        combo->setFont(defaultFont());
        combo->label->setFixedSize(labelWidth, ITEM_H);
        combo->combolist->setFixedSize(btnWidth, ITEM_H);
        connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
        combo->setProperty("comboId", qVariantFromValue(i));
        mainLayout->addWidget(combo);
        d_ptr->combos[i] = combo;
    }

    d_ptr->_alarmLbtn = new LabelButton("");
    d_ptr->_alarmLbtn->setFont(defaultFont());
    d_ptr->_alarmLbtn->label->setFixedSize(labelWidth, ITEM_H);
    d_ptr->_alarmLbtn->button->setFixedSize(btnWidth, ITEM_H);
    d_ptr->_alarmLbtn->button->setText(trs("AlarmLimitSetUp"));
    connect(d_ptr->_alarmLbtn->button, SIGNAL(realReleased()), this, SLOT(_alarmLbtnSlot()));
    mainLayout->addWidget(d_ptr->_alarmLbtn);

    mainLayout->addStretch(1);
}
// 报警项设置
void ConfigEditRespMenu::_alarmLbtnSlot()
{
    SubMenu *subMenuPrevious = (configEditMenuGrp.getCurrentEditConfigItem())["ConfigEditRespMenu"];
    SubMenu *subMenuCurrent = (configEditMenuGrp.getCurrentEditConfigItem())["ConfigEditAlarmLimitMenu"];
    ConfigEditAlarmLimitMenu *alarmLimit = qobject_cast<ConfigEditAlarmLimitMenu *>(subMenuCurrent);
    alarmLimit->setFocusIndex(SUB_PARAM_NONE + 1);
    configEditMenuGrp.changePage(subMenuCurrent, subMenuPrevious);
}

void ConfigEditRespMenu::readyShow()
{
    d_ptr->loadOptions();

    bool preStatusBool = !configManager.getWidgetsPreStatus();

    for (int i = 0; i < ConfigEditRespMenuPrivate::ComboListMax; i++)
    {
        d_ptr->combos[i]->setEnabled(preStatusBool);
    }
}

void ConfigEditRespMenu::onComboListConfigChanged(int index)
{
    IComboList *combo = qobject_cast<IComboList *>(sender());
    if (!combo)
    {
        qdebug("Invalid signal sender.");
        return;
    }

    ConfigEditRespMenuPrivate::ComboListId comboId = (ConfigEditRespMenuPrivate::ComboListId)
            combo->property("comboId").toInt();
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    switch (comboId)
    {

    case ConfigEditRespMenuPrivate::ApneaDelay:
        config->setNumValue("RESP|ApneaDelay", index);
        break;
    case ConfigEditRespMenuPrivate::BreathLead:
        config->setNumValue("RESP|BreathLead", index);
        break;
    case ConfigEditRespMenuPrivate::Gain:
        config->setNumValue("RESP|Gain", index);
        break;
    case ConfigEditRespMenuPrivate::SweepSpeed:
        config->setNumValue("RESP|SweepSpeed", index);
        break;
    default:
        qdebug("Invalid combo id.");
        break;
    }
}
