#include "DoseCalculationManager.h"

DoseCalculationManager *DoseCalculationManager::_selfObj = NULL;

/**************************************************************************************************
 * 功能： 析构。
 *************************************************************************************************/
DoseCalculationManager::~DoseCalculationManager()
{

}

/**************************************************************************************************
 * 功能： 设置病人类型。
 *************************************************************************************************/
void DoseCalculationManager::setType(PatientType type)
{
    _doseCalculationInfo.type = type;
}

/**************************************************************************************************
 * 功能： 获取病人类型。
 *************************************************************************************************/
PatientType DoseCalculationManager::getType()
{
    return _doseCalculationInfo.type;
}

/**************************************************************************************************
 * 功能： 设置药物名称。
 *************************************************************************************************/
void DoseCalculationManager::setDrugName(DrugName name)
{
    _doseCalculationInfo.name = name;
}

/**************************************************************************************************
 * 功能： 获取药物名称。
 *************************************************************************************************/
DrugName DoseCalculationManager::getDrugName()
{
    return _doseCalculationInfo.name;
}

/**************************************************************************************************
 * 功能： 设置药物单位类型
 *************************************************************************************************/
void DoseCalculationManager::setDrugUnit(DrugUnit unit)
{
    _doseCalculationInfo.unit = unit;
}

/**************************************************************************************************
 * 功能： 获取药物单位类型
 *************************************************************************************************/
DrugUnit DoseCalculationManager::getDrugUnit()
{
    return _doseCalculationInfo.unit;
}

/**************************************************************************************************
 * 功能： 存储指定位置药物参数的值和单位
 *************************************************************************************************/
void DoseCalculationManager::setDrugParam(int index, float value, QString unit)
{
    if ((index <= CALCULATION_PARAM_DOSE4 && value >= 10000) ||
            (index > CALCULATION_PARAM_DOSE4 && value >= 1000))
    {
        value = -1;
    }
    _doseCalculationInfo.drugParam[index].value = value;
    _doseCalculationInfo.drugParam[index].unit = unit;
}

/**************************************************************************************************
 * 功能： 获取指定位置的药物计算参数
 *************************************************************************************************/
DrugInfo DoseCalculationManager::getDrugParam(int index)
{
    return _doseCalculationInfo.drugParam[index];
}

/**************************************************************************************************
 * 功能： 获取药物计算信息。
 *************************************************************************************************/
const DoseCalculationInfo &DoseCalculationManager::getdoseCalculationInfo(void)
{
    return _doseCalculationInfo;
}

/**************************************************************************************************
 * 功能： 传入改变的药物计算参数，计算其他变化的药物参数值，并存储。
 *************************************************************************************************/
