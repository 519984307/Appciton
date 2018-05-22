#include "AlarmMaintainSymbol.h"
#include "AlarmMaintainMenu.h"

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
#include "Debug.h"
#include "UserMaintainManager.h"

AlarmMaintainMenu *AlarmMaintainMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmMaintainMenu::AlarmMaintainMenu() : SubMenu(trs("AlarmMaintainMenu")),
                                         _minAlarmVol(NULL),
                                         _pauseAlarmTime(NULL),
                                         _isPauseMaxAlarm15Min(NULL),
                                         _alarmVol(NULL),
                                         _highAlarmIntervals(NULL),
                                         _medAlarmIntervals(NULL),
                                         _lowAlarmIntervals(NULL),
                                         _reminderTone(NULL),
                                         _reminderToneIntervals(NULL),
                                         _ecgLeadOffLev(NULL),
                                         _spo2LeadOffLev(NULL),
                                         _ibpLeadOffLev(NULL),
                                         _isLethalArrhOff(NULL),
                                         _isExtendedArrh(NULL),
                                         _alarmLightOnAlarmReset(NULL),
                                         _alarmDelay(NULL),
                                         _stAlarmDraly(NULL),
                                         _defaults(NULL)
{
    setDesc(trs("AlarmMaintainMenuDesc"));

    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void AlarmMaintainMenu::readyShow()
{
    QString tmpStr;
    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|MinAlarmVol", tmpStr);
    _minAlarmVol->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|AlarmPauseTime", tmpStr);
    _pauseAlarmTime->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|STAlarmDraly", tmpStr);
    _minAlarmVol->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|PauseMaxAlarm15Min", tmpStr);
    _isPauseMaxAlarm15Min->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|AlarmVol", tmpStr);
    _alarmVol->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|ReminderToneIntervals", tmpStr);
    _reminderToneIntervals->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|EcgLeadOffLev", tmpStr);
    _ecgLeadOffLev->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|SPO2LeadOffLev", tmpStr);
    _spo2LeadOffLev->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|IBPLeadOffLev", tmpStr);
    _ibpLeadOffLev->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|LethalArrhOff", tmpStr);
    _isLethalArrhOff->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|ExtendedArrh", tmpStr);
    _isExtendedArrh->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|AlarmLightOnAlarmReset", tmpStr);
    _alarmLightOnAlarmReset->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|AlarmDelay", tmpStr);
    _alarmDelay->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|HighAlarmIntervals", tmpStr);
    _highAlarmIntervals->setValue(tmpStr);
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|MedAlarmIntervals", tmpStr);
    _medAlarmIntervals->setValue(tmpStr);
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|LowAlarmIntervals", tmpStr);
    _lowAlarmIntervals->setValue(tmpStr);
    tmpStr.clear();

    systemConfig.getStrValue("UserMaintain|AlarmMaintainMenu|ReminderTone", tmpStr);
    _minAlarmVol->setCurrentIndex(tmpStr.toInt());
    tmpStr.clear();

}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void AlarmMaintainMenu::layoutExec()
{
    int submenuW = userMaintainManager.getSubmenuWidth();
    int submenuH = userMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;


    //_minAlarmVol
    _minAlarmVol = new IComboList(trs("MinAlarmVol"));
    _minAlarmVol->label->setFixedSize(labelWidth, ITEM_H);
    _minAlarmVol->combolist->setFixedSize(btnWidth, ITEM_H);
    _minAlarmVol->setFont(fontManager.textFont(fontSize));
    for(int i=0; i<10; i++)
    {
        _minAlarmVol->addItem(trs(QString("%1").arg(i)));
    }
    connect(_minAlarmVol->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_setChangeMinAlarmVol(int)));
    mainLayout->addWidget(_minAlarmVol);


    //_pauseAlarmTime
    _pauseAlarmTime = new IComboList(trs("PauseAlarmTime"));
    _pauseAlarmTime->label->setFixedSize(labelWidth, ITEM_H);
    _pauseAlarmTime->combolist->setFixedSize(btnWidth, ITEM_H);
    _pauseAlarmTime->setFont(fontManager.textFont(fontSize));
    for(int i=0; i<ALARM_PAUSE_NR; i++)
    {
        _pauseAlarmTime->addItem(trs(AlarmMaintainSymbol::convert(MaintainAlarmPauseTime(i))));
    }
    connect(_pauseAlarmTime->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_alarmPauseTime(int)));
    mainLayout->addWidget(_pauseAlarmTime);

    //_isPauseMaxAlarm15Min
    _isPauseMaxAlarm15Min = new IComboList(trs("IsPauseMaxAlarm15Min"));
    _isPauseMaxAlarm15Min->label->setFixedSize(labelWidth, ITEM_H);
    _isPauseMaxAlarm15Min->combolist->setFixedSize(btnWidth, ITEM_H);
    _isPauseMaxAlarm15Min->setFont(fontManager.textFont(fontSize));
    _isPauseMaxAlarm15Min->addItem(trs("forbid"));
    _isPauseMaxAlarm15Min->addItem(trs("allow"));
    connect(_isPauseMaxAlarm15Min->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_pauseMaxAlarm15MinSlot(int)));
    mainLayout->addWidget(_isPauseMaxAlarm15Min);


    //_alarmVol
    _alarmVol = new IComboList(trs("AlarmVol"));
    _alarmVol->label->setFixedSize(labelWidth, ITEM_H);
    _alarmVol->combolist->setFixedSize(btnWidth, ITEM_H);
    _alarmVol->setFont(fontManager.textFont(fontSize));
    _alarmVol->addItem(trs("ISO"));
    _alarmVol->addItem(trs("mode 1"));
    _alarmVol->addItem(trs("mode 2"));
    connect(_alarmVol->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_alarmVolSlot(int)));
    mainLayout->addWidget(_alarmVol);

    //_highAlarmIntervals
    _highAlarmIntervals = new ISpinBox(trs("HighAlarmIntervals"));
    _highAlarmIntervals->setRadius(4.0, 4.0);
    _highAlarmIntervals->setRange(3, 15);
    _highAlarmIntervals->setMode(ISPIN_MODE_INT);
    _highAlarmIntervals->setStep(1);
    _highAlarmIntervals->setFont(defaultFont());
    _highAlarmIntervals->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _highAlarmIntervals->enableArrow(false);
    connect(_highAlarmIntervals, SIGNAL(valueChange(QString,int)),
            this, SLOT(_highAlarmIntervalsReleased(QString)));
    _highAlarmIntervals->setFixedHeight(ITEM_H);
    _highAlarmIntervals->setValueWidth(btnWidth);
    mainLayout->addWidget(_highAlarmIntervals);

    //_medAlarmIntervals
    _medAlarmIntervals = new ISpinBox(trs("MedAlarmIntervals"));
    _medAlarmIntervals->setRadius(4.0, 4.0);
    _medAlarmIntervals->setRange(3, 30);
    _medAlarmIntervals->setMode(ISPIN_MODE_INT);
    _medAlarmIntervals->setStep(1);
    _medAlarmIntervals->setFont(defaultFont());
    _medAlarmIntervals->setBorderColor(Qt::black);
    _medAlarmIntervals->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _medAlarmIntervals->enableArrow(false);
    connect(_medAlarmIntervals, SIGNAL(valueChange(QString,int)),
            this, SLOT(_medAlarmIntervalsReleased(QString)));
    _medAlarmIntervals->setFixedHeight(ITEM_H);
    _medAlarmIntervals->setValueWidth(btnWidth);
    mainLayout->addWidget(_medAlarmIntervals);

    //_lowAlarmIntervals
    _lowAlarmIntervals = new ISpinBox(trs("LowAlarmIntervals"));
    _lowAlarmIntervals->setRadius(4.0, 4.0);
    _lowAlarmIntervals->setRange(16, 30);
    _lowAlarmIntervals->setMode(ISPIN_MODE_INT);
    _lowAlarmIntervals->setStep(1);
    _lowAlarmIntervals->setFont(defaultFont());
    _lowAlarmIntervals->setBorderColor(Qt::black);
    _lowAlarmIntervals->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _lowAlarmIntervals->enableArrow(false);
    connect(_lowAlarmIntervals, SIGNAL(valueChange(QString,int)),
            this, SLOT(_lowAlarmIntervalsReleased(QString)));
    _lowAlarmIntervals->setFixedHeight(ITEM_H);
    _lowAlarmIntervals->setValueWidth(btnWidth);
    mainLayout->addWidget(_lowAlarmIntervals);

    //_reminderTone
    _reminderTone = new IComboList(trs("ReminderTone"));
    _reminderTone->label->setFixedSize(labelWidth, ITEM_H);
    _reminderTone->combolist->setFixedSize(btnWidth, ITEM_H);
    _reminderTone->setFont(fontManager.textFont(fontSize));
    _reminderTone->addItem(trs("close"));
    _reminderTone->addItem(trs("open"));
    _reminderTone->addItem(trs("openAgain"));
    connect(_reminderTone->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_reminderToneSlot(int)));
    mainLayout->addWidget(_reminderTone);

    //_reminderToneIntervals
    _reminderToneIntervals = new IComboList(trs("ReminderToneIntervals"));
    _reminderToneIntervals->label->setFixedSize(labelWidth, ITEM_H);
    _reminderToneIntervals->combolist->setFixedSize(btnWidth, ITEM_H);
    _reminderToneIntervals->setFont(fontManager.textFont(fontSize));
    _reminderToneIntervals->addItem(trs("1min"));
    _reminderToneIntervals->addItem(trs("2min"));
    _reminderToneIntervals->addItem(trs("3min"));
    connect(_reminderToneIntervals->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_reminderToneIntervalsSlot(int)));
    mainLayout->addWidget(_reminderToneIntervals);

    //_ecgLeadOffLev
    _ecgLeadOffLev = new IComboList(trs("EcgLeadOffLev"));
    _ecgLeadOffLev->label->setFixedSize(labelWidth, ITEM_H);
    _ecgLeadOffLev->combolist->setFixedSize(btnWidth, ITEM_H);
    _ecgLeadOffLev->setFont(fontManager.textFont(fontSize));
    _ecgLeadOffLev->addItem(trs("High"));
    _ecgLeadOffLev->addItem(trs("Med"));
    _ecgLeadOffLev->addItem(trs("Low"));
    connect(_ecgLeadOffLev->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_ecgLeadOffLevSlot(int)));
    mainLayout->addWidget(_ecgLeadOffLev);

    //_spo2LeadOffLev
    _spo2LeadOffLev = new IComboList(trs("spo2LeadOffLev"));
    _spo2LeadOffLev->label->setFixedSize(labelWidth, ITEM_H);
    _spo2LeadOffLev->combolist->setFixedSize(btnWidth, ITEM_H);
    _spo2LeadOffLev->setFont(fontManager.textFont(fontSize));
    _spo2LeadOffLev->addItem(trs("High"));
    _spo2LeadOffLev->addItem(trs("Med"));
    _spo2LeadOffLev->addItem(trs("Low"));
    connect(_spo2LeadOffLev->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_spo2LeadOffLevSlot(int)));
    mainLayout->addWidget(_spo2LeadOffLev);

    //_ibpLeadOffLev
    _ibpLeadOffLev = new IComboList(trs("IbpLeadOffLev"));
    _ibpLeadOffLev->label->setFixedSize(labelWidth, ITEM_H);
    _ibpLeadOffLev->combolist->setFixedSize(btnWidth, ITEM_H);
    _ibpLeadOffLev->setFont(fontManager.textFont(fontSize));
    _ibpLeadOffLev->addItem(trs("High"));
    _ibpLeadOffLev->addItem(trs("Med"));
    _ibpLeadOffLev->addItem(trs("Low"));
    connect(_ibpLeadOffLev->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_ibpLeadOffLevSlot(int)));
    mainLayout->addWidget(_ibpLeadOffLev);

    //_isLethalArrhOff
    _isLethalArrhOff = new IComboList(trs("LethalArrhOff"));
    _isLethalArrhOff->label->setFixedSize(labelWidth, ITEM_H);
    _isLethalArrhOff->combolist->setFixedSize(btnWidth, ITEM_H);
    _isLethalArrhOff->setFont(fontManager.textFont(fontSize));
    _isLethalArrhOff->addItem(trs("forbid"));
    _isLethalArrhOff->addItem(trs("allow"));
    connect(_isLethalArrhOff->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_ibpLeadOffLevSlot(int)));
    mainLayout->addWidget(_isLethalArrhOff);

    //_isExtendedArrh
    _isExtendedArrh = new IComboList(trs("ExtendedArrh"));
    _isExtendedArrh->label->setFixedSize(labelWidth, ITEM_H);
    _isExtendedArrh->combolist->setFixedSize(btnWidth, ITEM_H);
    _isExtendedArrh->setFont(fontManager.textFont(fontSize));
    _isExtendedArrh->addItem(trs("forbid"));
    _isExtendedArrh->addItem(trs("allow"));
    connect(_isLethalArrhOff->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_extendedArrhSlot(int)));
    mainLayout->addWidget(_isExtendedArrh);

    //_alarmLightOnAlarmReset
    _alarmLightOnAlarmReset = new IComboList(trs("_alarmLightOnAlarmReset"));
    _alarmLightOnAlarmReset->label->setFixedSize(labelWidth, ITEM_H);
    _alarmLightOnAlarmReset->combolist->setFixedSize(btnWidth, ITEM_H);
    _alarmLightOnAlarmReset->setFont(fontManager.textFont(fontSize));
    _alarmLightOnAlarmReset->addItem(trs("close"));
    _alarmLightOnAlarmReset->addItem(trs("open"));
    connect(_isExtendedArrh->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_alarmLightOnAlarmResetSlot(int)));
    mainLayout->addWidget(_alarmLightOnAlarmReset);

    //_alarmDelay
    _alarmDelay = new IComboList(trs("AlarmDelay"));
    _alarmDelay->label->setFixedSize(labelWidth, ITEM_H);
    _alarmDelay->combolist->setFixedSize(btnWidth, ITEM_H);
    _alarmDelay->setFont(fontManager.textFont(fontSize));
    _alarmDelay->addItem(trs("close"));
    for(int i=1; i<=8; i++)
    {
        _alarmDelay->addItem(trs(QString("%1").arg(i)));
    }
    connect(_alarmDelay->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_alarmDelaySlot(int)));
    mainLayout->addWidget(_alarmDelay);

    //_stAlarmDraly
    _stAlarmDraly = new IComboList(trs("PauseAlarmTime"));
    _stAlarmDraly->label->setFixedSize(labelWidth, ITEM_H);
    _stAlarmDraly->combolist->setFixedSize(btnWidth, ITEM_H);
    _stAlarmDraly->setFont(fontManager.textFont(fontSize));
    for(int i=0; i<ALARM_ST_DELAY_NR; i++)
    {
        _stAlarmDraly->addItem(trs(AlarmMaintainSymbol::convert(AlarmSTDelay(i))));
    }
    connect(_stAlarmDraly->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_stAlarmDralySlot(int)));
    mainLayout->addWidget(_stAlarmDraly);

    //Defaults
    _defaults = new LabelButton(trs("Defaults"));
    _defaults->setFont(fontManager.textFont(fontSize));
    _defaults->label->setFixedSize(labelWidth, ITEM_H);
    _defaults->button->setFixedSize(btnWidth, ITEM_H);
    _defaults->button->setText(trs("RecoverDefaults"));
    connect(_defaults->button, SIGNAL(released(int)), this, SLOT(_defaultsSlot()));
    mainLayout->addWidget(_defaults);
}

