/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/30
 **/

#include "ParamInfo.h"
#include <QString>
#include "Debug.h"
#include "SystemManager.h"
#include "ECGSymbol.h"
#include "ECGParam.h"

/**************************************************************************************************
 * 功能： 参数ID与名字的映射表。
 *************************************************************************************************/
static const char *_paramNames[PARAM_NR] =
{
    "ECG", "SPO2", "RESP", "NIBP", "CO2",
    "TEMP", "ECGDUP", "RESPDUP", "IBP", "AG",
    "CO", "O2", "UPGRADE"
};

/**************************************************************************************************
 * 功能： 子参数ID与扩展名字的映射表。
 *************************************************************************************************/
static const char *_subParamNames(SubParamID paramID, bool ignoreModuleConfig)
{
    switch (paramID)
    {
    case SUB_PARAM_HR_PR:
        if (ignoreModuleConfig || systemManager.isSupport(CONFIG_SPO2))
        {
            return "HR_PR";
        }
        else
        {
            return "HR";
        }
    case SUB_PARAM_PLUGIN_PR:
        return "PR_2";
    case SUB_PARAM_ECG_PVCS:
        return "ECG_PVCS";

    case SUB_PARAM_SPO2:
        return "SPO2";
    case SUB_PARAM_SPO2_2:
        return "SPO2_2";
    case SUB_PARAM_SPO2_D:
        return "SPO2_D";
    case SUB_PARAM_PVI:
        return "PVI";
    case SUB_PARAM_SPHB:
        return "SPHB";
    case SUB_PARAM_SPOC:
        return "SPOC";
    case SUB_PARAM_SPMET:
        return "SPMET";
    case SUB_PARAM_PI:
        return "PI";
    case SUB_PARAM_PLUGIN_PI:
        return "PI_2";
    case SUB_PARAM_SPCO:
        return "SPCO";

    case SUB_PARAM_RR_BR:
        if (ignoreModuleConfig || (systemManager.isSupport(CONFIG_CO2) &&
                                   systemManager.isSupport(CONFIG_RESP)))
        {
            return "RR_BR";
        }
        else if (systemManager.isSupport(CONFIG_RESP))
        {
            return "RR";
        }
        else
        {
            return "BR";
        }

    case SUB_PARAM_NIBP_SYS:
        return "NIBP_SYS";

    case SUB_PARAM_NIBP_DIA:
        return "NIBP_DIA";

    case SUB_PARAM_NIBP_MAP:
        return "NIBP_MAP";

    case SUB_PARAM_ART_SYS:
        return "ART_SYS";
    case SUB_PARAM_ART_DIA:
        return "ART_DIA";
    case SUB_PARAM_ART_MAP:
        return "ART_MAP";
    case SUB_PARAM_ART_PR:
        return "ART_PR";
    case SUB_PARAM_PA_SYS:
        return "PA_SYS";
    case SUB_PARAM_PA_DIA:
        return "PA_DIA";
    case SUB_PARAM_PA_MAP:
        return "PA_MAP";
    case SUB_PARAM_PA_PR:
        return "PA_PR";
    case SUB_PARAM_CVP_MAP:
        return "CVP_MAP";
    case SUB_PARAM_CVP_PR:
        return "CVP_PR";
    case SUB_PARAM_LAP_MAP:
        return "LAP_MAP";
    case SUB_PARAM_LAP_PR:
        return "LAP_PR";
    case SUB_PARAM_RAP_MAP:
        return "RAP_MAP";
    case SUB_PARAM_RAP_PR:
        return "RAP_PR";
    case SUB_PARAM_ICP_MAP:
        return "ICP_MAP";
    case SUB_PARAM_ICP_PR:
        return "ICP_PR";
    case SUB_PARAM_AUXP1_SYS:
        return "AUXP1_SYS";
    case SUB_PARAM_AUXP1_DIA:
        return "AUXP1_DIA";
    case SUB_PARAM_AUXP1_MAP:
        return "AUXP1_MAP";
    case SUB_PARAM_AUXP1_PR:
        return "AUXP1_PR";
    case SUB_PARAM_AUXP2_SYS:
        return "AUXP2_SYS";
    case SUB_PARAM_AUXP2_DIA:
        return "AUXP2_DIA";
    case SUB_PARAM_AUXP2_MAP:
        return "AUXP2_MAP";
    case SUB_PARAM_AUXP2_PR:
        return "AUXP2_PR";
    case SUB_PARAM_CO_CO:
        return "CO_CO";

    case SUB_PARAM_CO_CI:
        return "CO_CI";

    case SUB_PARAM_CO_TB:
        return "TB";

    case SUB_PARAM_ETCO2:
        return "ETCO2";

    case SUB_PARAM_FICO2:
        return "FICO2";

    case SUB_PARAM_AWRR:
        return "AWRR";

    case SUB_PARAM_ETN2O:
        return "ETN2O";

    case SUB_PARAM_FIN2O:
        return "FIN2O";

    case SUB_PARAM_ETAA1:
        return "ETAA1";

    case SUB_PARAM_FIAA1:
        return "FIAA1";

    case SUB_PARAM_ETAA2:
        return "ETAA2";

    case SUB_PARAM_FIAA2:
        return "FIAA2";

    case SUB_PARAM_ETO2:
        return "ETO2";

    case SUB_PARAM_FIO2:
        return "FIO2";

    case SUB_PARAM_T1:
        return "T1";

    case SUB_PARAM_T2:
        return "T2";

    case SUB_PARAM_TD:
        return "TD";

    case SUB_PARAM_ST_I:
        return "ST I";
    case SUB_PARAM_ST_II:
        return "ST II";
    case SUB_PARAM_ST_III:
        return "ST III";
    case SUB_PARAM_ST_aVR:
        return "ST aVR";
    case SUB_PARAM_ST_aVL:
        return "ST aVL";
    case SUB_PARAM_ST_aVF:
        return "ST aVF";
    case SUB_PARAM_ST_V1:
        return "ST V1";
    case SUB_PARAM_ST_V2:
        return "ST V2";
    case SUB_PARAM_ST_V3:
        return "ST V3";
    case SUB_PARAM_ST_V4:
        return "ST V4";
    case SUB_PARAM_ST_V5:
        return "ST V5";
    case SUB_PARAM_ST_V6:
        return "ST V6";
    case SUB_PARAM_O2:
        return "O2";
    default:
        return NULL;
    }
}

