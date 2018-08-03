//////////////////////////////////////////////
#include "ConfigEditIBPMenu.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "IComboList.h"
#include "LabelButton.h"
#include "ISpinBox.h"
#include "LanguageManager.h"
#include "SetWidget.h"
#include "PublicMenuManager.h"
#include "NumberInput.h"
#include "IMessageBox.h"
#include "NumberInput.h"
#include "COParam.h"
#include "ConfigManager.h"
#include "Config.h"
#include "ConfigEditMenuGrp.h"
#include "IBPParam.h"
#include "ConfigEditAlarmLimitMenu.h"

ConfigIBPMenu *ConfigIBPMenu::_selfObj = NULL;

ConfigIBPMenu::ConfigIBPMenu() : SubMenu(trs("ConfigIBPMenu"))
{
    setDesc(trs("ConfigIBPMenuDesc"));
    startLayout();
}

ConfigIBPMenu::~ConfigIBPMenu()
{

}
/**************************************************************************************************
 * 显示校零校准提示信息。
 *************************************************************************************************/
void ConfigIBPMenu::displayZeroRev(QString info)
{
    _zeroRev->setText(info);
}

/**************************************************************************************************
 * 通道一标名下拉菜单选中改变。
 *************************************************************************************************/
void ConfigIBPMenu::comboListChangeIBP1(IBPPressureName name)
{
    _entitle1->combolist->setCurrentIndex(name);
}

/**************************************************************************************************
 * 通道二标名下拉菜单选中改变。
 *************************************************************************************************/
void ConfigIBPMenu::comboListChangeIBP2(IBPPressureName name)
{
    _entitle2->combolist->setCurrentIndex(name);
}


void ConfigIBPMenu::readyShow()
{
    bool preStatusBool = !configManager.getWidgetsPreStatus();

    _entitle1->setEnabled(preStatusBool);
    _entitle2->setEnabled(preStatusBool);
    _speed->setEnabled(preStatusBool);
    _filter->setEnabled(preStatusBool);
    _sensitivity->setEnabled(preStatusBool);
    _zeroCalib->setEnabled(preStatusBool);
    _calibration->setEnabled(preStatusBool);
    _zeroRev->setEnabled(preStatusBool);
}

void ConfigIBPMenu::readyhide() { }

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void ConfigIBPMenu::layoutExec(void)
{
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    int index = 0;

    config->getNumValue("IBP|ChannelPressureEntitle1", index);

    _entitle1 = new IComboList(trs("ChannelPressureEntitle1"));
    _entitle1->setFont(defaultFont());
    for (int i = 0; i < IBP_PRESSURE_NR; i ++)
    {
        _entitle1->addItem(IBPSymbol::convert((IBPPressureName)i));
    }
    connect(_entitle1, SIGNAL(currentIndexChanged(int)), this, SLOT(_entitleSlot1(int)));
    _entitle1->label->setFixedSize(labelWidth, ITEM_H);
    _entitle1->combolist->setFixedSize(btnWidth, ITEM_H);
    _entitle1->combolist->setCurrentIndex(index);
    mainLayout->addWidget(_entitle1);

    index = 0;
    config->getNumValue("IBP|ChannelPressureEntitle2", index);
    _entitle2 = new IComboList(trs("ChannelPressureEntitle2"));
    _entitle2->setFont(defaultFont());
    for (int i = 0; i < IBP_PRESSURE_NR; i ++)
    {
        _entitle2->addItem(IBPSymbol::convert((IBPPressureName)i));
    }
    connect(_entitle2, SIGNAL(currentIndexChanged(int)), this, SLOT(_entitleSlot2(int)));
    _entitle2->label->setFixedSize(labelWidth, ITEM_H);
    _entitle2->combolist->setFixedSize(btnWidth, ITEM_H);
    _entitle2->combolist->setCurrentIndex(index);
    mainLayout->addWidget(_entitle2);

    index = 0;
    config->getNumValue("IBP|IBPSweepSpeed", index);
    _speed = new IComboList(trs("IBPSweepSpeed"));
    _speed->setFont(defaultFont());
    for (int i = 0; i < IBP_SWEEP_SPEED_NR; i ++)
    {
        _speed->addItem(IBPSymbol::convert((IBPSweepSpeed)i));
    }
    connect(_speed, SIGNAL(currentIndexChanged(int)), this, SLOT(_speedSlot(int)));
    _speed->label->setFixedSize(labelWidth, ITEM_H);
    _speed->combolist->setFixedSize(btnWidth, ITEM_H);
    _speed->combolist->setCurrentIndex(index);
    mainLayout->addWidget(_speed);

    index = 0;
    config->getNumValue("IBP|FilterMode",index);
    _filter = new IComboList(trs("FilterMode"));
    _filter->setFont(defaultFont());
    for (int i = 0; i < IBP_FILTER_MODE_NR; i ++)
    {
        _filter->addItem(IBPSymbol::convert((IBPFilterMode)i));
    }
    connect(_filter, SIGNAL(currentIndexChanged(int)), this, SLOT(_filterSlot(int)));
    _filter->label->setFixedSize(labelWidth, ITEM_H);
    _filter->combolist->setFixedSize(btnWidth, ITEM_H);
    _filter->combolist->setCurrentIndex(index);
    mainLayout->addWidget(_filter);

    index = 0;
    config->getNumValue("IBP|Sensitivity", index);
    _sensitivity = new IComboList(trs("Sensitivity"));
    _sensitivity->setFont(defaultFont());
    for (int i = 0; i < IBP_SENSITIVITY_NR; i ++)
    {
        _sensitivity->addItem(trs(IBPSymbol::convert((IBPSensitivity)i)));
    }
    connect(_sensitivity, SIGNAL(currentIndexChanged(int)), this, SLOT(_sensitivitySlot(int)));
    _sensitivity->label->setFixedSize(labelWidth, ITEM_H);
    _sensitivity->combolist->setFixedSize(btnWidth, ITEM_H);
    _sensitivity->combolist->setCurrentIndex(index);
    mainLayout->addWidget(_sensitivity);

    index = 0;
    _zeroCalib = new LabelButton(trs("IBPZeroCalib"));
    _zeroCalib->setFont(defaultFont());
    _zeroCalib->setValue(trs("IBPZeroStart"));
    connect(_zeroCalib->button, SIGNAL(realReleased()), this, SLOT(_zeroCalibReleased()));
    _zeroCalib->label->setFixedSize(labelWidth, ITEM_H);
    _zeroCalib->button->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_zeroCalib);

    index = 0;
    config->getNumValue("IBP|Calibration", index);
    _calibration = new LabelButton(trs("ServiceCalibrate"));
    _calibration->setFont(defaultFont());
    _calibration->button->setText(QString("%1").arg(index));
    connect(_calibration->button, SIGNAL(realReleased()), this, SLOT(_calibrationReleased()));
    _calibration->label->setFixedSize(labelWidth, ITEM_H);
    _calibration->button->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_calibration);

    _zeroRev = new QLabel();
    _zeroRev->setFont(defaultFont());
    _zeroRev->setFixedSize(btnWidth, ITEM_H);
    _zeroRev->setStyleSheet("background:transparent;border-width:0;border-style:outset");
    _zeroRev->setAlignment(Qt::AlignRight);
