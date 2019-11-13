/**
** This file is part of the nPM project.
** Copyright (C) Better Life Medical Technology Co., Ltd.
** All Rights Reserved.
** Unauthorized copying of this file, via any medium is strictly prohibited
** Proprietary and confidential
**
** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/16
**/
#pragma once

#include "Framework/UI/MenuContent.h"
#include "ConfigEditMenuWindow.h"

class ConfigEditCO2MenuContentPrivate;
class ConfigEditCO2MenuContent: public MenuContent
{
    Q_OBJECT
public:
    explicit ConfigEditCO2MenuContent(Config *const config);
    ~ConfigEditCO2MenuContent();

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

private:
    ConfigEditCO2MenuContentPrivate *const d_ptr;
};