/**************************************************************************************************
 * MonitorName槽函数。
 *************************************************************************************************/
void AlarmMaintainMenu::_defaultsSlot()
{

}

/**************************************************************************************************
 * Change MinAlarmVol。
 *************************************************************************************************/
void AlarmMaintainMenu::_setChangeMinAlarmVol(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|MinAlarmVol", index);
}

/**************************************************************************************************
 * PauseAlarmTime。
 *************************************************************************************************/
void AlarmMaintainMenu::_alarmPauseTime(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|AlarmPauseTime", index);
}

/**************************************************************************************************
 *STAlarmDraly
 *************************************************************************************************/
void AlarmMaintainMenu::_stAlarmDralySlot(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|STAlarmDraly", index);
}

/**************************************************************************************************
 * PauseMaxAlarm15Min。
 *************************************************************************************************/
void AlarmMaintainMenu::_pauseMaxAlarm15MinSlot(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|PauseMaxAlarm15Min", index);
}

/**************************************************************************************************
 * AlarmVol。
 *************************************************************************************************/
void AlarmMaintainMenu::_alarmVolSlot(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|AlarmVol", index);
}

/**************************************************************************************************
 * ReminderTone。
 *************************************************************************************************/
void AlarmMaintainMenu::_reminderToneSlot(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|ReminderTone", index);
}

