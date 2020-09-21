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

#include <QWidget>
#include <QScopedPointer>
#include "COMeasureData.h"
#include "Framework/Utility/Unit.h"

class COMeasureWidgetPrivate;
class COMeasureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit COMeasureWidget(QWidget *parent = NULL);
    ~COMeasureWidget();

    /**
     * @brief startMeasure switch to measure state
     */
    void startMeasure();

    /**
     * @brief stopMeasure stop measure
     */
    void stopMeasure();

    /**
     * @brief addMeasureWave add the measure wave data
     * @param wave the wave data
     * @note
     * The wave data should in unit of 0.01 celsius degree
     */
    void addMeasureWave(short wave);

    /**
     * @brief clearMeasureWave  clear the measure wave data
     */
    void clearMeasureWave();

    /**
     * @brief setWaveDataRate set the sample rate of the wave data
     * @param rate
     */
    void setWaveDataRate(short rate);

    /**
     * @brief setTi set the injectate temp
     * @param ti
     * @param false : ti value is unchanged, no need to update;
     *        true : need to update;
     */
    void setTi(short ti, bool updateTi = false);

    /**
     * @brief setTb set the measure blood temp
     * @param tb
     * @param false : tb value is unchanged, no need to update;
     *        true : need to update;
     */
    void setTb(short tb, bool updateTb = false);

    /**
     * @brief setUnit  set Unit
     * @param unit  Unit type
     */
    void setUnit(UnitType unit);

    /**
     * @brief setCo set the measure cardiac output
     * @param co
     */
    void setCo(short co);

    /**
     * @brief setCi set the measure cardiac index
     * @param ci
     */
    void setCi(short ci);

    /**
     * @brief setMessage set the measure message
     * @param str message string
     */
    void setMessage(const QString &str);

    /**
     * @brief getMeasureData get the co measure data
     * @return the co measure data
     */
    COMeasureData getMeasureData() const;

protected:
    /* override */
    void paintEvent(QPaintEvent *ev);

private:
    const QScopedPointer<COMeasureWidgetPrivate> pimpl;
};
