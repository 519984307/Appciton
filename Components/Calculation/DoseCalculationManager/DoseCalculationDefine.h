#pragma once
#include <string.h>
#include "BaseDefine.h"
#include "PatientDefine.h"
#include <QString>

enum TitrateTableParam
{
    TITRATE_TABLE_WEIGHT,
    TITRATE_TABLE_TOTAL,
    TITRATE_TABLE_VOLUME,
    TITRATE_TABLE_DOSE,
    TITRATE_TABLE_INFUSIONRATE,
    TITRATE_TABLE_DRIPRATE,
    TITRATE_TABLE_NR
};

enum DoseCalculationParam
{
    CALCULATION_PARAM_WEIGHT,
    CALCULATION_PARAM_TOTAL,
    CALCULATION_PARAM_VOLUME,
    CALCULATION_PARAM_CONCENTRATION,
    CALCULATION_PARAM_DOSE1,
    CALCULATION_PARAM_DOSE2,
    CALCULATION_PARAM_DOSE3,
    CALCULATION_PARAM_DOSE4,
    CALCULATION_PARAM_INFUSIONRATE,
    CALCULATION_PARAM_DRIPRATE,
    CALCULATION_PARAM_DROPSIZE,
    CALCULATION_PARAM_DURATION,
    CALCULATION_PARAM_NR
};

enum DrugUnit
{
    DRUG_UNIT_GRAM,
    DRUG_UNIT_UNIT,
    DRUG_UNIT_MEQ,
    DRUG_UNIT_NR
};

enum DrugName
{
    DRUG_NAME_DRUGA,                // 药物A
    DRUG_NAME_DRUGB,                // 药物B
    DRUG_NAME_DRUGC,                // 药物C
    DRUG_NAME_DRUGD,                // 药物D
    DRUG_NAME_DRUGE,                // 药物E
    DRUG_NAME_AMINOPHYLINE,         // 氨茶碱
    DRUG_NAME_DOBUTAMINE,           // 多巴酚丁胺
    DRUG_NAME_DOPAMINE,             // 多巴胺
    DRUG_NAME_EPINEPHRINE,          // 肾上腺素
    DRUG_NAME_HEPARIN,              // 肝素
    DRUG_NAME_ISUPREL,              // 异丙肾上腺素
    DRUG_NAME_LIDOCAINE,            // 利多卡因
    DRUG_NAME_NIPRIDE,              // 硝普钠
    DRUG_NAME_NITROGLYCERIN,        // 硝化甘油
    DRUG_NAME_PITOCIN,              // 催产素
    DRUG_NAME_NR
};

struct DrugInfo {
    float value;
    QString unit;
};

struct DoseCalculationInfo
{
    DoseCalculationInfo()
    {
         name = DRUG_NAME_DRUGA;
         type = PATIENT_TYPE_ADULT;
         unit = DRUG_UNIT_GRAM;
    }
    DrugName name;
    PatientType type;
    DrugUnit unit;
    DrugInfo drugParam[CALCULATION_PARAM_NR];
 };

class DoseCalculationSymbol
{
public:
    static const char *convert(PatientType index)
    {
        static const char *symbol[PATIENT_TYPE_NR] =
        {
            "Adult",
            "PED",
            "NEO",
            "NULL"
        };
        return symbol[index];
    }

    static const char *convert(DrugName index)
    {
        static const char *symbol[DRUG_NAME_NR] =
        {
            "DrugA",
            "DrugB",
            "DrugC",
            "DrugD",
            "DrugE",
            "Aminophyline",
            "Dobutamine",
            "Dopamine",
            "Epinephrine",
            "Heparin",
            "Isuprel",
            "Lidocaine",
            "Nipride",
            "Nitroglycerin",
            "Pitocin"
        };
        return symbol[index];
    }

    static const char *convert(DoseCalculationParam index)
    {
        static const char *symbol[CALCULATION_PARAM_NR] =
        {
            "PatientWeight" ,
            "DrugTotal",
            "LiquidVolume",
            "DrugConcentration",
            "Dosemin",
            "Dosehr",
            "DoseKgmin",
            "DoseKghr",
            "InfusionRate",
            "DripRate",
            "DropSize",
            "Duration"
        };
        return symbol[index];
    }