/**************************************************************************************************
 * 功能： 子参数ID与扩展名字的映射表。
 *************************************************************************************************/
static const char *_subDupParamNames[SUB_DUP_PARAM_NR] =
{
    "HR", "PR", "RR", "BR"
};

/**************************************************************************************************
 * 功能： 获取参数的名字。
 * 参数：
 *      id： 参数ID。
 *************************************************************************************************/
const char *ParamInfo::getParamName(ParamID id)
{
    if (id < PARAM_ECG || id >= PARAM_NR)
    {
        return NULL;
    }

    return _paramNames[id];
}

/**************************************************************************************************
 * 功能： 获取参数的ID。
 * 参数：
 *      name：参数的名字。
 *************************************************************************************************/
ParamID ParamInfo::getParamID(const QString &name)
{
    for (int i = 0; i < PARAM_NR; i++)
    {
        if (name == _paramNames[i])
        {
            return (ParamID)i;
        }
    }

    return PARAM_NONE;
}

/**************************************************************************************************
 * 功能： 获取参数的ID。
 *************************************************************************************************/
ParamID ParamInfo::getParamID(SubParamID id)
{
    ParamID paramID = PARAM_NONE;

    switch (id)
    {
    case SUB_PARAM_HR_PR:
    case SUB_PARAM_PLUGIN_PR:
#ifndef HIDE_ECG_ST_PVCS_SUBPARAM
    case SUB_PARAM_ECG_PVCS:
    case SUB_PARAM_ST_I:
    case SUB_PARAM_ST_II:
    case SUB_PARAM_ST_III:
    case SUB_PARAM_ST_aVR:
    case SUB_PARAM_ST_aVL:
    case SUB_PARAM_ST_aVF:
    case SUB_PARAM_ST_V1:
    case SUB_PARAM_ST_V2:
    case SUB_PARAM_ST_V3:
    case SUB_PARAM_ST_V4:
    case SUB_PARAM_ST_V5:
    case SUB_PARAM_ST_V6:
#endif
        paramID = PARAM_DUP_ECG;
        break;
    case SUB_PARAM_RR_BR:
        paramID = PARAM_DUP_RESP;
        break;
    case SUB_PARAM_SPO2:
    case SUB_PARAM_SPO2_2:
    case SUB_PARAM_SPO2_D:
    case SUB_PARAM_PI:
    case SUB_PARAM_PLUGIN_PI:
    case SUB_PARAM_PVI:
    case SUB_PARAM_SPHB:
    case SUB_PARAM_SPOC:
    case SUB_PARAM_SPMET:
    case SUB_PARAM_SPCO:
        paramID = PARAM_SPO2;
        break;
    case SUB_PARAM_T1:
    case SUB_PARAM_T2:
    case SUB_PARAM_TD:
        paramID = PARAM_TEMP;
        break;
    case SUB_PARAM_NIBP_SYS:
    case SUB_PARAM_NIBP_DIA:
    case SUB_PARAM_NIBP_MAP:
        paramID = PARAM_NIBP;
        break;
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_ART_DIA:
    case SUB_PARAM_ART_MAP:
    case SUB_PARAM_ART_PR:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_PA_DIA:
    case SUB_PARAM_PA_MAP:
    case SUB_PARAM_PA_PR:
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_CVP_PR:
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_LAP_PR:
    case SUB_PARAM_RAP_MAP:
    case SUB_PARAM_RAP_PR:
    case SUB_PARAM_ICP_MAP:
    case SUB_PARAM_ICP_PR:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP1_DIA:
    case SUB_PARAM_AUXP1_MAP:
    case SUB_PARAM_AUXP1_PR:
    case SUB_PARAM_AUXP2_SYS:
    case SUB_PARAM_AUXP2_DIA:
    case SUB_PARAM_AUXP2_MAP:
    case SUB_PARAM_AUXP2_PR:
        paramID = PARAM_IBP;
        break;
    case SUB_PARAM_CO_CO:
    case SUB_PARAM_CO_CI:
    case SUB_PARAM_CO_TB:
        paramID = PARAM_CO;
        break;
    case SUB_PARAM_ETCO2:
    case SUB_PARAM_FICO2:
    case SUB_PARAM_AWRR:
        paramID = PARAM_CO2;
        break;
    case SUB_PARAM_ETN2O:
    case SUB_PARAM_FIN2O:
    case SUB_PARAM_ETAA1:
    case SUB_PARAM_FIAA1:
    case SUB_PARAM_ETAA2:
    case SUB_PARAM_FIAA2:
    case SUB_PARAM_ETO2:
    case SUB_PARAM_FIO2:
        paramID = PARAM_AG;
        break;
    case SUB_PARAM_O2:
        paramID = PARAM_O2;
        break;
    default:
        break;
    }

    return paramID;
}

