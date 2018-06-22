#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "IBPMenu.h"
#include "IBPParam.h"
#include "IComboList.h"
#include "LabelButton.h"
#include "ISpinBox.h"
#include "LanguageManager.h"
#include "SetWidget.h"
#include "MenuManager.h"
#include "NumberInput.h"
#include "IMessageBox.h"
#include "LoadConfigMenu.h"
#include "ConfigManager.h"

IBPMenu *IBPMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
IBPMenu::IBPMenu() : SubMenu(trs("IBPMenu"))
{
    setDesc(trs("IBPMenuDesc"));
    startLayout();
    connect(&configManager, SIGNAL(configUpdated()), this, SLOT(_onConfigUpdated()));
}

/**************************************************************************************************
 * 显示校零校准提示信息。
 *************************************************************************************************/
void IBPMenu::displayZeroRev(QString info)
{
    _zeroRev->setText(info);
}

/**************************************************************************************************
 * 通道一标名下拉菜单选中改变。
 *************************************************************************************************/
void IBPMenu::comboListChangeIBP1(IBPPressureName name)
{
    _entitle1->combolist->setCurrentIndex(name);
}

/**************************************************************************************************
 * 通道二标名下拉菜单选中改变。
 *************************************************************************************************/
void IBPMenu::comboListChangeIBP2(IBPPressureName name)
{
    _entitle2->combolist->setCurrentIndex(name);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
IBPMenu::~IBPMenu()
{

}

void IBPMenu::readyShow()
{
    int index = 0;
    currentConfig.getNumValue("IBP|SweepSpeed", index);
    if(_speed->combolist)
    {
        _speed->combolist->setCurrentIndex(index);
    }
}

void IBPMenu::readyhide() { }

void IBPMenu::_onConfigUpdated()
{
    readyShow();
}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void IBPMenu::layoutExec(void)
{
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    _entitle1 = new IComboList(trs("ChannelPressureEntitle1"));
    _entitle1->setFont(defaultFont());
    for (int i = 0; i < IBP_PRESSURE_NR; i ++)
    {
        _entitle1->addItem(IBPSymbol::convert((IBPPressureName)i));
    }
    connect(_entitle1, SIGNAL(currentIndexChanged(int)), this, SLOT(_entitleSlot1(int)));
    _entitle1->label->setFixedSize(labelWidth, ITEM_H);
    _entitle1->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_entitle1);

    _entitle2 = new IComboList(trs("ChannelPressureEntitle2"));
    _entitle2->setFont(defaultFont());
    for (int i = 0; i < IBP_PRESSURE_NR; i ++)
    {
        _entitle2->addItem(IBPSymbol::convert((IBPPressureName)i));
    }
    connect(_entitle2, SIGNAL(currentIndexChanged(int)), this, SLOT(_entitleSlot2(int)));
    _entitle2->label->setFixedSize(labelWidth, ITEM_H);
    _entitle2->combolist->setFixedSize(btnWidth, ITEM_H);
    _entitle2->combolist->setCurrentIndex(1);
    mainLayout->addWidget(_entitle2);

    _speed = new IComboList(trs("IBPSweepSpeed"));
    _speed->setFont(defaultFont());
    for (int i = 0; i < IBP_SWEEP_SPEED_NR; i ++)
    {
        _speed->addItem(IBPSymbol::convert((IBPSweepSpeed)i));
    }
    connect(_speed, SIGNAL(currentIndexChanged(int)), this, SLOT(_speedSlot(int)));
    _speed->label->setFixedSize(labelWidth, ITEM_H);
    _speed->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_speed);

    _filter = new IComboList(trs("FilterMode"));
    _filter->setFont(defaultFont());
    for (int i = 0; i < IBP_FILTER_MODE_NR; i ++)
    {
        _filter->addItem(IBPSymbol::convert((IBPFilterMode)i));
    }
    connect(_filter, SIGNAL(currentIndexChanged(int)), this, SLOT(_filterSlot(int)));
    _filter->label->setFixedSize(labelWidth, ITEM_H);
    _filter->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_filter);

    _sensitivity = new IComboList(trs("Sensitivity"));
    _sensitivity->setFont(defaultFont());
    for (int i = 0; i < IBP_SENSITIVITY_NR; i ++)
    {
        _sensitivity->addItem(trs(IBPSymbol::convert((IBPSensitivity)i)));
    }
    connect(_sensitivity, SIGNAL(currentIndexChanged(int)), this, SLOT(_sensitivitySlot(int)));
    _sensitivity->label->setFixedSize(labelWidth, ITEM_H);
    _sensitivity->combolist->setFixedSize(btnWidth, ITEM_H);
    _sensitivity->combolist->setCurrentIndex(1);
    mainLayout->addWidget(_sensitivity);

    _zeroCalib = new LabelButton(trs("IBPZeroCalib"));
    _zeroCalib->setFont(defaultFont());
    _zeroCalib->setValue(trs("IBPZeroStart"));
    connect(_zeroCalib->button, SIGNAL(realReleased()), this, SLOT(_zeroCalibReleased()));
    _zeroCalib->label->setFixedSize(labelWidth, ITEM_H);
    _zeroCalib->button->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_zeroCalib);

    _calibration = new LabelButton(trs("ServiceCalibrate"));
    _calibration->setFont(defaultFont());
    _calibration->button->setText("80");
    connect(_calibration->button, SIGNAL(realReleased()), this, SLOT(_calibrationReleased()));
    _calibration->label->setFixedSize(labelWidth, ITEM_H);
    _calibration->button->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_calibration);

    _zeroRev = new QLabel();
    _zeroRev->setFont(defaultFont());
    _zeroRev->setFixedSize(btnWidth, ITEM_H);
    _zeroRev->setStyleSheet("background:transparent;border-width:0;border-style:outset");
    _zeroRev->setAlignment(Qt::AlignRight);
    mainLayout->addWidget(_zeroRev);
}

void IBPMenu::_speedSlot(int index)
{
    ibpParam.setSweepSpeed((IBPSweepSpeed)index);
    currentConfig.setNumValue("IBP|SweepSpeed", index);
}

/**************************************************************************************************
 * 通道１标名改变。
 *************************************************************************************************/
void IBPMenu::_entitleSlot1(int index)
{
    ibpParam.setEntitle((IBPPressureName)index, IBP_INPUT_1);
}

/**************************************************************************************************
 * 通道２标名改变。
 *************************************************************************************************/
void IBPMenu::_entitleSlot2(int index)
{
    ibpParam.setEntitle((IBPPressureName)index, IBP_INPUT_2);
}

/**************************************************************************************************
 * 滤波改变。
 *************************************************************************************************/
void IBPMenu::_filterSlot(int index)
{
    ibpParam.setFilter((IBPFilterMode)index);
}

/**************************************************************************************************
 * 灵敏度设置。
 *************************************************************************************************/
void IBPMenu::_sensitivitySlot(int index)
{
    ibpParam.setSensitivity((IBPSensitivity)index);
}

/**************************************************************************************************
 * 执行校零。
 *************************************************************************************************/
void IBPMenu::_zeroCalibReleased()
{
    ibpParam.zeroCalibration(IBP_INPUT_1);
}

/**************************************************************************************************
 * 执行校准。
 *************************************************************************************************/
void IBPMenu::_calibrationReleased()
{
    NumberInput numberPad;
    numberPad.setTitleBarText(trs("ServiceCalibrate"));
    numberPad.setMaxInputLength(7);
    numberPad.setInitString(_calibration->button->text());
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
            }
            else
            {
                IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "80-300", QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
        }
    }
}
