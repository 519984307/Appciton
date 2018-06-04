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
#include "COParam.h"

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
                                           _touchScreenCalibration(NULL),
                                           _start(NULL),
                                           _measureSta(CO_INST_START),
                                           _isAnaesthesiaCal(true),
                                           _isIBPPressureCal(true),
                                           _isCO2Cal(true),
                                           _isNIBPPressureTest(true),
                                           _isNIBPLeakageDet(true),
                                           _isECGCal(true),
                                           _istouchScreenCal(true)
{
    setDesc(trs("ModuleMaintainMenuDesc"));

    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void ModuleMaintainMenu::readyShow()
{

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

    _start = new LabelButton("");
    _start->label->setFixedSize(labelWidth, ITEM_H);
    _start->setFont(defaultFont());
    _start->button->setFixedSize(btnWidth, ITEM_H);
    _start->button->setText(trs("COStart"));
    connect(_start, SIGNAL(realReleased()), this, SLOT(_startReleased()));
    mainLayout->addWidget(_start, 0, Qt::AlignRight);
}

/**************************************************************************************************
 *麻醉模块校准槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isAnaesthesiaSlot()
{
    if(_isAnaesthesiaCal)
    {
        _isAnaesthesiaCal = false;
        _anaesthesiaModuleCalibration->button->setText(trs("StopAnaesthesia"));
    }
    else if(!_isAnaesthesiaCal)
    {
        _isAnaesthesiaCal = true;
        _anaesthesiaModuleCalibration->button->setText(trs("Anaesthesia"));
    }
}

/**************************************************************************************************
 *NIBP压力检验槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isNIBPPressureTestSlot()
{
    if(_isNIBPPressureTest)
    {
        _isNIBPPressureTest = false;
        _nibpPressureTest->button->setText(trs("StopNIBPPressureTest"));
    }
    else if(!_isNIBPPressureTest)
    {
        _isNIBPPressureTest = true;
        _nibpPressureTest->button->setText(trs("NIBPPressureTest"));
    }
}
/**************************************************************************************************
 *NIBP漏气检测槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isNIBPLeakageDetectionSlot()
{
    if(_isNIBPLeakageDet)
    {
        _isNIBPLeakageDet = false;
        _nibpLeakageDetection->button->setText(trs("StopNIBPLeakageDetection"));
    }
    else if(!_isNIBPLeakageDet)
    {
        _isNIBPLeakageDet = true;
        _nibpLeakageDetection->button->setText(trs("NIBPLeakageDetection"));
    }
}
/**************************************************************************************************
 *ECG模块校准槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isECGModuleCalibrationSlot()
{
    if(_isECGCal)
    {
        _isECGCal = false;
        _ecgModuleCalibration->button->setText(trs("StopECGModuleCalibration"));
    }
    else if(!_isECGCal)
    {
        _isECGCal = true;
        _ecgModuleCalibration->button->setText(trs("ECGModuleCalibration"));
    }
}
/**************************************************************************************************
 *IBP压力校准槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isIBPPressureCalibrationSlot()
{
    if(_isIBPPressureCal)
    {
        _isIBPPressureCal = false;
        _ibpPressureCalibration->button->setText(trs("StopIBPPressureCalibration"));
    }
    else if(!_isIBPPressureCal)
    {
        _isIBPPressureCal = true;
        _ibpPressureCalibration->button->setText(trs("IBPPressureCalibration"));
    }
}

/**************************************************************************************************
 * CO2模块维护槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isCO2ModuleMaintenanceSlot()
{
    if(_isCO2Cal)
    {
        _isCO2Cal = false;
        _co2ModuleMaintenance->button->setText(trs("StopCO2ModuleMaintenance"));
    }
    else if(!_isCO2Cal)
    {
        _isCO2Cal = true;
        _co2ModuleMaintenance->button->setText(trs("CO2ModuleMaintenance"));
    }
}

/**************************************************************************************************
 *touch screen校准槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_isTouchScreenCalibrationSlot()
{
    if(_istouchScreenCal)
    {
        _istouchScreenCal = false;
        _touchScreenCalibration->button->setText(trs("StopTouchScreenCalibration"));
    }
    else if(!_istouchScreenCal)
    {
        _istouchScreenCal = true;
        _touchScreenCalibration->button->setText(trs("TouchScreenCalibration"));
    }
}

/**************************************************************************************************
 * 开始测量槽函数。
 *************************************************************************************************/
void ModuleMaintainMenu::_startReleased(void)
{
    coParam.measureCtrl(_measureSta);
    if (_measureSta == CO_INST_START)
    {
        _start->button->setText(trs("Cancel"));
        _measureSta = CO_INST_END;
    }
    else if (_measureSta == CO_INST_END)
    {
        _start->button->setText(trs("COStart"));
        _measureSta = CO_INST_START;
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ModuleMaintainMenu::~ModuleMaintainMenu()
{

}
