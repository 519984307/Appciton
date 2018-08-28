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

#include "MenuContent.h"
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
     * @brief onBtnReleasedChanged
     */
    void onBtnReleasedChanged();
private:
    ConfigEditCO2MenuContentPrivate *const d_ptr;
};
