#include "TitrateTableManager.h"

TitrateTableManager *TitrateTableManager::_selfObj = NULL;

/**************************************************************************************************
 * 名称： 析构函数
 * 功能：
 *************************************************************************************************/
TitrateTableManager::~TitrateTableManager()
{

}

void TitrateTableManager::setStep(StepOption step)
{
    _setTableParam.step = step;
}

void TitrateTableManager::setDatumTerm(DatumTerm datumTerm)
{
    _setTableParam.datumTerm = datumTerm;
}

void TitrateTableManager::setDoseType(DoseType doseType)
{
    _setTableParam.doseType = doseType;
}

const SetTableParam &TitrateTableManager::getSetTableParam()
{
    return _setTableParam;
}

/**************************************************************************************************
 * 名称： 构造函数
 * 功能：
 *************************************************************************************************/
TitrateTableManager::TitrateTableManager()
{

}
