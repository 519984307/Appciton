/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#pragma once

#include "MenuContent.h"
#include "CO2Define.h"

class CO2MenuContentPrivate;
class CO2MenuContent: public MenuContent
{
    Q_OBJECT
public:
    CO2MenuContent();
    ~CO2MenuContent();

protected:
    /**
     * @brief readyShow
     */
    virtual void readyShow();
    /**
     * @brief layoutExec
     */
    virtual void layoutExec();
private slots:
    /**
     * @brief onComboBoxIndexChanged
     * @param index
     */
    void onComboBoxIndexChanged(int index);
    /**
     * @brief onBtnReleasedChanged
     */
    void onBtnReleasedChanged();
    /**
     * @brief onAlarmBtnReleased
     */
    void onAlarmBtnReleased(void);

    /**
     * @brief o2CompenSpinboxReleased
     * @param value
     * @param scale
     */
    void o2CompenSpinboxReleased(int value, int scale);

    /**
     * @brief n2oCompenSpinboxReleased
     * @param value
     * @param scale
     */
    void n2oCompenSpinboxReleased(int value, int scale);

    /**
     * @brief disableZero 失能CO2校零
     * @param sta
     */
    void disableZero(bool sta);

    /**
     * @brief onUpdateCompensation 更新气体可补偿状态
     * @param enable
     */
    void onUpdateCompensation(CO2Compensation gas, bool enable);

private:
    CO2MenuContentPrivate * const d_ptr;
};
