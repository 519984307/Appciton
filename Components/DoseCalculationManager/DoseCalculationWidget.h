#pragma once
#include "PopupWidget.h"
//#include "PatientDefine.h"
#include "IComboList.h"
#include "QSignalMapper"
#include <QString>
#include "DoseCalculationDefine.h"

class LabelButton;
class IButton;
class DoseCalculationWidget : public PopupWidget
{
    Q_OBJECT

public:
    static DoseCalculationWidget &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new DoseCalculationWidget();
        }
        return *_selfObj;
    }
    static DoseCalculationWidget *_selfObj;
    ~DoseCalculationWidget();

    void layoutExec(void);

    void initDrugParam(void);
    void updateDrugParam(void);
    void updataParamDefault(void);

    void weightHandle(float);
    void totalHandle(float);
    void volumeHandle(float);
    void concentrationHandle(float);
    void dose1Handle(float);
    void dose2Handle(float);
    void dose3Handle(float);
    void dose4Handle(float);
    void infusionRateHandle(float);
    void dripRateHandle(float);
    void dropSizeHandle(float);
    void durationHandle(float);

protected:
    void showEvent(QShowEvent *e);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void _btnParamReleased(int);
    void _updataParamDefault(int);
    void _titrateTableReleased(void);

private:
    DoseCalculationWidget();

private:
    static const int _itemH = 30;       // 子项高度

    QSignalMapper *_signalMapper;

    IComboList *_drugName;              // 药物名称
    IComboList *_patientType;           // 病人类型
    LabelButton *_calcParam[CALCULATION_PARAM_NR];
    QLabel *_calcParamUnit[CALCULATION_PARAM_NR];
    IButton *_titrationTable;           // 滴定表

};
#define doseCalculationWidget (DoseCalculationWidget::construction())
#define deleteDoseCalculationWidget() (delete DoseCalculationWidget::_selfObj)
