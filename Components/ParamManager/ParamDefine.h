/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#pragma once
#include <QMap>
#include <QVector>

enum ParamID
{
    PARAM_NONE = -1,

    PARAM_ECG,
    PARAM_SPO2,
    PARAM_RESP,
    PARAM_NIBP,
    PARAM_CO2,
    PARAM_TEMP,
    PARAM_DUP_ECG,
    PARAM_DUP_RESP,

    PARAM_IBP,
    PARAM_AG,
    PARAM_CO,
    PARAM_O2,

    PARAM_UPGRADE,

    PARAM_NR,
};

enum SubParamID
{
    SUB_PARAM_NONE = -1,

    SUB_PARAM_HR_PR,
    SUB_PARAM_ECG_PVCS,
    // spo2
    SUB_PARAM_SPO2,
    SUB_PARAM_SPO2_2,
    SUB_PARAM_SPO2_D,
    SUB_PARAM_PI,
    SUB_PARAM_PLUGIN_PI,
    SUB_PARAM_PVI,
    SUB_PARAM_SPHB,
    SUB_PARAM_SPOC,
    SUB_PARAM_SPMET,
    SUB_PARAM_SPCO,

    SUB_PARAM_RR_BR,
    SUB_PARAM_NIBP_SYS,
    SUB_PARAM_NIBP_DIA,
    SUB_PARAM_NIBP_MAP,
    SUB_PARAM_NIBP_PR,

    // IBP
    SUB_PARAM_ART_SYS,
    SUB_PARAM_ART_DIA,
    SUB_PARAM_ART_MAP,
    SUB_PARAM_ART_PR,
    SUB_PARAM_PA_SYS,
    SUB_PARAM_PA_DIA,
    SUB_PARAM_PA_MAP,
    SUB_PARAM_PA_PR,
    SUB_PARAM_CVP_MAP,
    SUB_PARAM_CVP_PR,
    SUB_PARAM_LAP_MAP,
    SUB_PARAM_LAP_PR,
    SUB_PARAM_RAP_MAP,
    SUB_PARAM_RAP_PR,
    SUB_PARAM_ICP_MAP,
    SUB_PARAM_ICP_PR,
    SUB_PARAM_AUXP1_SYS,
    SUB_PARAM_AUXP1_DIA,
    SUB_PARAM_AUXP1_MAP,
    SUB_PARAM_AUXP1_PR,
    SUB_PARAM_AUXP2_SYS,
    SUB_PARAM_AUXP2_DIA,
    SUB_PARAM_AUXP2_MAP,
    SUB_PARAM_AUXP2_PR,

    // CO
    SUB_PARAM_CO_CO,
    SUB_PARAM_CO_CI,
    SUB_PARAM_CO_TB,

    // AG
    SUB_PARAM_ETCO2,
    SUB_PARAM_FICO2,
    SUB_PARAM_ETN2O,
    SUB_PARAM_FIN2O,
    SUB_PARAM_ETAA1,
    SUB_PARAM_FIAA1,
    SUB_PARAM_ETAA2,
    SUB_PARAM_FIAA2,
    SUB_PARAM_ETO2,
    SUB_PARAM_FIO2,

    SUB_PARAM_T1,
    SUB_PARAM_T2,
    SUB_PARAM_TD,

    SUB_PARAM_ST_I,
    SUB_PARAM_ST_II,
    SUB_PARAM_ST_III,
    SUB_PARAM_ST_aVR,
    SUB_PARAM_ST_aVL,
    SUB_PARAM_ST_aVF,
    SUB_PARAM_ST_V1,
    SUB_PARAM_ST_V2,
    SUB_PARAM_ST_V3,
    SUB_PARAM_ST_V4,
    SUB_PARAM_ST_V5,
    SUB_PARAM_ST_V6,

    SUB_PARAM_O2,

    SUB_PARAM_NR,
};

// 该ID用于处理HR/PR，RR/BR复用的情况。
enum SubDupParamID
{
    SUB_DUP_PARAM_NONE = -1,

    SUB_DUP_PARAM_HR,
    SUB_DUP_PARAM_PR,
    SUB_DUP_PARAM_RR,
    SUB_DUP_PARAM_BR,

    SUB_DUP_PARAM_NR,
};

enum WaveformID
{
    WAVE_NONE = -1,

    WAVE_ECG_I,
    WAVE_ECG_II,
    WAVE_ECG_III,
    WAVE_ECG_aVR,
    WAVE_ECG_aVL,
    WAVE_ECG_aVF,

    WAVE_ECG_V1,
    WAVE_ECG_V2,
    WAVE_ECG_V3,
    WAVE_ECG_V4,
    WAVE_ECG_V5,
    WAVE_ECG_V6,

    WAVE_RESP,
    WAVE_SPO2,
    WAVE_SPO2_2,

    WAVE_CO2,
    WAVE_N2O,
    WAVE_AA1,
    WAVE_AA2,
    WAVE_O2,

    WAVE_ART,
    WAVE_PA,
    WAVE_CVP,
    WAVE_LAP,
    WAVE_RAP,
    WAVE_ICP,
    WAVE_AUXP1,
    WAVE_AUXP2,

    WAVE_NR
};
