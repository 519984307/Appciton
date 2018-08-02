/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/2
 **/


#include "ConfigEditEcgMenu.h"
#include "ConfigEditMenuGrp.h"
#include "IComboList.h"
#include "ECGSymbol.h"
#include "SoundManager.h"
#include "LabelButton.h"
#include "ConfigEditAlarmLimitMenu.h"
#include "LoadConfigMenu.h"
#include "ConfigManager.h"
// 配置编辑ECG菜单私有类
class ConfigEditEcgMenuPrivate
{
public:
    enum ComboListId
    {
        AlarmSource,
        Ecg1Wave,
        Ecg2Wave,
        Ecg1Gain,
        Ecg2Gain,
        SweepSpeed,
        Filter,
        HeartBeatVol,
        LeadMode,
        NotchFilter,
        SmartLeadOff,
        ComboListMax
    };

    ConfigEditEcgMenuPrivate()
    {
        memset(combos, 0, sizeof(combos));

        comboLables[AlarmSource] = "AlarmSource";
        comboLables[Ecg1Wave] = "Ecg1";
        comboLables[Ecg2Wave] = "Ecg2";
        comboLables[Ecg1Gain] = "Ecg1Gain";
        comboLables[Ecg2Gain] = "Ecg2Gain";
        comboLables[SweepSpeed] = "SweepSpeed";
        comboLables[Filter] = "Filter";
        comboLables[HeartBeatVol] = "HeatBeatVol";
        comboLables[LeadMode] = "LeadMode";
        comboLables[NotchFilter] = "NotchFilter";
        comboLables[SmartLeadOff] = "SmartLeadOff";
        _alarmLbtn = NULL;
    }
    void loadOptions();
    IComboList *combos[ComboListMax];
    const char *comboLables[ComboListMax];
    LabelButton *_alarmLbtn;            //跳到报警项设置按钮
};