/**************************************************************************************************
 * 功能： 获取Param支持的SubParamID。
 * 参数：
 *      ids： 返回子参数ID。
 *************************************************************************************************/
QList<SubParamID> ParamInfo::getSubParams(ParamID id)
{
    QList<SubParamID> ids;
    ids.clear();
    switch (id)
    {
    case PARAM_DUP_ECG:
        ids += SUB_PARAM_HR_PR;
        ids += SUB_PARAM_PLUGIN_PR;
        ids += SUB_PARAM_ECG_PVCS;
        ids += SUB_PARAM_ST_I;
        ids += SUB_PARAM_ST_II;
        ids += SUB_PARAM_ST_III;
        ids += SUB_PARAM_ST_aVR;
        ids += SUB_PARAM_ST_aVL;
        ids += SUB_PARAM_ST_aVF;
        ids += SUB_PARAM_ST_V1;
        ids += SUB_PARAM_ST_V2;
        ids += SUB_PARAM_ST_V3;
        ids += SUB_PARAM_ST_V4;
        ids += SUB_PARAM_ST_V5;
        ids += SUB_PARAM_ST_V6;
        break;

    case PARAM_SPO2:
        ids += SUB_PARAM_SPO2;
        break;

    case PARAM_DUP_RESP:
        ids += SUB_PARAM_RR_BR;
        break;

    case PARAM_NIBP:
        ids += SUB_PARAM_NIBP_SYS;
        ids += SUB_PARAM_NIBP_DIA;
        ids += SUB_PARAM_NIBP_MAP;
        break;

    case PARAM_CO2:
        ids += SUB_PARAM_ETCO2;
        ids += SUB_PARAM_FICO2;
        ids += SUB_PARAM_AWRR;
        break;

    case PARAM_TEMP:
        ids += SUB_PARAM_T1;
        ids += SUB_PARAM_T2;
        ids += SUB_PARAM_TD;
        break;

    default:
        break;
    }
    return ids;
}

