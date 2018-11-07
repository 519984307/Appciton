/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/20
 **/



#pragma once


/**
 * @brief The OxyCRGInterval enum  时间间隔
 */
enum OxyCRGInterval
{
    OXYCRG_INTERVAL_1,      // x1
    OXYCRG_INTERVAL_2,      // x2
    OXYCRG_INTERVAL_4,      // x4
    OXYCRG_INTERVAL_8,      // x8
    OXYCRG_INTERVAL_NR
};

/**
 * @brief The OxyCRGTrend enum 呼吸波与RR
 */
enum OxyCRGWave
{
    OXYCRG_WAVE_RESP,
    OXYCRG_WAVE_CO2,
    OXYCRG_WAVE_NR
};

/**
 * @brief The TrendTypeOne enum  Trend1
 */
enum TrendTypeOne
{
    TREND_HR_PR = 0,
    TREND_RR,
    TRENDONE_NR,
};

/**
 * @brief The TrendTypeTwo enum  Trend2
 */
enum TrendTypeTwo
{
    TREND_SPO2 = 0,
    TRENDTWO_NR,
};

/**
 * @brief The HRLowTypes enum  HR low
 */
enum HRLowTypes
{
    HR_LOW_0 = 0,
    HR_LOW_40,
    HR_LOW_80,
    HR_LOW_120,
    HR_LOW_NR,
};

/**
 * @brief The HRHighTypes enum  HR high
 */
enum HRHighTypes
{
    HR_HIGH_160 = 0,
    HR_HIGH_200,
    HR_HIGH_240,
    HR_HIGH_280,
    HR_HIGH_320,
    HR_HIGH_360,
    HR_HIGH_NR,
};

/**
 * @brief The RRHighTypes enum  RR high
 */
enum RRHighTypes
{
    RR_HIGH_40 = 0,
    RR_HIGH_80,
    RR_HIGH_120,
    RR_HIGH_160,
    RR_HIGH_NR,
};

/**
 * @brief The RRLowTypes enum  RR low
 */
enum RRLowTypes
{
    RR_LOW_0 = 0,
    RR_LOW_40,
    RR_LOW_NR,
};

/**
 * @brief The SPO2HighTypes enum  SPO2 high
 */
enum SPO2HighTypes
{
    SPO2_HIGH_100 = 0,
    SPO2_HIGH_NR,
};


/**
 * @brief The SPO2LowTypes enum  SPO2 low
 */
enum SPO2LowTypes
{
    SPO2_LOW_60 = 0,
    SPO2_LOW_68,
    SPO2_LOW_76,
    SPO2_LOW_84,
    SPO2_LOW_92,
    SPO2_LOW_NR,
};


/**
 * @brief The CO2HighTypes enum  CO2 high
 */
enum CO2HighTypes
{
    CO2_HIGH_15 = 0,
    CO2_HIGH_20,
    CO2_HIGH_25,
    CO2_HIGH_40,
    CO2_HIGH_50,
    CO2_HIGH_60,
    CO2_HIGH_80,
    CO2_HIGH_NR
};

/**
 * @brief The CO2LowTypes enum  CO2 low
 */
enum CO2LowTypes
{
    CO2_LOW_0 = 0,
    CO2_LOW_NR,
};