void DoseCalculationManager::doseCalcAction(DoseCalculationParam index, float value)
{
    setDrugParam(index, value, _doseCalculationInfo.drugParam[index].unit);
    switch ((int)index)
    {
    case CALCULATION_PARAM_WEIGHT:
    {
        DrugInfo doseKgmin;
        DrugInfo doseKgh;
        doseKgmin.value = getDrugParam(CALCULATION_PARAM_DOSE1).value/value;
        doseKgmin.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
        doseKgh.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/value;
        doseKgh.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
        unitConversion(doseKgmin);
        unitConversion(doseKgh);
        setDrugParam(CALCULATION_PARAM_DOSE3, doseKgmin.value, doseKgmin.unit + "/kg/min");
        setDrugParam(CALCULATION_PARAM_DOSE4, doseKgh.value, doseKgh.unit + "/kg/h");
        break;
    }
    case CALCULATION_PARAM_TOTAL:
    {
        unitConversion(_doseCalculationInfo.drugParam[index]);
        setDrugParam(index, _doseCalculationInfo.drugParam[index].value, _doseCalculationInfo.drugParam[index].unit);

        DrugInfo concentration;
        concentration.value = getDrugParam(CALCULATION_PARAM_TOTAL).value/getDrugParam(CALCULATION_PARAM_VOLUME).value;
        concentration.unit = getDrugParam(CALCULATION_PARAM_TOTAL).unit;
        unitConversion(concentration);
        setDrugParam(CALCULATION_PARAM_CONCENTRATION, concentration.value, concentration.unit + "/ml");

        DrugInfo doseh;
        doseh.value = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).value * getDrugParam(CALCULATION_PARAM_CONCENTRATION).value;
        doseh.unit = getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3);
        unitConversion(doseh);
        setDrugParam(CALCULATION_PARAM_DOSE2, doseh.value, doseh.unit + "/h");

        DrugInfo dosemin;
        dosemin.value = getDrugParam(CALCULATION_PARAM_DOSE2).value / 60;
        dosemin.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
        unitConversion(dosemin);
        setDrugParam(CALCULATION_PARAM_DOSE1, dosemin.value, dosemin.unit + "/min");

        if(getDrugParam(CALCULATION_PARAM_WEIGHT).value != 0)
        {
            DrugInfo doseKgmin;
            doseKgmin.value = getDrugParam(CALCULATION_PARAM_DOSE1).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgmin.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
            unitConversion(doseKgmin);
            setDrugParam(CALCULATION_PARAM_DOSE3, doseKgmin.value, doseKgmin.unit + "/kg/min");

            DrugInfo doseKgh;
            doseKgh.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgh.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
            unitConversion(doseKgh);
            setDrugParam(CALCULATION_PARAM_DOSE4, doseKgh.value, doseKgh.unit + "/kg/h");
        }

        break;
    }
    case CALCULATION_PARAM_VOLUME:
    {
        DrugInfo concentration;
        concentration.value = getDrugParam(CALCULATION_PARAM_TOTAL).value/value;
        concentration.unit =  getDrugParam(CALCULATION_PARAM_TOTAL).unit;
        unitConversion(concentration);
        setDrugParam(CALCULATION_PARAM_CONCENTRATION, concentration.value, concentration.unit + "/ml");

        DrugInfo doseh;
        doseh.value = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).value * getDrugParam(CALCULATION_PARAM_CONCENTRATION).value;
        doseh.unit = getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3);
        unitConversion(doseh);
        setDrugParam(CALCULATION_PARAM_DOSE2, doseh.value, doseh.unit + "/h");

        DrugInfo dosemin;
        dosemin.value = getDrugParam(CALCULATION_PARAM_DOSE2).value / 60;
        dosemin.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
        unitConversion(dosemin);
        setDrugParam(CALCULATION_PARAM_DOSE1, dosemin.value, dosemin.unit + "/min");

        if(getDrugParam(CALCULATION_PARAM_WEIGHT).value != 0)
        {
            DrugInfo doseKgmin;
            doseKgmin.value = getDrugParam(CALCULATION_PARAM_DOSE1).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgmin.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
            unitConversion(doseKgmin);
            setDrugParam(CALCULATION_PARAM_DOSE3, doseKgmin.value, doseKgmin.unit + "/kg/min");

            DrugInfo doseKgh;
            doseKgh.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgh.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
            unitConversion(doseKgh);
            setDrugParam(CALCULATION_PARAM_DOSE4, doseKgh.value, doseKgh.unit + "/kg/h");
        }

        DrugInfo duration;
        int multiple1;
        int multiple2;
        unifyUnit(getDrugParam(CALCULATION_PARAM_TOTAL).unit,
                  getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2),
                  multiple1,
                  multiple2);
        duration.value = (getDrugParam(CALCULATION_PARAM_TOTAL).value * multiple1)
                /(getDrugParam(CALCULATION_PARAM_DOSE2).value * multiple2);
        duration.unit = getDrugParam(CALCULATION_PARAM_DURATION).unit;
        setDrugParam(CALCULATION_PARAM_DURATION, duration.value, duration.unit);

        break;
    }
    case CALCULATION_PARAM_CONCENTRATION:
    {
        DrugInfo concentration;
        concentration.value = getDrugParam(CALCULATION_PARAM_CONCENTRATION).value;
        concentration.unit =  getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3);
        unitConversion(concentration);
        setDrugParam(CALCULATION_PARAM_CONCENTRATION, concentration.value, concentration.unit + "/ml");

        DrugInfo doseh;
        doseh.value = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).value * getDrugParam(CALCULATION_PARAM_CONCENTRATION).value;
        doseh.unit = getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3);
        unitConversion(doseh);
        setDrugParam(CALCULATION_PARAM_DOSE2, doseh.value, doseh.unit + "/h");

        DrugInfo dosemin;
        dosemin.value = getDrugParam(CALCULATION_PARAM_DOSE2).value / 60;
        dosemin.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
        unitConversion(dosemin);
        setDrugParam(CALCULATION_PARAM_DOSE1, dosemin.value, dosemin.unit + "/min");

        if(getDrugParam(CALCULATION_PARAM_WEIGHT).value != 0)
        {
            DrugInfo doseKgmin;
            doseKgmin.value = getDrugParam(CALCULATION_PARAM_DOSE1).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgmin.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
            unitConversion(doseKgmin);
            setDrugParam(CALCULATION_PARAM_DOSE3, doseKgmin.value, doseKgmin.unit + "/kg/min");

            DrugInfo doseKgh;
            doseKgh.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgh.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
            unitConversion(doseKgh);
            setDrugParam(CALCULATION_PARAM_DOSE4, doseKgh.value, doseKgh.unit + "/kg/h");
        }

        DrugInfo total;
        total.value = getDrugParam(CALCULATION_PARAM_CONCENTRATION).value * getDrugParam(CALCULATION_PARAM_VOLUME).value;
        total.unit = getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3);
        unitConversion(total);
        setDrugParam(CALCULATION_PARAM_TOTAL, total.value, total.unit);

        break;
    }
    case CALCULATION_PARAM_DOSE1:
    {
        DrugInfo dosemin;
        dosemin.value = getDrugParam(CALCULATION_PARAM_DOSE1).value;
        dosemin.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
        unitConversion(dosemin);
        setDrugParam(CALCULATION_PARAM_DOSE1, dosemin.value, dosemin.unit + "/min");

        DrugInfo doseh;
        doseh.value = getDrugParam(CALCULATION_PARAM_DOSE1).value * 60;
        doseh.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
        unitConversion(doseh);
        setDrugParam(CALCULATION_PARAM_DOSE2, doseh.value, doseh.unit + "/h");

        if(getDrugParam(CALCULATION_PARAM_WEIGHT).value != 0)
        {
            DrugInfo doseKgmin;
            doseKgmin.value = getDrugParam(CALCULATION_PARAM_DOSE1).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgmin.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
            unitConversion(doseKgmin);
            setDrugParam(CALCULATION_PARAM_DOSE3, doseKgmin.value, doseKgmin.unit + "/kg/min");

            DrugInfo doseKgh;
            doseKgh.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgh.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
            unitConversion(doseKgh);
            setDrugParam(CALCULATION_PARAM_DOSE4, doseKgh.value, doseKgh.unit + "/kg/h");
        }

        DrugInfo infusionrate;
        int multiple1;
        int multiple2;
        unifyUnit(getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2),
                  getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3),
                  multiple1,
                  multiple2);
        infusionrate.value = (getDrugParam(CALCULATION_PARAM_DOSE2).value * multiple1)
                /(getDrugParam(CALCULATION_PARAM_CONCENTRATION).value * multiple2);
        infusionrate.unit = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).unit;
        setDrugParam(CALCULATION_PARAM_INFUSIONRATE, infusionrate.value, infusionrate.unit);

        DrugInfo dripRate;
        dripRate.value = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).value*getDrugParam(CALCULATION_PARAM_DROPSIZE).value/60;
        dripRate.unit = getDrugParam(CALCULATION_PARAM_DRIPRATE).unit;
        setDrugParam(CALCULATION_PARAM_DRIPRATE, dripRate.value, dripRate.unit);

        DrugInfo duration;
        unifyUnit(getDrugParam(CALCULATION_PARAM_TOTAL).unit,
                  getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2),
                  multiple1,
                  multiple2);
        duration.value = (getDrugParam(CALCULATION_PARAM_TOTAL).value * multiple1)
                /(getDrugParam(CALCULATION_PARAM_DOSE2).value * multiple2);
        duration.unit = getDrugParam(CALCULATION_PARAM_DURATION).unit;
        setDrugParam(CALCULATION_PARAM_DURATION, duration.value, duration.unit);
        break;
    }
    case CALCULATION_PARAM_DOSE2:
    {
        DrugInfo doseh;
        doseh.value = getDrugParam(CALCULATION_PARAM_DOSE2).value;
        doseh.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
        unitConversion(doseh);
        setDrugParam(CALCULATION_PARAM_DOSE2, doseh.value, doseh.unit + "/h");

        DrugInfo dosemin;
        dosemin.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/60;
        dosemin.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
        unitConversion(dosemin);
        setDrugParam(CALCULATION_PARAM_DOSE1, dosemin.value, dosemin.unit + "/min");

        if(getDrugParam(CALCULATION_PARAM_WEIGHT).value != 0)
        {
            DrugInfo doseKgmin;
            doseKgmin.value = getDrugParam(CALCULATION_PARAM_DOSE1).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgmin.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
            unitConversion(doseKgmin);
            setDrugParam(CALCULATION_PARAM_DOSE3, doseKgmin.value, doseKgmin.unit + "/kg/min");

            DrugInfo doseKgh;
            doseKgh.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgh.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
            unitConversion(doseKgh);
            setDrugParam(CALCULATION_PARAM_DOSE4, doseKgh.value, doseKgh.unit + "/kg/h");
        }

        DrugInfo infusionrate;
        int multiple1;
        int multiple2;
        unifyUnit(getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2),
                  getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3),
                  multiple1,
                  multiple2);
        infusionrate.value = (getDrugParam(CALCULATION_PARAM_DOSE2).value * multiple1)
                /(getDrugParam(CALCULATION_PARAM_CONCENTRATION).value * multiple2);
        infusionrate.unit = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).unit;
        setDrugParam(CALCULATION_PARAM_INFUSIONRATE, infusionrate.value, infusionrate.unit);

        DrugInfo dripRate;
        dripRate.value = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).value*getDrugParam(CALCULATION_PARAM_DROPSIZE).value/60;
        dripRate.unit = getDrugParam(CALCULATION_PARAM_DRIPRATE).unit;
        setDrugParam(CALCULATION_PARAM_DRIPRATE, dripRate.value, dripRate.unit);

        DrugInfo duration;
        unifyUnit(getDrugParam(CALCULATION_PARAM_TOTAL).unit,
                  getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2),
                  multiple1,
                  multiple2);
        duration.value = (getDrugParam(CALCULATION_PARAM_TOTAL).value * multiple1)
                /(getDrugParam(CALCULATION_PARAM_DOSE2).value * multiple2);
        duration.unit = getDrugParam(CALCULATION_PARAM_DURATION).unit;
        setDrugParam(CALCULATION_PARAM_DURATION, duration.value, duration.unit);
        break;
    }
    case CALCULATION_PARAM_DOSE3:
    {
        DrugInfo doseKgmin;
        doseKgmin.value = getDrugParam(CALCULATION_PARAM_DOSE3).value;
        doseKgmin.unit = getDrugParam(CALCULATION_PARAM_DOSE3).unit.left(getDrugParam(CALCULATION_PARAM_DOSE3).unit.length() - 7);
        unitConversion(doseKgmin);
        setDrugParam(CALCULATION_PARAM_DOSE3, doseKgmin.value, doseKgmin.unit + "/kg/min");

        if(getDrugParam(CALCULATION_PARAM_WEIGHT).value != 0)
        {
            DrugInfo dosemin;
            dosemin.value = getDrugParam(CALCULATION_PARAM_DOSE3).value*getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            dosemin.unit = getDrugParam(CALCULATION_PARAM_DOSE3).unit.left(getDrugParam(CALCULATION_PARAM_DOSE3).unit.length() - 7);
            unitConversion(dosemin);
            setDrugParam(CALCULATION_PARAM_DOSE1, dosemin.value, dosemin.unit + "/min");

            DrugInfo doseh;
            doseh.value = getDrugParam(CALCULATION_PARAM_DOSE1).value*60;
            doseh.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
            unitConversion(doseh);
            setDrugParam(CALCULATION_PARAM_DOSE2, doseh.value, doseh.unit + "/h");

            DrugInfo doseKgh;
            doseKgh.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgh.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
            unitConversion(doseKgh);
            setDrugParam(CALCULATION_PARAM_DOSE4, doseKgh.value, doseKgh.unit + "/kg/h");

            DrugInfo infusionrate;
            int multiple1;
            int multiple2;
            unifyUnit(getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2),
                      getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3),
                      multiple1,
                      multiple2);
            infusionrate.value = (getDrugParam(CALCULATION_PARAM_DOSE2).value * multiple1)
                    /(getDrugParam(CALCULATION_PARAM_CONCENTRATION).value * multiple2);
            infusionrate.unit = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).unit;
            setDrugParam(CALCULATION_PARAM_INFUSIONRATE, infusionrate.value, infusionrate.unit);

            DrugInfo dripRate;
            dripRate.value = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).value*getDrugParam(CALCULATION_PARAM_DROPSIZE).value/60;
            dripRate.unit = getDrugParam(CALCULATION_PARAM_DRIPRATE).unit;
            setDrugParam(CALCULATION_PARAM_DRIPRATE, dripRate.value, dripRate.unit);

            DrugInfo duration;
            unifyUnit(getDrugParam(CALCULATION_PARAM_TOTAL).unit,
                      getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2),
                      multiple1,
                      multiple2);
            duration.value = (getDrugParam(CALCULATION_PARAM_TOTAL).value * multiple1)
                    /(getDrugParam(CALCULATION_PARAM_DOSE2).value * multiple2);
            duration.unit = getDrugParam(CALCULATION_PARAM_DURATION).unit;
            setDrugParam(CALCULATION_PARAM_DURATION, duration.value, duration.unit);
        }
        break;
    }
    case CALCULATION_PARAM_DOSE4:
    {
        DrugInfo doseKgh;
        doseKgh.value = getDrugParam(CALCULATION_PARAM_DOSE4).value;
        doseKgh.unit = getDrugParam(CALCULATION_PARAM_DOSE4).unit.left(getDrugParam(CALCULATION_PARAM_DOSE4).unit.length() - 5);
        unitConversion(doseKgh);
        setDrugParam(CALCULATION_PARAM_DOSE4, doseKgh.value, doseKgh.unit + "/kg/h");

        if(getDrugParam(CALCULATION_PARAM_WEIGHT).value != 0)
        {
            DrugInfo doseh;
            doseh.value = getDrugParam(CALCULATION_PARAM_DOSE4).value * getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseh.unit = getDrugParam(CALCULATION_PARAM_DOSE4).unit.left(getDrugParam(CALCULATION_PARAM_DOSE4).unit.length() - 5);
            unitConversion(doseh);
            setDrugParam(CALCULATION_PARAM_DOSE2, doseh.value, doseh.unit + "/h");

            DrugInfo dosemin;
            dosemin.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/60;
            dosemin.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
            unitConversion(dosemin);
            setDrugParam(CALCULATION_PARAM_DOSE1, dosemin.value, dosemin.unit + "/min");

            DrugInfo doseKgmin;
            doseKgmin.value = getDrugParam(CALCULATION_PARAM_DOSE1).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgmin.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
            unitConversion(doseKgmin);
            setDrugParam(CALCULATION_PARAM_DOSE3, doseKgmin.value, doseKgmin.unit + "/kg/min");

            DrugInfo infusionrate;
            int multiple1;
            int multiple2;
            unifyUnit(getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2),
                      getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3),
                      multiple1,
                      multiple2);
            infusionrate.value = (getDrugParam(CALCULATION_PARAM_DOSE2).value * multiple1)
                    /(getDrugParam(CALCULATION_PARAM_CONCENTRATION).value * multiple2);
            infusionrate.unit = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).unit;
            setDrugParam(CALCULATION_PARAM_INFUSIONRATE, infusionrate.value, infusionrate.unit);

            DrugInfo dripRate;
            dripRate.value = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).value*getDrugParam(CALCULATION_PARAM_DROPSIZE).value/60;
            dripRate.unit = getDrugParam(CALCULATION_PARAM_DRIPRATE).unit;
            setDrugParam(CALCULATION_PARAM_DRIPRATE, dripRate.value, dripRate.unit);

            DrugInfo duration;
            unifyUnit(getDrugParam(CALCULATION_PARAM_TOTAL).unit,
                      getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2),
                      multiple1,
                      multiple2);
            duration.value = (getDrugParam(CALCULATION_PARAM_TOTAL).value * multiple1)
                    /(getDrugParam(CALCULATION_PARAM_DOSE2).value * multiple2);
            duration.unit = getDrugParam(CALCULATION_PARAM_DURATION).unit;
            setDrugParam(CALCULATION_PARAM_DURATION, duration.value, duration.unit);
        }

        break;
    }
    case CALCULATION_PARAM_INFUSIONRATE:
    {
        DrugInfo doseh;
        doseh.value = getDrugParam(CALCULATION_PARAM_CONCENTRATION).value * value;
        doseh.unit = getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3);
        unitConversion(doseh);
        setDrugParam(CALCULATION_PARAM_DOSE2, doseh.value, doseh.unit + "/h");

        DrugInfo dosemin;
        dosemin.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/60;
        dosemin.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
        unitConversion(dosemin);
        setDrugParam(CALCULATION_PARAM_DOSE1, dosemin.value, dosemin.unit + "/min");

        if(getDrugParam(CALCULATION_PARAM_WEIGHT).value != 0)
        {
            DrugInfo doseKgmin;
            doseKgmin.value = getDrugParam(CALCULATION_PARAM_DOSE1).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgmin.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
            unitConversion(doseKgmin);
            setDrugParam(CALCULATION_PARAM_DOSE3, doseKgmin.value, doseKgmin.unit + "/kg/min");

            DrugInfo doseKgh;
            doseKgh.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgh.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
            unitConversion(doseKgh);
            setDrugParam(CALCULATION_PARAM_DOSE4, doseKgh.value, doseKgh.unit + "/kg/h");
        }

        DrugInfo dripRate;
        dripRate.value = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).value*getDrugParam(CALCULATION_PARAM_DROPSIZE).value/60;
        dripRate.unit = getDrugParam(CALCULATION_PARAM_DRIPRATE).unit;
        setDrugParam(CALCULATION_PARAM_DRIPRATE, dripRate.value, dripRate.unit);

        DrugInfo duration;
        int multiple1;
        int multiple2;
        unifyUnit(getDrugParam(CALCULATION_PARAM_TOTAL).unit,
                  getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2),
                  multiple1,
                  multiple2);
        duration.value = (getDrugParam(CALCULATION_PARAM_TOTAL).value * multiple1)
                /(getDrugParam(CALCULATION_PARAM_DOSE2).value * multiple2);
        duration.unit = getDrugParam(CALCULATION_PARAM_DURATION).unit;
        setDrugParam(CALCULATION_PARAM_DURATION, duration.value, duration.unit);

        break;
    }
    case CALCULATION_PARAM_DRIPRATE:
    {
        DrugInfo infusionrate;
        infusionrate.value =value * 60/getDrugParam(CALCULATION_PARAM_DROPSIZE).value;
        infusionrate.unit = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).unit;
        setDrugParam(CALCULATION_PARAM_INFUSIONRATE, infusionrate.value, infusionrate.unit);

        DrugInfo doseh;
        doseh.value = getDrugParam(CALCULATION_PARAM_CONCENTRATION).value * getDrugParam(CALCULATION_PARAM_INFUSIONRATE).value;
        doseh.unit = getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3);
        unitConversion(doseh);
        setDrugParam(CALCULATION_PARAM_DOSE2, doseh.value, doseh.unit + "/h");

        DrugInfo dosemin;
        dosemin.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/60;
        dosemin.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
        unitConversion(dosemin);
        setDrugParam(CALCULATION_PARAM_DOSE1, dosemin.value, dosemin.unit + "/min");

        if(getDrugParam(CALCULATION_PARAM_WEIGHT).value != 0)
        {
            DrugInfo doseKgmin;
            doseKgmin.value = getDrugParam(CALCULATION_PARAM_DOSE1).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgmin.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
            unitConversion(doseKgmin);
            setDrugParam(CALCULATION_PARAM_DOSE3, doseKgmin.value, doseKgmin.unit + "/kg/min");

            DrugInfo doseKgh;
            doseKgh.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgh.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
            unitConversion(doseKgh);
            setDrugParam(CALCULATION_PARAM_DOSE4, doseKgh.value, doseKgh.unit + "/kg/h");
        }

        DrugInfo duration;
        int multiple1;
        int multiple2;
        unifyUnit(getDrugParam(CALCULATION_PARAM_TOTAL).unit,
                  getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2),
                  multiple1,
                  multiple2);
        duration.value = (getDrugParam(CALCULATION_PARAM_TOTAL).value * multiple1)
                /(getDrugParam(CALCULATION_PARAM_DOSE2).value * multiple2);
        duration.unit = getDrugParam(CALCULATION_PARAM_DURATION).unit;
        setDrugParam(CALCULATION_PARAM_DURATION, duration.value, duration.unit);

        break;
    }
    case CALCULATION_PARAM_DROPSIZE:
    {
        DrugInfo infusionrate;
        infusionrate.value =getDrugParam(CALCULATION_PARAM_DRIPRATE).value * 60/value;
        infusionrate.unit = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).unit;
        setDrugParam(CALCULATION_PARAM_INFUSIONRATE, infusionrate.value, infusionrate.unit);

        DrugInfo doseh;
        doseh.value = getDrugParam(CALCULATION_PARAM_CONCENTRATION).value * getDrugParam(CALCULATION_PARAM_INFUSIONRATE).value;
        doseh.unit = getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3);
        unitConversion(doseh);
        setDrugParam(CALCULATION_PARAM_DOSE2, doseh.value, doseh.unit + "/h");

        DrugInfo dosemin;
        dosemin.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/60;
        dosemin.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
        unitConversion(dosemin);
        setDrugParam(CALCULATION_PARAM_DOSE1, dosemin.value, dosemin.unit + "/min");

        if(getDrugParam(CALCULATION_PARAM_WEIGHT).value != 0)
        {
            DrugInfo doseKgmin;
            doseKgmin.value = getDrugParam(CALCULATION_PARAM_DOSE1).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgmin.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
            unitConversion(doseKgmin);
            setDrugParam(CALCULATION_PARAM_DOSE3, doseKgmin.value, doseKgmin.unit + "/kg/min");

            DrugInfo doseKgh;
            doseKgh.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgh.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
            unitConversion(doseKgh);
            setDrugParam(CALCULATION_PARAM_DOSE4, doseKgh.value, doseKgh.unit + "/kg/h");
        }

        DrugInfo duration;
        int multiple1;
        int multiple2;
        unifyUnit(getDrugParam(CALCULATION_PARAM_TOTAL).unit,
                  getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2),
                  multiple1,
                  multiple2);
        duration.value = (getDrugParam(CALCULATION_PARAM_TOTAL).value * multiple1)
                /(getDrugParam(CALCULATION_PARAM_DOSE2).value * multiple2);
        duration.unit = getDrugParam(CALCULATION_PARAM_DURATION).unit;
        setDrugParam(CALCULATION_PARAM_DURATION, duration.value, duration.unit);

        break;
    }
    case CALCULATION_PARAM_DURATION:
    {
        DrugInfo doseh;
        doseh.value = getDrugParam(CALCULATION_PARAM_TOTAL).value / value;
        doseh.unit = getDrugParam(CALCULATION_PARAM_TOTAL).unit;
        unitConversion(doseh);
        setDrugParam(CALCULATION_PARAM_DOSE2, doseh.value, doseh.unit + "/h");

        DrugInfo dosemin;
        dosemin.value = getDrugParam(CALCULATION_PARAM_TOTAL).value / value/60;
        dosemin.unit = getDrugParam(CALCULATION_PARAM_TOTAL).unit;
        unitConversion(dosemin);
        setDrugParam(CALCULATION_PARAM_DOSE1, dosemin.value, dosemin.unit + "/min");

        if(getDrugParam(CALCULATION_PARAM_WEIGHT).value != 0)
        {
            DrugInfo doseKgmin;
            doseKgmin.value = getDrugParam(CALCULATION_PARAM_DOSE1).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgmin.unit = getDrugParam(CALCULATION_PARAM_DOSE1).unit.left(getDrugParam(CALCULATION_PARAM_DOSE1).unit.length() - 4);
            unitConversion(doseKgmin);
            setDrugParam(CALCULATION_PARAM_DOSE3, doseKgmin.value, doseKgmin.unit + "/kg/min");

            DrugInfo doseKgh;
            doseKgh.value = getDrugParam(CALCULATION_PARAM_DOSE2).value/getDrugParam(CALCULATION_PARAM_WEIGHT).value;
            doseKgh.unit = getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2);
            unitConversion(doseKgh);
            setDrugParam(CALCULATION_PARAM_DOSE4, doseKgh.value, doseKgh.unit + "/kg/h");
        }

        DrugInfo infusionrate;
        int multiple1;
        int multiple2;
        unifyUnit(getDrugParam(CALCULATION_PARAM_DOSE2).unit.left(getDrugParam(CALCULATION_PARAM_DOSE2).unit.length() - 2),
                  getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.left(getDrugParam(CALCULATION_PARAM_CONCENTRATION).unit.length() - 3),
                  multiple1,
                  multiple2);
        infusionrate.value = (getDrugParam(CALCULATION_PARAM_DOSE2).value * multiple1)
                /(getDrugParam(CALCULATION_PARAM_CONCENTRATION).value * multiple2);
        infusionrate.unit = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).unit;
        setDrugParam(CALCULATION_PARAM_INFUSIONRATE, infusionrate.value, infusionrate.unit);

        DrugInfo dripRate;
        dripRate.value = getDrugParam(CALCULATION_PARAM_INFUSIONRATE).value*getDrugParam(CALCULATION_PARAM_DROPSIZE).value/60;
        dripRate.unit = getDrugParam(CALCULATION_PARAM_DRIPRATE).unit;
        setDrugParam(CALCULATION_PARAM_DRIPRATE, dripRate.value, dripRate.unit);
        break;
    }
    }
}