/**************************************************************************************************
 * 功能： 获取子参数的名字。
 * 参数：
 *      id： 参数ID。
 *      ignoreModuleConfig: ignore the module config flag, if true, return the name when all related
 *                          module is working
 *************************************************************************************************/
const char *ParamInfo::getSubParamName(SubParamID id, bool ignoreModuleConfig)
{
    if (id < SUB_PARAM_NR)
    {
        return _subParamNames(id, ignoreModuleConfig);
    }
    return NULL;
}

/**************************************************************************************************
 * 功能： 获取子参数的名字。
 * 参数：
 *      id： 参数ID。
 *************************************************************************************************/
const char *ParamInfo::getSubParamName(SubDupParamID id)
{
    return _subDupParamNames[id];
}


/**
 * @brief ParamInfo::getIBPPressName get a press name according the sub param id
 * @param id
 * @return
 */
const char *ParamInfo::getIBPPressName(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_ART_DIA:
    case SUB_PARAM_ART_MAP:
    case SUB_PARAM_ART_PR:
        return "ART";
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_PA_DIA:
    case SUB_PARAM_PA_MAP:
    case SUB_PARAM_PA_PR:
        return "PA";
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_CVP_PR:
        return "CVP";
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_LAP_PR:
        return "LAP";
    case SUB_PARAM_RAP_MAP:
    case SUB_PARAM_RAP_PR:
        return "RAP";
    case SUB_PARAM_ICP_MAP:
    case SUB_PARAM_ICP_PR:
        return "ICP";
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP1_DIA:
    case SUB_PARAM_AUXP1_MAP:
    case SUB_PARAM_AUXP1_PR:
        return "P1";
    case SUB_PARAM_AUXP2_SYS:
    case SUB_PARAM_AUXP2_DIA:
    case SUB_PARAM_AUXP2_MAP:
    case SUB_PARAM_AUXP2_PR:
        return "P2";
    default:
        return "";
    }
}

/**************************************************************************************************
 * 功能： 获取波形ID对应的名字。
 * 参数：
 *      id： 参数ID。
 *************************************************************************************************/
const char *ParamInfo::getParamWaveformName(WaveformID id)
{
    switch (id)
    {
    case WAVE_ECG_I:
    case WAVE_ECG_II:
    case WAVE_ECG_III:
    case WAVE_ECG_aVR:
    case WAVE_ECG_aVL:
    case WAVE_ECG_aVF:
    case WAVE_ECG_V1:
    case WAVE_ECG_V2:
    case WAVE_ECG_V3:
    case WAVE_ECG_V4:
    case WAVE_ECG_V5:
    case WAVE_ECG_V6:
        return ECGSymbol::convert(static_cast<ECGLead>(id), ecgParam.getLeadConvention());
    case WAVE_SPO2:
        return "PLETH";
    case WAVE_SPO2_2:
        return "PLETH2";
    case WAVE_RESP:
    case WAVE_CO2:
        return getParamName(getParamID(id));
    case WAVE_N2O:
        return "N2O";
    case WAVE_AA1:
        return "AA1";
    case WAVE_AA2:
        return "AA2";
    case WAVE_O2:
        return "O2";
    case WAVE_ART:
        return "ART";
    case WAVE_PA:
        return "PA";
    case WAVE_CVP:
        return "CVP";
    case WAVE_LAP:
        return "LAP";
    case WAVE_RAP:
        return "RAP";
    case WAVE_ICP:
        return "ICP";
    case WAVE_AUXP1:
        return "P1";
    case WAVE_AUXP2:
        return "P2";
    default:
        break;
    }

    return "";
}

