/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/18
 **/
#pragma once
#include "MenuContent.h"
#include "ConfigEditMenuWindow.h"

class ConfigEditSpO2MenuContentPrivate;
class ConfigEditSpO2MenuContent : public MenuContent
{
    Q_OBJECT
public:
    explicit ConfigEditSpO2MenuContent(Config *const config);
    ~ConfigEditSpO2MenuContent();

protected:
    virtual void readyShow();
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
     * @brief onPopupListItemFocusChanged
     * @param volume
     */
    void onPopupListItemFocusChanged(int volume);

private:
    ConfigEditSpO2MenuContentPrivate *const d_ptr;
};