    static const char *convert(PatientType type, DrugName name, DoseCalculationParam param)
    {
        static const char *symbol[PATIENT_TYPE_NR][DRUG_NAME_NR][CALCULATION_PARAM_NR] =
        {
            {
                {"", "400.00", "250.00", "1600.00", "2500.00", "150.00", "", "", "93.75", "31.25", "20.00", "2.67"}
                ,{"", "400.00", "250.00", "1600.00", "2500.00", "150.00", "", "", "93.75", "31.25", "20.00", "2.67"}
                ,{"", "400.00", "250.00", "1600.00", "2500.00", "150.00", "", "", "93.75", "31.25", "20.00", "2.67"}
                ,{"", "400.00", "250.00", "1.60", "2.50", "150.00", "", "", "93.75", "31.25", "20.00", "2.67"}
                ,{"", "400.00", "250.00", "1.60", "2.50", "150.00", "", "", "93.75", "31.25", "20.00", "2.67"}
                ,{"", "500.00", "500.00", "1000.00", "1000.00", "60.00", "", "", "60.00", "20.00", "20.00", "8.33"}
                ,{"", "500.00", "250.00", "2000.00", "100.00", "6000.00", "", "", "3.00", "1.00", "20.00", "83.33"}
                ,{"", "400.00", "250.00", "1600.00", "100.00", "6000.00", "", "", "3.75", "1.25", "20.00", "66.67"}
                ,{"", "4000.00", "250.00", "16.00", "10.00", "600.00", "", "", "37.50", "12.50", "20.00", "6.67"}
                ,{"", "12.50", "250.00", "50.00", "50.00", "3000.00", "", "", "60.00", "20.00", "20.00", "4.17"}
                ,{"", "1000.00", "250.00", "4.00", "2.50", "150.00", "", "", "37.50", "12.50", "20.00", "6.67"}
                ,{"", "250.00", "250.00", "1000.00", "1000.00", "60.00", "", "", "60.00", "20.00", "20.00", "4.17"}
                ,{"", "50.00", "250.00", "200.00", "100.00", "6000.00", "", "", "30.00", "10.00", "20.00", "8.33"}
                ,{"", "50.00", "250.00", "200.00", "50.00", "3000.00", "", "", "15.00", "5.00", "20.00", "16.67"}
                ,{"", "50.00", "500.00", "0.10", "0.10", "6.00", "", "", "60.00", "20.00", "20.00", "8.33"}
            },

            {
                {"", "160.00", "100.00", "1600.00", "800.00", "48.00", "", "", "30.00", "20.00", "40.00", "3.33"}
                ,{"", "160.00", "100.00", "1600.00", "800.00", "48.00", "", "", "30.00", "20.00", "40.00", "3.33"}
                ,{"", "160.00", "100.00", "1600.00", "800.00", "48.00", "", "", "30.00", "20.00", "40.00", "3.33"}
                ,{"", "160.00", "100.00", "1.60", "0.80", "48.00", "", "", "30.00", "20.00", "40.00", "3.33"}
                ,{"", "160.00", "100.00", "1.60", "0.80", "48.00", "", "", "30.00", "20.00", "40.00", "3.33"}
                ,{"", "125.00", "125.00", "1000.00", "250.00", "15.00", "", "", "15.00", "10.00", "40.00", "8.33"}
                ,{"", "125.00", "62.50", "2000.00", "25.00", "1500.00", "", "", "0.75", "0.50", "40.00", "83.33"}
                ,{"", "100.00", "62.50", "1600.00", "25.00", "1500.00", "", "", "0.94", "0.63", "40.00", "66.67"}
                ,{"", "1000.00", "62.50", "16.00", "2.50", "150.00", "", "", "9.38", "6.25", "40.00", "6.67"}
                ,{"", "3125.00", "62.50", "50.00", "12.50", "750.00", "", "", "15.00", "10.00", "40.00", "4.17"}
                ,{"", "250.00", "62.50", "4.00", "0.63", "37.50", "", "", "9.38", "6.25", "40.00", "6.67"}
                ,{"", "62.50", "62.50", "1000.00", "250.00", "15.00", "", "", "15.00", "10.00", "40.00", "4.17"}
                ,{"", "12.50", "62.50", "200.00", "25.00", "1500.00", "", "", "7.50", "5.00", "40.00", "8.33"}
                ,{"", "15.50", "62.50", "200.00", "12.50", "750.00", "", "", "3.75", "2.50", "40.00", "16.67"}
                ,{"", "12.50", "125.00", "0.10", "0.03", "1.50", "", "", "15.00", "10.00", "40.00", "8.33"}
            },

            {
                {"", "10.00", "20.00", "500.00", "33.33", "2000.00", "", "", "4.00", "0.00", "0.00", "5.00"}
                ,{"", "10.00", "20.00", "500.00", "33.33", "2000.00", "", "", "4.00", "0.00", "0.00", "5.00"}
                ,{"", "10.00", "20.00", "500.00", "33.33", "2000.00", "", "", "4.00", "0.00", "0.00", "5.00"}
                ,{"", "10.00", "20.00", "0.50", "0.03", "2.00", "", "", "4.00", "0.00", "0.00", "5.00"}
                ,{"", "10.00", "20.00", "0.50", "0.03", "2.00", "", "", "4.00", "0.00", "0.00", "5.00"}
                ,{"", "50.00", "50.00", "1000.00", "100.00", "6000.00", "", "", "6.00", "0.00", "0.00", "8.33"}
                ,{"", "50.00", "25.00", "2000.00", "10.00", "6000.00", "", "", "0.30", "0.00", "0.00", "83.33"}
                ,{"", "40.00", "25.00", "1600.00", "10.00", "600.00", "", "", "0.37", "0.00", "0.00", "66.67"}
                ,{"", "400.00", "25.00", "16.00", "1.00", "60.00", "", "", "3.75", "0.00", "0.00", "6.67"}
                ,{"", "1250.00", "25.00", "50.00", "5.00", "300.00", "", "", "6.00", "0.00", "0.00", "4.17"}
                ,{"", "100.00", "25.00", "4.00", "0.25", "15.00", "", "", "3.75", "0.00", "0.00", "6.67"}
                ,{"", "25.00", "25.00", "1000.00", "100.00", "6000.00", "", "", "6.00", "0.00", "0.00", "4.17"}
                ,{"", "5000.00", "25.00", "200.00", "10.00", "600.00", "", "", "3.00", "0.00", "0.00", "8.33"}
                ,{"", "5000.00", "25.00", "200.00", "5.00", "300.00", "", "", "1.50", "0.00", "0.00", "16.67"}
                ,{"", "5.00", "50.00", "0.10", "0.01", "0.60", "", "", "6.00", "0.00", "0.00", "8.33"}
            }
        };
        return symbol[type][name][param];
    }

