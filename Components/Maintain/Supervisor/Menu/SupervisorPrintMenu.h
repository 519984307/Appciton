#pragma once
#include "SubMenu.h"

class IComboList;

//Print配置
class SupervisorPrintMenu : public SubMenu
{
    Q_OBJECT

public:
    static SupervisorPrintMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new SupervisorPrintMenu();
        }

        return *_selfObj;
    }
    static SupervisorPrintMenu *_selfObj;

    ~SupervisorPrintMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _ECGRhythmAutoPrintSlot(int index);
    void _ECGForeAnalysisAutoPrintSlot(int index);
    void _shockDeliveryAutoPrintSlot(int index);
    void _checkPatientAlarmAutoPrintSlot(int index);
    void _pacerStartAutoPrintSlot(int index);
    void _phyAlarmAutoPrintSlot(int index);
    void _codemarkerAutoPrintSlot(int index);
    void _NIBPAutoPrintSlot(int index);
//    void _diagECGAutoPrintSlot(int index);
//    void _autoPrintInAEDSlot(int index);
//    void _autoPrint30JtestResultSlot(int index);

private:
    SupervisorPrintMenu();

    IComboList *_ECGRhythmAutoPrint;              // 自动打印ECG节率
    IComboList *_checkPatientAlarmAutoPrint;      // 自动打印检查病人报警
    IComboList *_phyAlarmAutoPrint;               // 自动打印生理报警快照
    IComboList *_codemarkerAutoPrint;             // 自动打印coder marker快照
    IComboList *_NIBPAutoPrint;                    // Automatic print nibp snapshot
//    IComboList *_diagECGAutoPrint;                // Automatic print diag ecg snapshot
//    IComboList *_autoPrintInAED;                  // AED模式自动打印
//    IComboList *_30jSelftest;                     // 30J自测
};
#define supervisorPrintMenu (SupervisorPrintMenu::construction())