void ConfigEditEcgMenuPrivate::loadOptions()
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();

    int index = 0;
    if (combos[AlarmSource]->count() == 0)
    {
        for (int i = 0; i < ECG_ALARM_SRC_NR; i++)
        {
            combos[AlarmSource]->addItem(trs(ECGSymbol::convert((ECGAlarmSource)i)));
        }
        config->getNumValue("ECG|AlarmSource", index);
        combos[AlarmSource]->setCurrentIndex(index);
    }

    int leadmode = 0;
    config->getNumValue("ECG|LeadMode", leadmode);
    combos[LeadMode]->clear();
    for (int i = 0; i < ECG_LEAD_MODE_NR; i++)
    {
        combos[LeadMode]->addItem(trs(ECGSymbol::convert((ECGLeadMode)i)));
    }
    combos[LeadMode]->setCurrentIndex(leadmode);

    combos[Ecg1Wave]->clear();
    combos[Ecg2Wave]->clear();
    for (int i = 0; i < ECG_LEAD_NR; i++)
    {
        if ((leadmode == ECG_LEAD_MODE_5 && i > ECG_LEAD_V1)
                || (leadmode == ECG_LEAD_MODE_3 && i > ECG_LEAD_III))
        {
            break;
        }

        combos[Ecg1Wave]->addItem(trs(ECGSymbol::convert((ECGLead)i, ECG_CONVENTION_AAMI)));
        combos[Ecg2Wave]->addItem(trs(ECGSymbol::convert((ECGLead)i, ECG_CONVENTION_AAMI)));
    }

    config->getNumValue("ECG|Ecg1Wave", index);
    if (leadmode == ECG_LEAD_MODE_12)
    {
        combos[Ecg1Wave]->setCurrentIndex(index);
        config->getNumValue("ECG|Ecg2Wave", index);
        combos[Ecg2Wave]->setCurrentIndex(index);
        combos[Ecg1Wave]->label->setText(trs(comboLables[Ecg1Wave]));
        combos[Ecg2Wave]->setVisible(true);
    }
    else if (leadmode == ECG_LEAD_MODE_5)
    {
        int wave2index = 0;
        config->getNumValue("ECG|Ecg2Wave", wave2index);
        if (index > ECG_LEAD_V1)
        {
            index = ECG_LEAD_I;
            if (index == wave2index)
            {
                index = ECG_LEAD_II;
            }
        }

        if (wave2index > ECG_LEAD_V1)
        {
            wave2index = ECG_LEAD_II;
        }

        combos[Ecg2Wave]->setVisible(true);
        combos[Ecg1Wave]->setCurrentIndex(index);
        combos[Ecg2Wave]->setCurrentIndex(wave2index);
        combos[Ecg1Wave]->label->setText(trs(comboLables[Ecg1Wave]));
    }
    else if (leadmode == ECG_LEAD_MODE_3)
    {
        if (index > ECG_LEAD_III)
        {
            index = ECG_LEAD_II;
        }
        combos[Ecg1Wave]->setCurrentIndex(index);
        combos[Ecg1Wave]->label->setText(trs("EcgWave"));
        combos[Ecg2Wave]->setVisible(false);
    }

    combos[Ecg1Gain]->clear();
    combos[Ecg2Gain]->clear();
    for (int i = 0; i < ECG_GAIN_NR; i++)
    {
        combos[Ecg1Gain]->addItem(trs(ECGSymbol::convert((ECGGain)i)));
        combos[Ecg2Gain]->addItem(trs(ECGSymbol::convert((ECGGain)i)));
    }

    config->getNumValue("ECG|Ecg1Gain", index);
    combos[Ecg1Gain]->setCurrentIndex(index);
    config->getNumValue("ECG|Ecg2Gain", index);
    combos[Ecg2Gain]->setCurrentIndex(index);

    if (leadmode > ECG_LEAD_MODE_3)
    {
        combos[Ecg1Gain]->label->setText(trs(comboLables[Ecg1Gain]));
        combos[Ecg2Gain]->setVisible(true);
    }
    else
    {
        combos[Ecg1Gain]->label->setText(trs("EcgGain"));
        combos[Ecg2Gain]->setVisible(false);
    }

    if (combos[SweepSpeed]->count() == 0)
    {
        config->getNumValue("ECG|SweepSpeed", index);
        for (int i = 0; i < ECG_SWEEP_SPEED_NR; i++)
        {
            combos[SweepSpeed]->addItem(ECGSymbol::convert((ECGSweepSpeed) i));
        }
        combos[SweepSpeed]->setCurrentIndex(index);
    }

    if (combos[Filter]->count() == 0)
    {
        config->getNumValue("ECG|Filter", index);
        for (int i = 0; i < ECG_BANDWIDTH_NR; i++)
        {
            combos[Filter]->addItem(ECGSymbol::convert((ECGBandwidth)i));
        }
        combos[Filter]->setCurrentIndex(index);
    }

    if (combos[HeartBeatVol]->count() == 0)
    {
        config->getNumValue("ECG|QRSVolume", index);
        for (int i = 0; i < SoundManager::VOLUME_LEV_NR; i++)
        {
            combos[HeartBeatVol]->addItem(QString::number(i));
        }
        combos[HeartBeatVol]->setCurrentIndex(index);
    }

    if (combos[NotchFilter]->count() == 0)
    {
        config->getNumValue("ECG|NotchFilter", index);
        for (int i = 0; i < ECG_NOTCH_NR; i++)
        {
            combos[NotchFilter]->addItem(ECGSymbol::convert((ECGNotchFilter) i));
        }
        combos[NotchFilter]->setCurrentIndex(index);
    }

    if (combos[SmartLeadOff]->count() == 0)
    {
        config->getNumValue("ECG|SmartLeadOff", index);
        combos[SmartLeadOff]->addItems(QStringList() << trs("OFF") << trs("ON"));
        combos[SmartLeadOff]->setCurrentIndex(index);
    }
}

