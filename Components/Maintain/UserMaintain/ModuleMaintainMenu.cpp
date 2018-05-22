#include "ModuleMaintainMenu.h"
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

ModuleMaintainMenu *ModuleMaintainMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ModuleMaintainMenu::ModuleMaintainMenu() : SubMenu(trs("ModuleMaintainMenu")),
                                           _anaesthesiaModuleCalibration(NULL),
                                           _ibpPressureCalibration(NULL),
                                           _co2ModuleMaintenance(NULL),
                                           _nibpPressureTest(NULL),
                                           _nibpLeakageDetection(NULL),
                                           _ecgModuleCalibration(NULL),
                                           _touchScreenCalibration(NULL)
{
    setDesc(trs("ModuleMaintainMenuDesc"));

    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void ModuleMaintainMenu::readyShow()
{

//    QString tmpStr;
//    systemConfig.getStrValue("UserMaintainManager|ModuleMaintain|AnaesthesiaModule", tmpStr);
//    _anaesthesiaModuleCalibration->setValue(tmpStr);

//    tmpStr.clear();
//    systemConfig.getStrValue("General|UserMaintainPassword", tmpStr);
//    _modifyPassword->setValue(tmpStr);

//    tmpStr.clear();
//    systemConfig.getStrValue("General|Department", tmpStr);
//    _department->setValue(tmpStr);

//    tmpStr.clear();
//    systemConfig.getStrValue("General|BedNumber", tmpStr);
//    _bedNumber->setValue(tmpStr);

//    int value = 0;
//    systemConfig.getNumValue("General|ChangeBedNumberRight", value);
//    _changeBedNumberRight->setCurrentIndex(value);


//    systemConfig.getNumValue("General|Language", value);
//    _language->setCurrentIndex(value);

}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void ModuleMaintainMenu::layoutExec()
{
    int submenuW = userMaintainManager.getSubmenuWidth();
    int submenuH = userMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    //anaesthesia Module Calibration
    _anaesthesiaModuleCalibration = new LabelButton(trs("AnaesthesiaModuleCalibration"));
    _anaesthesiaModuleCalibration->setFont(fontManager.textFont(fontSize));
    _anaesthesiaModuleCalibration->label->setFixedSize(labelWidth, ITEM_H);
    _anaesthesiaModuleCalibration->button->setFixedSize(btnWidth, ITEM_H);
    _anaesthesiaModuleCalibration->button->setText("Anaesthesia");
    connect(_anaesthesiaModuleCalibration->button, SIGNAL(released(int)), this, SLOT(_isAnaesthesiaSlot()));
    mainLayout->addWidget(_anaesthesiaModuleCalibration);

    //IBPPressureCalibration
    _ibpPressureCalibration = new LabelButton(trs("IBPPressureCalibration"));
    _ibpPressureCalibration->label->setFixedSize(labelWidth, ITEM_H);
    _ibpPressureCalibration->button->setFixedSize(btnWidth, ITEM_H);
    _ibpPressureCalibration->setFont(fontManager.textFont(fontSize));
    _ibpPressureCalibration->button->setText("IBPPressureCalibration");
    connect(_ibpPressureCalibration->button, SIGNAL(released(int)), this, SLOT(_isIBPPressureCalibrationSlot()));
    mainLayout->addWidget(_ibpPressureCalibration);

    //CO2 ModuleMaintenance
    _co2ModuleMaintenance = new LabelButton(trs("CO2ModuleMaintenance"));
    _co2ModuleMaintenance->label->setFixedSize(labelWidth, ITEM_H);
    _co2ModuleMaintenance->button->setFixedSize(btnWidth, ITEM_H);
    _co2ModuleMaintenance->setFont(fontManager.textFont(fontSize));
    _co2ModuleMaintenance->button->setText("CO2ModuleMaintenance");
    connect(_co2ModuleMaintenance->button, SIGNAL(released(int)), this, SLOT(_isCO2ModuleMaintenanceSlot()));
    mainLayout->addWidget(_co2ModuleMaintenance);

    //NIBP Pressure Test
    _nibpPressureTest = new LabelButton("NIBPPressureTest");
    _nibpPressureTest->label->setFixedSize(labelWidth, ITEM_H);
    _nibpPressureTest->button->setFixedSize(btnWidth, ITEM_H);
    _nibpPressureTest->setFont(fontManager.textFont(fontSize));
    _nibpPressureTest->button->setText("NIBPPressureTest");
    connect(_nibpPressureTest->button, SIGNAL(released(int)), this, SLOT(_isNIBPPressureTestSlot()));
    mainLayout->addWidget(_nibpPressureTest);

    //NIBP Leakage Detection
    _nibpLeakageDetection = new LabelButton(trs("NIBPLeakageDetection"));
    _nibpLeakageDetection->label->setFixedSize(labelWidth, ITEM_H);
    _nibpLeakageDetection->button->setFixedSize(btnWidth, ITEM_H);
    _nibpLeakageDetection->setFont(fontManager.textFont(fontSize));
    _nibpLeakageDetection->button->setText("NIBPLeakageDetection");
    connect(_nibpLeakageDetection->button, SIGNAL(released(int)), this, SLOT(_isNIBPLeakageDetectionSlot()));
    mainLayout->addWidget(_nibpLeakageDetection);

    //ECG Module Calibration
    _ecgModuleCalibration = new LabelButton(trs("ECGModuleCalibration"));
    _ecgModuleCalibration->label->setFixedSize(labelWidth, ITEM_H);
    _ecgModuleCalibration->button->setFixedSize(btnWidth, ITEM_H);
    _ecgModuleCalibration->setFont(fontManager.textFont(fontSize));
    _ecgModuleCalibration->button->setText("ECGModuleCalibration");
    connect(_ecgModuleCalibration->button, SIGNAL(released(int)), this, SLOT(_isECGModuleCalibrationSlot()));
    mainLayout->addWidget(_ecgModuleCalibration);

    //Touch Screen Module Calibration
    _touchScreenCalibration = new LabelButton(trs("TouchScreenCalibration"));
    _touchScreenCalibration->label->setFixedSize(labelWidth, ITEM_H);
    _touchScreenCalibration->button->setFixedSize(btnWidth, ITEM_H);
    _touchScreenCalibration->setFont(fontManager.textFont(fontSize));
    _touchScreenCalibration->button->setText("TouchScreenCalibration");
    connect(_touchScreenCalibration->button, SIGNAL(released(int)), this, SLOT(_isTouchScreenCalibrationSlot()));
    mainLayout->addWidget(_touchScreenCalibration);
}

/**************************************************************************************************
 *麻醉模块校准槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isAnaesthesiaSlot()
{
    if(_anaesthesiaModuleCalibration->button->text()=="Anaesthesia")
    {
        _anaesthesiaModuleCalibration->button->setText("StopAnaesthesia");
    }
    else if(_anaesthesiaModuleCalibration->button->text()=="StopAnaesthesia")
    {
        _anaesthesiaModuleCalibration->button->setText("Anaesthesia");
    }
}

/**************************************************************************************************
 *NIBP压力检验槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isNIBPPressureTestSlot()
{
    if(_nibpPressureTest->button->text()=="NIBPPressureTest")
    {
        _nibpPressureTest->button->setText("StopNIBPPressureTest");
    }
    else if(_nibpPressureTest->button->text()=="StopNIBPPressureTest")
    {
        _nibpPressureTest->button->setText("NIBPPressureTest");
    }
}
/**************************************************************************************************
 *NIBP漏气检测槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isNIBPLeakageDetectionSlot()
{
    if(_nibpLeakageDetection->button->text()=="NIBPLeakageDetection")
    {
        _nibpLeakageDetection->button->setText("StopNIBPLeakageDetection");
    }
    else if(_nibpLeakageDetection->button->text()=="StopNIBPLeakageDetection")
    {
        _nibpLeakageDetection->button->setText("NIBPLeakageDetection");
    }
}
/**************************************************************************************************
 *ECG模块校准槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isECGModuleCalibrationSlot()
{
    if(_ecgModuleCalibration->button->text()=="ECGModuleCalibration")
    {
        _ecgModuleCalibration->button->setText("StopECGModuleCalibration");
    }
    else if(_ecgModuleCalibration->button->text()=="StopECGModuleCalibration")
    {
        _ecgModuleCalibration->button->setText("ECGModuleCalibration");
    }
}
/**************************************************************************************************
 *IBP压力校准槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isIBPPressureCalibrationSlot()
{
    if(_ibpPressureCalibration->button->text()=="IBPPressureCalibration")
    {
        _ibpPressureCalibration->button->setText("StopIBPPressureCalibration");
    }
    else if(_ibpPressureCalibration->button->text()=="StopIBPPressureCalibration")
    {
        _ibpPressureCalibration->button->setText("IBPPressureCalibration");
    }
}

/**************************************************************************************************
 * CO2模块维护槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isCO2ModuleMaintenanceSlot()
{
    if(_co2ModuleMaintenance->button->text()=="CO2ModuleMaintenance")
    {
        _co2ModuleMaintenance->button->setText("StopCO2ModuleMaintenance");
    }
    else if(_co2ModuleMaintenance->button->text()=="StopCO2ModuleMaintenance")
    {
        _co2ModuleMaintenance->button->setText("CO2ModuleMaintenance");
    }
}

/**************************************************************************************************
 *touch screen校准槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isTouchScreenCalibrationSlot()
{
    if(_touchScreenCalibration->button->text()=="TouchScreenCalibration")
    {
        _touchScreenCalibration->button->setText("StopTouchScreenCalibration");
    }
    else if(_touchScreenCalibration->button->text()=="StopTouchScreenCalibration")
    {
        _touchScreenCalibration->button->setText("TouchScreenCalibration");
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ModuleMaintainMenu::~ModuleMaintainMenu()
{

}
