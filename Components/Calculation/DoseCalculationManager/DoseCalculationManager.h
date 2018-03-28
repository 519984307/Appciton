#pragma once

#include <QObject>
#include "DoseCalculationDefine.h"

class DoseCalculationManager : public QObject
{
    Q_OBJECT

 public:
     static DoseCalculationManager &construction(void)
     {
         if (_selfObj == NULL)
         {
             _selfObj = new DoseCalculationManager();
         }
         return *_selfObj;
     }
     ~DoseCalculationManager();

public:
     // 设置-获取病人类型
     void setType(PatientType);
     PatientType getType(void);

     // 设置-获取药物名称
     void setDrugName(DrugName);
     DrugName getDrugName(void);

     // 设置-获取药物单位
     void setDrugUnit(DrugUnit);
     DrugUnit getDrugUnit(void);

     // 设置-获取病人参数
     void setDrugParam(int, float, QString);
     DrugInfo getDrugParam(int);

     // 获取病人信息。
     const DoseCalculationInfo &getdoseCalculationInfo(void);

     // 药物计算入口函数
     void doseCalcAction(DoseCalculationParam, float);

     // 单位换算
     void unitConversion(DrugInfo &);

     // 统一单位
     void unifyUnit(QString, QString, int &, int &);

     // 指定单位转换
     void specifyUnit(DrugInfo &, QString);

private:     
     static DoseCalculationManager *_selfObj;
     DoseCalculationManager();

     DoseCalculationInfo _doseCalculationInfo;
};

#define doseCalculationManager (DoseCalculationManager::construction())
#define deleteDoseCalculationManager() (delete &doseCalculationManager)
