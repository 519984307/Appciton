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


/**************************************************************************************************
 * 时间间隔
 *************************************************************************************************/
enum OxyCRGInterval
{
    OxyCRG_Interval_1,      // x1
    OxyCRG_Interval_2,      // x2
    OxyCRG_Interval_4,      // x4
    OxyCRG_Interval_8,      // x8
    OxyCRG_Interval_NR
};

/**************************************************************************************************
 * 呼吸波与RR
 *************************************************************************************************/
enum OxyCRGTrend
{
    OxyCRG_Trend_RESP,
    OxyCRG_Trend_CO2,
    OxyCRG_Trend_NR
};