/**************************************************************************************************
 * ReminderToneIntervals。
 *************************************************************************************************/
void AlarmMaintainMenu::_reminderToneIntervalsSlot(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|ReminderToneIntervals", index);
}

/**************************************************************************************************
 * EcgLeadOffLev。
 *************************************************************************************************/
void AlarmMaintainMenu::_ecgLeadOffLevSlot(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|EcgLeadOffLev", index);
}

/**************************************************************************************************
 * SpO2LeadOffLev。
 *************************************************************************************************/
void AlarmMaintainMenu::_spo2LeadOffLevSlot(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|SPO2LeadOffLev", index);
}

/**************************************************************************************************
 * IBPLeadOffLev。
 *************************************************************************************************/
void AlarmMaintainMenu::_ibpLeadOffLevSlot(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|IBPLeadOffLev", index);
}

/**************************************************************************************************
 * LethalArrhOff。
 *************************************************************************************************/
void AlarmMaintainMenu::_lethalArrhOffSlot(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|LethalArrhOff", index);
}

/**************************************************************************************************
 * ExtendedArrh。
 *************************************************************************************************/
void AlarmMaintainMenu::_extendedArrhSlot(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|ExtendedArrh", index);
}

/**************************************************************************************************
 * AlarmLightOnAlarmReset。
 *************************************************************************************************/
void AlarmMaintainMenu::_alarmLightOnAlarmResetSlot(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|AlarmLightOnAlarmReset", index);
}

/**************************************************************************************************
 * AlarmDelay。
 *************************************************************************************************/
void AlarmMaintainMenu::_alarmDelaySlot(int index)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|AlarmDelay", index);
}

/**************************************************************************************************
 * 高级报警间隔。
 *************************************************************************************************/
void AlarmMaintainMenu::_highAlarmIntervalsReleased(QString strValue)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|HighAlarmIntervals", strValue.toInt());
}

/**************************************************************************************************
 * 中级报警间隔。
 *************************************************************************************************/
void AlarmMaintainMenu::_medAlarmIntervalsReleased(QString strValue)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|MedAlarmIntervals", strValue.toInt());
}

/**************************************************************************************************
 * 低级报警间隔。
 *************************************************************************************************/
void AlarmMaintainMenu::_lowAlarmIntervalsReleased(QString strValue)
{
    systemConfig.setNumValue("UserMaintain|AlarmMaintainMenu|LowAlarmIntervals", strValue.toInt());
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmMaintainMenu::~AlarmMaintainMenu()
{

}