/**************************************************************************************************
 * 功能： 根据波形ID获取参数的ID。
 *************************************************************************************************/
ParamID ParamInfo::getParamID(WaveformID id)
{
    ParamID paramID = PARAM_NONE;
    switch (id)
    {
    case WAVE_ECG_I:
    case WAVE_ECG_II:
    case WAVE_ECG_III:
    case WAVE_ECG_aVR:
    case WAVE_ECG_aVL:
    case WAVE_ECG_aVF:
    case WAVE_ECG_V1:
    case WAVE_ECG_V2:
    case WAVE_ECG_V3:
    case WAVE_ECG_V4:
    case WAVE_ECG_V5:
    case WAVE_ECG_V6:
        paramID = PARAM_ECG;
        break;
    case WAVE_RESP:
        paramID = PARAM_RESP;
        break;
    case WAVE_SPO2:
    case WAVE_SPO2_2:
        paramID = PARAM_SPO2;
        break;
    case WAVE_CO2:
        paramID = PARAM_CO2;
        break;
    case WAVE_N2O:
    case WAVE_AA1:
    case WAVE_AA2:
    case WAVE_O2:
        paramID = PARAM_AG;
        break;
    case WAVE_ART:
    case WAVE_PA:
    case WAVE_CVP:
    case WAVE_LAP:
    case WAVE_RAP:
    case WAVE_ICP:
    case WAVE_AUXP1:
    case WAVE_AUXP2:
        paramID = PARAM_IBP;
        break;
    default:
        break;
    }
    return paramID;
}

/**************************************************************************************************
 * 功能： 获取参数存放趋势数据时的放大倍数。
 * 参数：
 *      id： 子参数ID。
 *************************************************************************************************/
int ParamInfo::getMultiOfSubParam(SubParamID id)
{
    if ((id >= SUB_PARAM_ETCO2 && id <= SUB_PARAM_ST_V6) ||
            id == SUB_PARAM_SPHB || id == SUB_PARAM_SPMET)
    {
        return 10;
    }
    else if (id == SUB_PARAM_PI || id == SUB_PARAM_PLUGIN_PI)
    {
        return 100;
    }

    return 1;
}

/**************************************************************************************************
 * 功能： 获取参数存放趋势数据时的放大倍数。
 * 参数：
 *      name: 子参数名字。
 * 返回：放大倍数。
 *************************************************************************************************/
int ParamInfo::getMultiOfSubParam(const char *name)
{
    QString subName(name);
    if (subName.indexOf("ST") != -1)
    {
        return 100;
    }

    if ((subName.indexOf("T1") != -1) || (subName.indexOf("T2") != -1) ||
            (subName.indexOf("TD") != -1))
    {
        return 100;
    }

    return 0;
}

/**************************************************************************************************
 * 功能： 获取子参数的默认单位。
 * 参数：
 *      id：之参数ID。
 * 返回： 默认单位。
 *************************************************************************************************/
