/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/12
 **/



#pragma once
#include "SubMenu.h"

class IComboList;

// Print配置
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

private:
    SupervisorPrintMenu();

    IComboList *_ECGRhythmAutoPrint;              // 自动打印ECG节率
    IComboList *_checkPatientAlarmAutoPrint;      // 自动打印检查病人报警
    IComboList *_phyAlarmAutoPrint;               // 自动打印生理报警快照
    IComboList *_codemarkerAutoPrint;             // 自动打印coder marker快照
    IComboList *_NIBPAutoPrint;                    // Automatic print nibp snapshot
};
#define supervisorPrintMenu (SupervisorPrintMenu::construction())
