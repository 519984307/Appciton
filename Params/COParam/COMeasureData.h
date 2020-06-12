/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/11
 **/

#pragma once
#include <QVector>
#include "BaseDefine.h"

/**
 * @brief The COMeasureData struct record the result of single measurement
 */
struct COMeasureData
{
    COMeasureData()
        : timestamp(0), co(InvData()), ci(InvData()), dataRate(0) {}

    bool isValid() const { return co != InvData();}

    unsigned timestamp; /* measure timestamp */
    short co;   /* measure co value */
    short ci;   /* measure ci value */
    short dataRate; /* wave samples per second */
    QVector<short> measureWave;  /* TB wave data, the wave data should in unit of 0.01 celsius degree */
};