/**************************************************************************************************
 * 功能： 根据传入药物参数的数值和单位，进行单位转换，大于10000进位，小于10降位
 *************************************************************************************************/
void DoseCalculationManager::unitConversion(DrugInfo &drugInfo)
{
    if (drugInfo.value > 10000)
    {
        drugInfo.value = drugInfo.value/1000;
        if (drugInfo.value > 10000)
        {
            drugInfo.value = drugInfo.value/1000;
           if (drugInfo.unit == "unit")
           {
               drugInfo.unit = "MU";
           }
           else if (drugInfo.unit == "mcg")
           {
               drugInfo.unit = "g";
           }
           else
           {
               drugInfo.value = -1;
           }
        }
        else
        {
            if (drugInfo.unit == "g" ||
                    drugInfo.unit == "MU" ||
                    drugInfo.unit == "mEq")
            {
                drugInfo.value = -1;
            }
            else if(drugInfo.unit == "mg")
            {
                drugInfo.unit = "g";
            }
            else if(drugInfo.unit == "KU")
            {
                drugInfo.unit = "MU";
            }
            else if(drugInfo.unit == "mcg")
            {
                drugInfo.unit = "mg";
            }
            else if(drugInfo.unit == "unit")
            {
                drugInfo.unit = "KU";
            }
        }
    }
    else if (drugInfo.value < 10)
    {
        if (drugInfo.unit == "mcg" ||
                drugInfo.unit == "unit" ||
                drugInfo.unit == "mEq")
        {
        }
        else if(drugInfo.unit == "mg")
        {
            drugInfo.unit = "mcg";
            drugInfo.value = drugInfo.value*1000;
        }
        else if(drugInfo.unit == "KU")
        {
            drugInfo.unit = "unit";
            drugInfo.value = drugInfo.value*1000;
        }
        else if(drugInfo.unit == "g")
        {
            drugInfo.unit = "mg";
            drugInfo.value = drugInfo.value*1000;
        }
        else if(drugInfo.unit == "MU")
        {
            drugInfo.unit = "KU";
            drugInfo.value = drugInfo.value*1000;
        }
    }
    else
    {
        if (drugInfo.unit == "g")
        {
            drugInfo.unit = "mg";
            drugInfo.value = 9999.99;
        }
        else if (drugInfo.unit == "MU")
        {
            drugInfo.unit = "KU";
            drugInfo.value = 9999.99;
        }
    }
}

