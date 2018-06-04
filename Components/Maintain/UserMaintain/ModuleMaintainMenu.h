#pragma once
#include "SubMenu.h"
#include "CODefine.h"

class LabelButton;
class IComboList;
class ModuleMaintainMenu : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction 初始化方法
     * @return
     */
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
    /**
     * @brief layoutExec 布局方法
     */
    virtual void layoutExec(void);
    /**
     * @brief readyShow 数据装载方法
     */
    virtual void readyShow(void);
private slots:
    /**
     * @brief _isAnaesthesiaSlot 麻醉模块校准动作方法
     */
    void _isAnaesthesiaSlot(void);
    /**
     * @brief _isNIBPPressureTestSlot  NIBP压力检验动作方法
     */
    void _isNIBPPressureTestSlot(void);
    /**
     * @brief _isIBPPressureCalibrationSlot  IBP压力校准动作方法
     */
    void _isIBPPressureCalibrationSlot(void);
    /**
     * @brief _isCO2ModuleMaintenanceSlot  CO2模块维护动作方法
     */
    void _isCO2ModuleMaintenanceSlot(void);
    /**
     * @brief _isNIBPLeakageDetectionSlot  NIBP漏气检测动作方法
     */
    void _isNIBPLeakageDetectionSlot(void);
    /**
     * @brief _isECGModuleCalibrationSlot  ECG模块校准动作方法
     */
    void _isECGModuleCalibrationSlot(void);
    /**
     * @brief _isTouchScreenCalibrationSlot  触摸屏模块校准动作方法
     */
    void _isTouchScreenCalibrationSlot(void);
    /**
     * @brief _startReleased  开始测量动作方法
     */
    void _startReleased(void);
private:
    ModuleMaintainMenu();
    LabelButton  *_anaesthesiaModuleCalibration;      //麻醉模块校准
    LabelButton  *_ibpPressureCalibration;            //IBP压力校准
    LabelButton  *_co2ModuleMaintenance;              //CO2模块维护
    LabelButton  *_nibpPressureTest;                  //NIBP压力检验
    LabelButton  *_nibpLeakageDetection;              //NIBP漏气检测
    LabelButton  *_ecgModuleCalibration;              //ECG模块校准
    LabelButton  *_touchScreenCalibration;            //触摸屏模块校准
    LabelButton  *_start;                             //CO开始测量
    COInstCtl    _measureSta;                         // 测量状态
    bool         _isAnaesthesiaCal;                   //麻醉模块是否校准
    bool         _isIBPPressureCal;                   //IBP压力是否校准
    bool         _isCO2Cal;                           //CO2模块是否校准
    bool         _isNIBPPressureTest;                 //NIBP压力是否检验
    bool         _isNIBPLeakageDet;                   //NIBP漏气是否检测
    bool         _isECGCal;                           //ECG模块是否校准
    bool         _istouchScreenCal;                   //触摸屏模块是否校准
};
#define moduleMaintainMenu (ModuleMaintainMenu::construction())
