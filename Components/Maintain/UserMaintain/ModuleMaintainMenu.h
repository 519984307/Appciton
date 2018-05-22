#pragma once
#include "SubMenu.h"

class LabelButton;
class IComboList;
class ModuleMaintainMenu : public SubMenu
{
    Q_OBJECT

public:
    static ModuleMaintainMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ModuleMaintainMenu();
        }

        return *_selfObj;
    }
    static ModuleMaintainMenu *_selfObj;

    ~ModuleMaintainMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _isAnaesthesiaSlot(void);
    void _isNIBPPressureTestSlot(void);
    void _isIBPPressureCalibrationSlot(void);
    void _isCO2ModuleMaintenanceSlot(void);
    void _isNIBPLeakageDetectionSlot(void);
    void _isECGModuleCalibrationSlot(void);
    void _isTouchScreenCalibrationSlot(void);

private:
    ModuleMaintainMenu();

    LabelButton  *_anaesthesiaModuleCalibration;      //麻醉模块校准
    LabelButton  *_ibpPressureCalibration;            //IBP压力校准
    LabelButton  *_co2ModuleMaintenance;              //CO2模块维护
    LabelButton  *_nibpPressureTest;                  //NIBP压力检验
    LabelButton  *_nibpLeakageDetection;              //NIBP漏气检测
    LabelButton  *_ecgModuleCalibration;              //ECG模块校准
    LabelButton  *_touchScreenCalibration;            //触摸屏模块校准

};
#define moduleMaintainMenu (ModuleMaintainMenu::construction())