    static const char *convert(PatientType type, DrugUnit unit, DoseCalculationParam param)
    {
        static const char *symbol[PATIENT_TYPE_NR][DRUG_UNIT_NR][CALCULATION_PARAM_NR] =
        {
            {
                {"kg" , "mg", "ml", "mcg/ml", "mcg/min", "mg/h", "mcg/kg/min", "mcg/kg/h", "ml/h", "gtt/min", "gtt/ml", "h"}
                ,{"kg" , "unit", "ml", "unit/ml", "unit/min", "unit/h", "unit/kg/min", "unit/kg/h", "ml/h", "gtt/min", "gtt/ml", "h"}
                ,{"kg" , "mEq", "ml", "mEq/ml", "mEq/min", "mEq/h", "mEq/kg/min", "mEq/kg/h", "ml/h", "gtt/min", "gtt/ml", "h"}
            }
            ,{
                {"kg" , "mg", "ml", "mcg/ml", "mcg/min", "mg/h", "mcg/kg/min", "mcg/kg/h", "ml/h", "gtt/min", "gtt/ml", "h"}
                ,{"kg" , "unit", "ml", "unit/ml", "unit/min", "unit/h", "unit/kg/min", "unit/kg/h", "ml/h", "gtt/min", "gtt/ml", "h"}
                ,{"kg" , "mEq", "ml", "mEq/ml", "mEq/min", "mEq/h", "mEq/kg/min", "mEq/kg/h", "ml/h", "gtt/min", "gtt/ml", "h"}
            }
            ,{
                {"kg" , "mg", "ml", "mcg/ml", "mcg/min", "mcg/h", "mcg/kg/min", "mcg/kg/h", "ml/h", "gtt/min", "gtt/ml", "h"}
                ,{"kg" , "unit", "ml", "unit/ml", "unit/min", "unit/h", "unit/kg/min", "unit/kg/h", "ml/h", "gtt/min", "gtt/ml", "h"}
                ,{"kg" , "mEq", "ml", "mEq/ml", "mEq/min", "mEq/h", "mEq/kg/min", "mEq/kg/h", "ml/h", "gtt/min", "gtt/ml", "h"}
            }
        };
        return symbol[type][unit][param];
    }

    static const DoseCalculationParam &convert(TitrateTableParam param)
    {
        static const DoseCalculationParam symbol[TITRATE_TABLE_NR] =
        {
            CALCULATION_PARAM_WEIGHT,
            CALCULATION_PARAM_TOTAL,
            CALCULATION_PARAM_VOLUME,
            CALCULATION_PARAM_DOSE1,
            CALCULATION_PARAM_INFUSIONRATE,
            CALCULATION_PARAM_DRIPRATE
        };
        return symbol[param];
    }
};