//    mainLayout->addWidget(_zeroRev);

    _alarmLbtn = new LabelButton("");
    _alarmLbtn->setFont(defaultFont());
    _alarmLbtn->label->setFixedSize(labelWidth, ITEM_H);
    _alarmLbtn->button->setFixedSize(btnWidth, ITEM_H);
    _alarmLbtn->button->setText(trs("AlarmLimitSetUp"));
    connect(_alarmLbtn->button, SIGNAL(realReleased()), this, SLOT(_alarmLbtnSlot()));
    mainLayout->addWidget(_alarmLbtn);
}
//报警项设置
void ConfigIBPMenu::_alarmLbtnSlot()
{
    SubMenu *subMenuPrevious = (configEditMenuGrp.getCurrentEditConfigItem())["ConfigIBPMenu"] ;
    SubMenu *subMenuCurrent = (configEditMenuGrp.getCurrentEditConfigItem())["ConfigEditAlarmLimitMenu"] ;
    ConfigEditAlarmLimitMenu* alarmLimit = qobject_cast<ConfigEditAlarmLimitMenu*>(subMenuCurrent);
    alarmLimit->setFocusIndex(SUB_PARAM_ART_SYS+1);
    configEditMenuGrp.changePage(subMenuCurrent, subMenuPrevious);
}

void ConfigIBPMenu::_speedSlot(int index)
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    config->setNumValue("IBP|IBPSweepSpeed", index);
    ibpParam.setSweepSpeed((IBPSweepSpeed)index);
}

/**************************************************************************************************
 * 通道１标名改变。
 *************************************************************************************************/
void ConfigIBPMenu::_entitleSlot1(int index)
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    config->setNumValue("IBP|ChannelPressureEntitle1", index);
    ibpParam.setEntitle((IBPPressureName)index, IBP_INPUT_1);
}

/**************************************************************************************************
 * 通道２标名改变。
 *************************************************************************************************/
void ConfigIBPMenu::_entitleSlot2(int index)
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    config->setNumValue("IBP|ChannelPressureEntitle2", index);
    ibpParam.setEntitle((IBPPressureName)index, IBP_INPUT_2);
}

/**************************************************************************************************
 * 滤波改变。
 *************************************************************************************************/
void ConfigIBPMenu::_filterSlot(int index)
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    config->setNumValue("IBP|FilterMode", index);
    ibpParam.setFilter((IBPFilterMode)index);
}

/**************************************************************************************************
 * 灵敏度设置。
 *************************************************************************************************/
void ConfigIBPMenu::_sensitivitySlot(int index)
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    config->setNumValue("IBP|Sensitivity", index);
    ibpParam.setSensitivity((IBPSensitivity)index);
}

/**************************************************************************************************
 * 执行校零。
 *************************************************************************************************/
void ConfigIBPMenu::_zeroCalibReleased()
{
    ibpParam.zeroCalibration(IBP_INPUT_1);
}

/**************************************************************************************************
 * 执行校准。
 *************************************************************************************************/
void ConfigIBPMenu::_calibrationReleased()
{
    NumberInput numberPad;
    numberPad.setTitleBarText(trs("ServiceCalibrate"));
    numberPad.setMaxInputLength(7);
    numberPad.setInitString(_calibration->button->text());
    Config *config = configEditMenuGrp.getCurrentEditConfig();

    if (numberPad.exec())
    {
        QString text = numberPad.getStrValue();
        bool ok = false;
        unsigned short value = text.toShort(&ok);
        if (ok)
        {
            if (value >= 80 && value <= 300)
            {
                _calibration->button->setText(text);
                ibpParam.setCalibration(IBP_INPUT_1, value);
                config->setNumValue("IBP|Calibration", value);
            }
            else
            {
                IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "80-300", QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
        }
    }
}
