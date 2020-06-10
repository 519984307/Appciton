/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/10
 **/

#pragma once

#include "Framework/UI/Dialog.h"
#include <QScopedPointer>


class COMeasureWindowPrivate;
/**
 * @brief The COMeasureWindow class the CO measure window, support display the measurement
 * waveform and calculate the average cardiac output and cardiac index. We cound calculate
 * the average CO and CI with 6 measurement data at most.
 */
class COMeasureWindow : public Dialog
{
    Q_OBJECT
public:
    /**
     * @brief COMeasureWindow
     */
    COMeasureWindow();

    ~COMeasureWindow();

private:
    const QScopedPointer<COMeasureWindowPrivate> pimpl;
};
