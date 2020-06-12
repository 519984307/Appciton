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

    /**
     * @brief setMeasureResult
     * @param co cardiac output
     * #param ci cardiac index
     */
    void setMeasureResult(short co, short ci);

    /**
     * @brief setTb set the curent Tb
     * @param tb blood temperature
     * @note
     * Only set the tb value when  in measurement
     */
    void setTb(short tb);

    /**
     * @brief addMeasureWaveData add measure wave data
     * @param wave
     */
    void addMeasureWaveData(short wave);


protected:
    /* override */
    void showEvent(QShowEvent *ev);
    /* override */
    void timerEvent(QTimerEvent *ev);

private slots:
    /* handle the button clicked signal */
    void btnClicked();
    /* handle the measure result checked signal */
    void onResultChecked();
    /* handle the measure result checked signal */
    void onWorkModeChanged();

private:
    const QScopedPointer<COMeasureWindowPrivate> pimpl;
};