UnitType ParamInfo::getUnitOfSubParam(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_HR_PR:
    case SUB_PARAM_PLUGIN_PR:
    case SUB_PARAM_ECG_PVCS:
    case SUB_PARAM_NIBP_PR:
    case SUB_PARAM_ART_PR:
    case SUB_PARAM_PA_PR:
    case SUB_PARAM_CVP_PR:
    case SUB_PARAM_LAP_PR:
    case SUB_PARAM_RAP_PR:
    case SUB_PARAM_ICP_PR:
    case SUB_PARAM_AUXP1_PR:
    case SUB_PARAM_AUXP2_PR:
    case SUB_PARAM_AWRR:
        return UNIT_BPM;

    case SUB_PARAM_ST_I:
    case SUB_PARAM_ST_II:
    case SUB_PARAM_ST_III:
    case SUB_PARAM_ST_aVR:
    case SUB_PARAM_ST_aVL:
    case SUB_PARAM_ST_aVF:
    case SUB_PARAM_ST_V1:
    case SUB_PARAM_ST_V2:
    case SUB_PARAM_ST_V3:
    case SUB_PARAM_ST_V4:
    case SUB_PARAM_ST_V5:
    case SUB_PARAM_ST_V6:
        return UNIT_MV;
    case SUB_PARAM_RR_BR:
        return UNIT_RPM;

    case SUB_PARAM_SPO2:
    case SUB_PARAM_SPO2_2:
    case SUB_PARAM_SPO2_D:
    case SUB_PARAM_PI:
    case SUB_PARAM_PLUGIN_PI:
    case SUB_PARAM_PVI:
    case SUB_PARAM_SPMET:
    case SUB_PARAM_SPCO:
        return UNIT_PERCENT;
    case SUB_PARAM_SPHB:
        return UNIT_GDL;
    case SUB_PARAM_SPOC:
        return UNIT_MLDL;

    case SUB_PARAM_NIBP_SYS:
    case SUB_PARAM_NIBP_DIA:
    case SUB_PARAM_NIBP_MAP:
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_ART_DIA:
    case SUB_PARAM_ART_MAP:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_PA_DIA:
    case SUB_PARAM_PA_MAP:
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_RAP_MAP:
    case SUB_PARAM_ICP_MAP:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP1_DIA:
    case SUB_PARAM_AUXP1_MAP:
    case SUB_PARAM_AUXP2_SYS:
    case SUB_PARAM_AUXP2_DIA:
    case SUB_PARAM_AUXP2_MAP:
        return UNIT_MMHG;

    case SUB_PARAM_ETCO2:
    case SUB_PARAM_FICO2:
    case SUB_PARAM_ETN2O:
    case SUB_PARAM_FIN2O:
    case SUB_PARAM_ETAA1:
    case SUB_PARAM_FIAA1:
    case SUB_PARAM_ETAA2:
    case SUB_PARAM_FIAA2:
    case SUB_PARAM_ETO2:
    case SUB_PARAM_FIO2:
    case SUB_PARAM_O2:
        return UNIT_PERCENT;

    case SUB_PARAM_CO_CO:
        return UNIT_LPM;
    case SUB_PARAM_CO_CI:
        return UNIT_LPMPSQM;
    case SUB_PARAM_CO_TB:
        return UNIT_TC;

    case SUB_PARAM_T1:
    case SUB_PARAM_T2:
        return UNIT_TC;
    case SUB_PARAM_TD:
        return UNIT_TDC;

    default:
        break;
    }

    return UNIT_NONE;
}

/**************************************************************************************************
 * 功能： 查询子参数的默认单位。
 * 参数：
 *      id：子参数ID。
 *      t0：可选单位。
 *      t1：可选单位。
 * 返回： 默认单位。
 *************************************************************************************************/