ConfigEditEcgMenu::ConfigEditEcgMenu()
    : SubMenu(trs("ECGMenu")),
      d_ptr(new ConfigEditEcgMenuPrivate)
{
    setDesc(trs("ECGMenuDesc"));
    startLayout();
}

ConfigEditEcgMenu::~ConfigEditEcgMenu()
{
}
void ConfigEditEcgMenu::layoutExec()
{
    int submenuW = configEditMenuGrp.getSubmenuWidth();
    int submenuH = configEditMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    for (int i = 0; i < ConfigEditEcgMenuPrivate::ComboListMax; i++)
    {
        IComboList *combo = new IComboList(trs(d_ptr->comboLables[i]));
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
void ConfigEditEcgMenu::readyShow()
{
    d_ptr->loadOptions();

    bool preStatusBool = !configManager.getWidgetsPreStatus();

    for (int i = 0; i < ConfigEditEcgMenuPrivate::ComboListMax; i++)
    {
        d_ptr->combos[i]->setEnabled(preStatusBool);
    }
}
// 报警项设置
void ConfigEditEcgMenu::_alarmLbtnSlot()
{
    SubMenu *subMenuPrevious = (configEditMenuGrp.getCurrentEditConfigItem())["ConfigEditEcgMenu"];
    SubMenu *subMenuCurrent = (configEditMenuGrp.getCurrentEditConfigItem())["ConfigEditAlarmLimitMenu"];
    ConfigEditAlarmLimitMenu *alarmLimit = qobject_cast<ConfigEditAlarmLimitMenu *>(subMenuCurrent);
    alarmLimit->setFocusIndex(SUB_PARAM_ECG_PVCS + 1);
    configEditMenuGrp.changePage(subMenuCurrent, subMenuPrevious);
}

void ConfigEditEcgMenu::onComboListConfigChanged(int index)
{
    IComboList *combo = qobject_cast<IComboList *>(sender());
    if (!combo)
    {
        qdebug("Invalid signal sender.");
        return;
    }

    ConfigEditEcgMenuPrivate::ComboListId comboId = (ConfigEditEcgMenuPrivate::ComboListId)
            combo->property("comboId").toInt();
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    \
    switch (comboId)
    {
    case ConfigEditEcgMenuPrivate::AlarmSource:
        config->setNumValue("ECG|AlarmSource", index);
        break;
    case ConfigEditEcgMenuPrivate::Ecg1Wave:
        config->setNumValue("ECG|Ecg1Wave", index);
        break;
    case ConfigEditEcgMenuPrivate::Ecg2Wave:
        config->setNumValue("ECG|Ecg2Wave", index);
        break;
    case ConfigEditEcgMenuPrivate::Ecg1Gain:
        config->setNumValue("ECG|Ecg1Gain", index);
        break;
    case ConfigEditEcgMenuPrivate::Ecg2Gain:
        config->setNumValue("ECG|Ecg2Gain", index);
        break;
    case ConfigEditEcgMenuPrivate::SweepSpeed:
        config->setNumValue("ECG|SweepSpeed", index);
        break;
    case ConfigEditEcgMenuPrivate::Filter:
        config->setNumValue("ECG|Filter", index);
        break;
    case ConfigEditEcgMenuPrivate::HeartBeatVol:
        config->setNumValue("ECG|QRSVolume", index);
        break;
    case ConfigEditEcgMenuPrivate::LeadMode:
        config->setNumValue("ECG|LeadMode", index);
        // need reload after lead mode changed
        d_ptr->loadOptions();
        break;
    case ConfigEditEcgMenuPrivate::NotchFilter:
        config->setNumValue("ECG|NotchFilter", index);
        break;
    case ConfigEditEcgMenuPrivate::SmartLeadOff:
        config->setNumValue("ECG|SmartLeadOff", index);
        break;
    default:
        qdebug("Invalid combo id.");
        break;
    }
}

