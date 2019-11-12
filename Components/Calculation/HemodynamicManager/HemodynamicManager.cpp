/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/12
 **/

#include "HemodynamicManager.h"
#include <qmath.h>
#include <QDateTime>
#include "Framework/TimeDate/TimeDate.h"
#define MAX_CALC_DATA   10

HemodynamicManager *HemodynamicManager::_selfObj = NULL;

/**************************************************************************************************
 * 功能： 析构函数。
 *************************************************************************************************/
HemodynamicManager::~HemodynamicManager()
{
}

/**************************************************************************************************
 * 功能： 获取计算相关信息(输入参数，计算结果，计算时间)。
 *************************************************************************************************/
HemodynamicInfo HemodynamicManager::getCalcValue()
{
    return _hemodynamicInfo;
}

/**************************************************************************************************
 * 功能： 设置计算相关信息(输入参数，计算结果，计算时间)。
 *************************************************************************************************/
void HemodynamicManager::setCalcValue(HemodynamicInfo info)
{
    _hemodynamicInfo = info;
}

/**************************************************************************************************
 * 功能： 获取计算回顾数据。
 *************************************************************************************************/
QList<HemodynamicInfo> HemodynamicManager::getReviewData()
{
    return _reviewData;
}

/**************************************************************************************************
 * 功能： 设置计算输入参数值。
 *************************************************************************************************/
void HemodynamicManager::setCalcInput(int index, float value)
{
    _hemodynamicInfo.calcInput[index] = value;
}

/**************************************************************************************************
 * 功能： 设置计算结果输出值。
 *************************************************************************************************/
void HemodynamicManager::setCalcOutput(int index, float value)
{
    _hemodynamicInfo.calcOutput[index] = value;
}

/**************************************************************************************************
 * 功能： 血液动力学计算。
 *************************************************************************************************/
void HemodynamicManager::calcAction()
{
    // BSA
    if (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_WEIGHT] == InvData() ||
            _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HEIGHT] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_BSA] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_BSA] =
                qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_WEIGHT], 0.425) *
                qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HEIGHT], 0.725) *
                0.007184;
    }

    // C.I.
    if (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] == InvData() ||
            _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_BSA] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_CI] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_CI] =
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] /
                    (qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_WEIGHT], 0.425) *
                    qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HEIGHT], 0.725) *
                    0.007184);
    }

    // SV
    if (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] == InvData() ||
            _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HR] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_SV] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_SV] =
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] /
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HR] *
                    1000;
    }

    // SVI
    if (_hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_SV] == InvData() ||
            _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_BSA] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_SVI] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_SVI] =
                    (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] /
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HR] *
                    1000) /
                    (qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_WEIGHT], 0.425) *
                    qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HEIGHT], 0.725) *
                    0.007184);
    }

    // SVR
    if (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] == InvData() ||
            _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_ARTMEAN] == InvData() ||
            _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CVP] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_SVR] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_SVR] =
                    79.96 *
                    (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_ARTMEAN] -
                     _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CVP]) /
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO];
    }

    // SVRI
    if (_hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_BSA] ==  InvData()||
            _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_SVR] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_SVRI] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_SVRI] =
                    (79.96 *
                    (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_ARTMEAN] -
                     _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CVP]) /
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO]) *
                    (qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_WEIGHT], 0.425) *
                    qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HEIGHT], 0.725) *
                    0.007184);
    }

    // PVR
    if (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] == InvData() ||
            _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_PAMEAN] == InvData() ||
            _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_PAWP] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_PVR] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_PVR] =
                    79.96 *
                    (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_PAMEAN] -
                     _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_PAWP]) /
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO];
    }

    // PVRI
    if (_hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_BSA] == InvData() ||
            _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_PVR] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_PVRI] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_PVRI] =
                    (79.96 *
                    (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_PAMEAN] -
                     _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_PAWP]) /
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO]) *
                    (qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_WEIGHT], 0.425) *
                    qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HEIGHT], 0.725) *
                    0.007184);
    }

    // LCW
    if (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_ARTMEAN] == InvData() ||
            _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_LCW] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_LCW] =
                    0.0136 *
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_ARTMEAN] *
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO];
    }

    // LCWI
    if (_hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_LCW] == InvData() ||
            _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_BSA] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_LCWI] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_LCWI] =
                    (0.0136 *
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_ARTMEAN] *
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO]) /
                    (qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_WEIGHT], 0.425) *
                    qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HEIGHT], 0.725) *
                    0.007184);
    }

    // LVSW
    if (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_ARTMEAN] == InvData() ||
            _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_SV] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_LVSW] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_LVSW] =
                    0.0136 *
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_ARTMEAN] *
                    (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] /
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HR] *
                    1000);
    }

    // LVSWI
    if (_hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_LVSW] == InvData() ||
            _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_BSA] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_LVSWI] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_LVSWI] =
                    (0.0136 *
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_ARTMEAN] *
                    (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] /
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HR] *
                    1000)) /
                    (qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_WEIGHT], 0.425) *
                    qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HEIGHT], 0.725) *
                    0.007184);
    }

    // RCW
    if (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_PAMEAN] == InvData() ||
            _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_RCW] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_RCW] =
                    0.0136 *
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_PAMEAN] *
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO];
    }

    // RCWI
    if (_hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_RCW] == InvData() ||
            _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_BSA] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_RCWI] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_RCWI] =
                    (0.0136 *
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_PAMEAN] *
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO]) /
                    (qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_WEIGHT], 0.425) *
                    qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HEIGHT], 0.725) *
                    0.007184);
    }

    // RVSW
    if (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_PAMEAN] == InvData() ||
            _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_SV] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_RVSW] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_RVSW] =
                    0.0136 *
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_PAMEAN] *
                    (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] /
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HR] *
                    1000);
    }

    // RVSWI
    if (_hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_RVSW] == InvData() ||
            _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_BSA] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_RVSWI] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_RVSWI] =
                    (0.0136 *
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_PAMEAN] *
                    (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] /
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HR] *
                    1000)) /
                    (qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_WEIGHT], 0.425) *
                    qPow(_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HEIGHT], 0.725) *
                    0.007184);
    }

    // EF
    if (_hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_SV] == InvData() ||
            _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_EDV] == InvData())
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_EF] = InvData();
    }
    else
    {
        _hemodynamicInfo.calcOutput[HEMODYNAMIC_OUTPUT_EF] =
                    100 *
                    (_hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_CO] /
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_HR] *
                    1000) /
                    _hemodynamicInfo.calcInput[HEMODYNAMIC_PARAM_EDV];
    }


    // 获取当前计算的时间戳
    _hemodynamicInfo.calcTime = timeDate->time();
}