UnitType ParamInfo::getUnitOfSubParam(SubParamID id, UnitType *t0, UnitType *t1)
{
    *t0 = UNIT_NONE;
    *t1 = UNIT_NONE;

    switch (id)
    {
    case SUB_PARAM_HR_PR:
    case SUB_PARAM_PLUGIN_PR:
    case SUB_PARAM_ECG_PVCS:
    case SUB_PARAM_ART_PR:
    case SUB_PARAM_PA_PR:
    case SUB_PARAM_CVP_PR:
    case SUB_PARAM_LAP_PR:
    case SUB_PARAM_RAP_PR:
    case SUB_PARAM_ICP_PR:
    case SUB_PARAM_AUXP1_PR:
    case SUB_PARAM_AUXP2_PR:
    case SUB_PARAM_AWRR:
        return UNIT_BPM;

    case SUB_PARAM_ST_I:
    case SUB_PARAM_ST_II:
    case SUB_PARAM_ST_III:
    case SUB_PARAM_ST_aVR:
    case SUB_PARAM_ST_aVL:
    case SUB_PARAM_ST_aVF:
    case SUB_PARAM_ST_V1:
    case SUB_PARAM_ST_V2:
    case SUB_PARAM_ST_V3:
    case SUB_PARAM_ST_V4:
    case SUB_PARAM_ST_V5:
    case SUB_PARAM_ST_V6:
        return UNIT_MV;
    case SUB_PARAM_RR_BR:
        return UNIT_RPM;

    case SUB_PARAM_SPO2:
    case SUB_PARAM_SPO2_2:
    case SUB_PARAM_SPO2_D:
    case SUB_PARAM_PI:
    case SUB_PARAM_PLUGIN_PI:
    case SUB_PARAM_PVI:
    case SUB_PARAM_SPMET:
    case SUB_PARAM_SPCO:
        return UNIT_PERCENT;
    case SUB_PARAM_SPOC:
        return UNIT_MLDL;

    case SUB_PARAM_SPHB:
        *t0 = UNIT_MMOL_L;
        return UNIT_GDL;

    case SUB_PARAM_NIBP_SYS:
    case SUB_PARAM_NIBP_DIA:
    case SUB_PARAM_NIBP_MAP:
        *t0 = UNIT_KPA;
        return UNIT_MMHG;

    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_ART_DIA:
    case SUB_PARAM_ART_MAP:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_PA_DIA:
    case SUB_PARAM_PA_MAP:
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_RAP_MAP:
    case SUB_PARAM_ICP_MAP:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP1_DIA:
    case SUB_PARAM_AUXP1_MAP:
    case SUB_PARAM_AUXP2_SYS:
    case SUB_PARAM_AUXP2_DIA:
    case SUB_PARAM_AUXP2_MAP:
        *t0 = UNIT_KPA;
        *t1 = UNIT_CMH2O;
        return UNIT_MMHG;

    case SUB_PARAM_ETCO2:
    case SUB_PARAM_FICO2:
        *t0 = UNIT_MMHG;
        *t1 = UNIT_KPA;
        return UNIT_PERCENT;

    case SUB_PARAM_CO_CO:
        return UNIT_LPM;
    case SUB_PARAM_CO_CI:
        return UNIT_LPMPSQM;
    case SUB_PARAM_CO_TB:
        *t0 = UNIT_TF;
        return UNIT_TC;

    case SUB_PARAM_T1:
    case SUB_PARAM_T2:
        *t0 = UNIT_TF;
        return UNIT_TC;

    case SUB_PARAM_TD:
        *t0 = UNIT_TDF;
        return UNIT_TDC;

    case SUB_PARAM_O2:
        return UNIT_PERCENT;
    default:
        break;
    }

    return UNIT_NONE;
}


static bool subParamIDLessThan(unsigned int a, unsigned int b)
{
    int rank[SUB_PARAM_NR] =
    {
        1,      // HR/PR
        2,      // SPO2
        4,      // RR/BR
        3,      // NIBP_SYS
        3,      // NIBP_DIA
        3,      // NIBP_MAP
        5,      // ETCO2
        6,      // FICO2
        7,      // T1
        8,      // T2
        9,      // TD
    };

    if (a >= SUB_PARAM_NR || b >= SUB_PARAM_NR)
    {
        qdebug("Invalid SubParamID");
        return a < b;
    }

    return rank[a] < rank[b];
}

/***************************************************************************************************
 * sort the sub param in a list
 **************************************************************************************************/
void ParamInfo::sortSubParamId(QList<SubParamID> *subIDlist)
{
    qSort(subIDlist->begin(), subIDlist->end(), subParamIDLessThan);
}

/**************************************************************************************************
 * 功能： 构造。
 *************************************************************************************************/
ParamInfo::ParamInfo()
{
}

/**************************************************************************************************
 * 功能： 析构。
 *************************************************************************************************/
ParamInfo &ParamInfo::getInstance()
{
    static ParamInfo *instance = NULL;
    if (instance == NULL)
    {
        instance = new ParamInfo;
        ParamInfoInterface *old = registerParamInfo(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

ParamInfo::~ParamInfo()
{
}
