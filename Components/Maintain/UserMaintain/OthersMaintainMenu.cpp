#include "OthersMaintainMenu.h"
#include "UserMaintainManager.h"
#include <QVBoxLayout>
#include <QRegExp>
#include "FontManager.h"
#include "LanguageManager.h"
#include "LabelButton.h"
#include "IButton.h"
#include "IComboList.h"
#include "SupervisorMenuManager.h"
#include "SupervisorConfigManager.h"
#include "PatientDefine.h"
#include "KeyBoardPanel.h"


OthersMaintainMenu *OthersMaintainMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
OthersMaintainMenu::OthersMaintainMenu() : SubMenu(trs("OthersMaintainMenu")),
                                           _waveLine(NULL),
                                           _ecgStandard(NULL),
                                           _frequencyNotch(NULL),
                                           _paraSwitchPrem(NULL),
                                           _confImpactItemSettings(NULL),
                                           _nurseCallSetting(NULL),
                                           _signalType(NULL),
                                           _triggerMode(NULL),
                                           _alarmLevel(NULL),
                                           _alarmType(NULL)
{
    setDesc(trs("OthersMaintainMenuDesc"));

    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void OthersMaintainMenu::readyShow()
{
    QString tmpStr;
    systemConfig.getStrValue("Others|WaveLine", tmpStr);
    if(tmpStr.toInt()>=_waveLine->count())
    {
        _waveLine->setCurrentIndex(0);
    }
    else
    {
        _waveLine->setCurrentIndex(tmpStr.toInt());
    }
    tmpStr.clear();

    systemConfig.getStrValue("Others|ECGStandard", tmpStr);
    if(tmpStr.toInt()>=_ecgStandard->count())
    {
        _ecgStandard->setCurrentIndex(0);
    }
    else
    {
        _ecgStandard->setCurrentIndex(tmpStr.toInt());
    }
    tmpStr.clear();

    systemConfig.getStrValue("Others|FrequencyNotch", tmpStr);
    if(tmpStr.toInt()>=_frequencyNotch->count())
    {
        _frequencyNotch->setCurrentIndex(0);
    }
    else
    {
        _frequencyNotch->setCurrentIndex(tmpStr.toInt());
    }
    tmpStr.clear();

    systemConfig.getStrValue("Others|ParaSwitchPrem", tmpStr);
    if(tmpStr.toInt()>=_paraSwitchPrem->count())
    {
        _paraSwitchPrem->setCurrentIndex(0);
    }
    else
    {
        _paraSwitchPrem->setCurrentIndex(tmpStr.toInt());
    }
    tmpStr.clear();

    systemConfig.getStrValue("Others|ConfImpactItemSettings", tmpStr);
    if(tmpStr.toInt()>=_confImpactItemSettings->count())
    {
        _confImpactItemSettings->setCurrentIndex(0);
    }
    else
    {
        _confImpactItemSettings->setCurrentIndex(tmpStr.toInt());
    }
    tmpStr.clear();

    systemConfig.getStrValue("Others|NurseCallSetting", tmpStr);
    if(tmpStr.toInt()>=_nurseCallSetting->count())
    {
        _nurseCallSetting->setCurrentIndex(0);
    }
    else
    {
        _nurseCallSetting->setCurrentIndex(tmpStr.toInt());
        _signalType->setEnabled(!tmpStr.toInt());
        _triggerMode->setEnabled(!tmpStr.toInt());
        _alarmLevel->setEnabled(!tmpStr.toInt());
        _alarmType->setEnabled(!tmpStr.toInt());
    }
    tmpStr.clear();

    systemConfig.getStrValue("Others|SignalType", tmpStr);
    if(tmpStr.toInt()>=_signalType->count())
    {
        _signalType->setCurrentIndex(0);
    }
    else
    {
        _signalType->setCurrentIndex(tmpStr.toInt());
    }
    tmpStr.clear();

    systemConfig.getStrValue("Others|TriggerMode", tmpStr);
    if(tmpStr.toInt()>=_triggerMode->count())
    {
        _triggerMode->setCurrentIndex(0);
    }
    else
    {
        _triggerMode->setCurrentIndex(tmpStr.toInt());
    }
    tmpStr.clear();

    systemConfig.getStrValue("Others|AlarmLevel", tmpStr);
    if(tmpStr.toInt()>=_alarmLevel->count())
    {
        _alarmLevel->setCurrentIndex(0);
    }
    else
    {
        _alarmLevel->setCurrentIndex(tmpStr.toInt());
    }
    tmpStr.clear();

    systemConfig.getStrValue("Others|AlarmType", tmpStr);
    if(tmpStr.toInt()>=_alarmType->count())
    {
        _alarmType->setCurrentIndex(0);
    }
    else
    {
        _alarmType->setCurrentIndex(tmpStr.toInt());
    }
    tmpStr.clear();
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void OthersMaintainMenu::layoutExec()
{
    int submenuW = userMaintainManager.getSubmenuWidth();
    int submenuH = userMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    //_waveLine setup
    _waveLine = new IComboList(trs("WaveLine"));
    _waveLine->label->setFixedSize(labelWidth, ITEM_H);
    _waveLine->combolist->setFixedSize(btnWidth, ITEM_H);
    _waveLine->setFont(fontManager.textFont(fontSize));
    _waveLine->addItem(trs("fat"));
    _waveLine->addItem(trs("med"));
    _waveLine->addItem(trs("thin"));
    _waveLine->combolist->setProperty("ComboID",qVariantFromValue(0));
    connect(_waveLine->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_editCombolistSlot(int)));
    mainLayout->addWidget(_waveLine);


    //_ecgStandard
    _ecgStandard = new IComboList(trs("ECGStandard"));
    _ecgStandard->setFont(fontManager.textFont(fontSize));
    _ecgStandard->label->setFixedSize(labelWidth, ITEM_H);
    _ecgStandard->combolist->setFixedSize(btnWidth, ITEM_H);
    _ecgStandard->addItem(trs("AHA"));
    _ecgStandard->addItem(trs("IEC"));
    _ecgStandard->combolist->setProperty("ComboID",qVariantFromValue(1));
    connect(_ecgStandard->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_editCombolistSlot(int)));
    mainLayout->addWidget(_ecgStandard);

    //_frequencyNotch
    _frequencyNotch = new IComboList(trs("FrequencyNotch"));
    _frequencyNotch->label->setFixedSize(labelWidth, ITEM_H);
    _frequencyNotch->combolist->setFixedSize(btnWidth, ITEM_H);
    _frequencyNotch->addItem(trs("50 Hz"));
    _frequencyNotch->addItem(trs("60 Hz"));
    _frequencyNotch->setFont(fontManager.textFont(fontSize));
    _frequencyNotch->combolist->setProperty("ComboID",qVariantFromValue(2));
    connect(_frequencyNotch->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_editCombolistSlot(int)));
    mainLayout->addWidget(_frequencyNotch);

    //paraSwitchPrem
    _paraSwitchPrem = new IComboList(trs("paraSwitchPrem"));
    _paraSwitchPrem->label->setFixedSize(labelWidth, ITEM_H);
    _paraSwitchPrem->combolist->setFixedSize(btnWidth, ITEM_H);
    _paraSwitchPrem->addItem(trs("open"));
    _paraSwitchPrem->addItem(trs("protected"));
    _paraSwitchPrem->setFont(fontManager.textFont(fontSize));
    _paraSwitchPrem->combolist->setProperty("ComboID",qVariantFromValue(3));
    connect(_paraSwitchPrem->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_editCombolistSlot(int)));
    mainLayout->addWidget(_paraSwitchPrem);

    //_confImpactItemSettings
    _confImpactItemSettings = new IComboList(trs("ConfImpactItemSettings"));
    _confImpactItemSettings->label->setFixedSize(labelWidth, ITEM_H);
    _confImpactItemSettings->combolist->setFixedSize(btnWidth, ITEM_H);
    _confImpactItemSettings->addItem(trs("close"));
    _confImpactItemSettings->addItem(trs("ParaSwitch"));
    _confImpactItemSettings->setFont(fontManager.textFont(fontSize));
    _confImpactItemSettings->combolist->setProperty("ComboID",qVariantFromValue(4));
    connect(_confImpactItemSettings->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_editCombolistSlot(int)));
    mainLayout->addWidget(_confImpactItemSettings);

    //_nurseCallSetting
    _nurseCallSetting = new IComboList(trs("NurseCallSetting"));
    _nurseCallSetting->label->setFixedSize(labelWidth, ITEM_H);
    _nurseCallSetting->combolist->setFixedSize(btnWidth, ITEM_H);
    _nurseCallSetting->addItem(trs("setting"));
    _nurseCallSetting->addItem(trs("stopSetting"));
    _nurseCallSetting->setFont(fontManager.textFont(fontSize));
    _nurseCallSetting->combolist->setProperty("ComboID",qVariantFromValue(5));
    connect(_nurseCallSetting->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_editCombolistSlot(int)));
    mainLayout->addWidget(_nurseCallSetting);

    //_signalType
    _signalType = new IComboList(trs("SignalType"));
    _signalType->label->setFixedSize(labelWidth, ITEM_H);
    _signalType->combolist->setFixedSize(btnWidth, ITEM_H);
    _signalType->setFont(fontManager.textFont(fontSize));
    _signalType->addItem(trs("Continuity"));
    _signalType->addItem(trs("Pluse"));
    _signalType->combolist->setProperty("ComboID",qVariantFromValue(6));
    connect(_signalType->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_editCombolistSlot(int)));
    mainLayout->addWidget(_signalType);


    //_triggerMode
    _triggerMode = new IComboList(trs("TriggerMode"));
    _triggerMode->label->setFixedSize(labelWidth, ITEM_H);
    _triggerMode->combolist->setFixedSize(btnWidth, ITEM_H);
    _triggerMode->setFont(fontManager.textFont(fontSize));
    _triggerMode->addItem(trs("Closed"));
    _triggerMode->addItem(trs("Opened"));
    _triggerMode->combolist->setProperty("ComboID",qVariantFromValue(7));
    connect(_triggerMode->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_editCombolistSlot(int)));
    mainLayout->addWidget(_triggerMode);

    //_alarmLevel
    _alarmLevel = new IComboList(trs("AlarmLevel"));
    _alarmLevel->label->setFixedSize(labelWidth, ITEM_H);
    _alarmLevel->combolist->setFixedSize(btnWidth, ITEM_H);
    _alarmLevel->setFont(fontManager.textFont(fontSize));
    _alarmLevel->addItem(trs("High"));
    _alarmLevel->addItem(trs("Med"));
    _alarmLevel->addItem(trs("Low"));
    _alarmLevel->combolist->setProperty("ComboID",qVariantFromValue(8));
    connect(_alarmLevel->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_editCombolistSlot(int)));
    mainLayout->addWidget(_alarmLevel);

    //_alarmType
    _alarmType = new IComboList(trs("AlarmType"));
    _alarmType->label->setFixedSize(labelWidth, ITEM_H);
    _alarmType->combolist->setFixedSize(btnWidth, ITEM_H);
    _alarmType->setFont(fontManager.textFont(fontSize));
    _alarmType->addItem(trs("Technology"));
    _alarmType->addItem(trs("Physiology"));
    _alarmType->combolist->setProperty("ComboID",qVariantFromValue(9));
    connect(_alarmType->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_editCombolistSlot(int)));
    mainLayout->addWidget(_alarmType);
}

/**************************************************************************************************
 * _editCombolistSlot。
 *************************************************************************************************/
void OthersMaintainMenu::_editCombolistSlot(int index)
{
    ComboList *sen = qobject_cast<ComboList*>(sender());
    if(!sen)
    {
        return;
    }
    QString string;
    int comboID = sen->property("ComboID").toInt();

    switch(comboID)
    {
        case 0:
            string = "WaveLine";
        break;
        case 1:
            string = "ECGStandard";
        break;
        case 2:
            string = "FrequencyNotch";
        break;
        case 3:
            string = "ParaSwitchPrem";
        break;
        case 4:
            string = "ConfImpactItemSettings";
        break;
        case 5:
            string = "NurseCallSetting";
            _signalType->setEnabled(!index);
            _triggerMode->setEnabled(!index);
            _alarmLevel->setEnabled(!index);
            _alarmType->setEnabled(!index);
        break;
        case 6:
            string = "SignalType";
        break;
        case 7:
            string = "TriggerMode";
        break;
        case 8:
            string = "AlarmLevel";
        break;
        case 9:
            string = "AlarmType";
        break;
    }
    systemConfig.setNumValue(QString("Others|%1").arg(string), index);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OthersMaintainMenu::~OthersMaintainMenu()
{

}