/**************************************************************************************************
 * 功能： 向计算回顾插入数据。
 *************************************************************************************************/
void HemodynamicManager::insertReviewData(HemodynamicInfo data)
{
    if (_reviewData.length() == MAX_CALC_DATA)
    {
        _reviewData.removeFirst();
    }
    _reviewData.append(data);
}

/**************************************************************************************************
 * 功能： 输出结果上下限。
 *************************************************************************************************/
ParamLimitInfo HemodynamicManager::outputLimit[HEMODYNAMIC_OUTPUT_NR] =
{
    {2.5, 4},
    {60, 130},
    {770, 1500},
    {100, 250},
    {5.4, 10},
    {},
    {},
    {},
    {},
    {},
    {35, 65},
    {1680, 2580},
    {225, 315},
    {3, 5.5},
    {47.2, 70.8},
    {0.54, 0.66},
    {7.9, 9.7}
};

/**************************************************************************************************
 * 功能： 输入参数上下限。
 *************************************************************************************************/
ParamLimitInfo HemodynamicManager::inputLimit[HEMODYNAMIC_PARAM_NR] =
{
    {0, 20},
    {0, 300},
    {0, 40},
    {0, 300},
    {0, 120},
    {0, 40},
    {1, 300},
    {20, 300},
    {0.1, 200}
};

/**************************************************************************************************
 * 功能： 输出结果小数点后精度值。
 *************************************************************************************************/
int HemodynamicManager::outputAccuracy[HEMODYNAMIC_OUTPUT_NR] =
{
    1, 1, 0, 0, 1, 1, 2, 2, 2, 3, 1, 0, 0, 1, 1, 2, 2
};

/**************************************************************************************************
 * 功能： 输入参数小数点后精度值。
 *************************************************************************************************/
int HemodynamicManager::inputAccuracy[HEMODYNAMIC_PARAM_NR] =
{
    1, 0, 0, 0, 0, 0, 0, 1, 1
};

/**************************************************************************************************
 * 功能： 构造函数。
 *************************************************************************************************/
HemodynamicManager::HemodynamicManager()
{
    for (int i = 0; i < HEMODYNAMIC_OUTPUT_NR; i ++)
    {
        _hemodynamicInfo.calcOutput[i] = InvData();
    }
}