/**************************************************************************************************
 * 功能： 根据传入的两个数值和单位，进行单位统一，大单位换小单位。
 *************************************************************************************************/
void DoseCalculationManager::unifyUnit(QString unit1, QString unit2, int &multiple1, int &multiple2)
{
    if(unit1 == unit2)
    {
        multiple1 = 1;
        multiple2 = 1;
    }
    else if(((unit1 == "g") && (unit2 == "mg"))
            || ((unit1 == "mg") && (unit2 == "mcg"))
            || ((unit1 == "MU") && (unit2 == "KU"))
            || ((unit1 == "KU") && (unit2 == "unit")))
    {
        multiple1 = 1000;
        multiple2 = 1;
    }
    else if(((unit1 == "mg") && (unit2 == "g"))
            || ((unit1 == "mcg") && (unit2 == "mg"))
            || ((unit1 == "KU") && (unit2 == "MU"))
            || ((unit1 == "unit") && (unit2 == "KU")))
    {
        multiple1 = 1;
        multiple2 = 1000;
    }
}

/**************************************************************************************************
 * 功能： 将传入的目标参数值和单位，转换为传入的指定单位
 *************************************************************************************************/
void DoseCalculationManager::specifyUnit(DrugInfo &param, QString unit)
{
    if (((param.unit == "mcg") && (unit == "mg"))
            || ((param.unit == "mg") && (unit == "g")))
    {
        param.unit = unit;
        param.value = param.value/1000;
    }
    else if (((param.unit == "mg") && (unit == "mcg"))
             || ((param.unit == "g") && (unit == "mg")))
    {
        param.unit = unit;
        param.value = param.value * 1000;
    }
    else if((param.unit == "mcg") && (unit == "g"))
    {
        param.unit = unit;
        param.value = param.value / 1000 / 1000;
    }
    else if((param.unit == "g") && (unit == "mcg"))
    {
        param.unit = unit;
        param.value = param.value * 1000 * 1000;
    }
}

/**************************************************************************************************
 * 功能： 构造
 *************************************************************************************************/
DoseCalculationManager::DoseCalculationManager()
{

}
