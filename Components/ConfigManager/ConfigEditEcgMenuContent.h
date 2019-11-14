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

class ConfigEditECGMenuContentPrivate;
class ConfigEditECGMenuContent : public MenuContent
{
    Q_OBJECT
public:
    explicit ConfigEditECGMenuContent(Config * const config);
    ~ConfigEditECGMenuContent();

protected:
    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    void onComboBoxIndexChanged(int index);
    /**
     * @brief onSTSwitchBtnReleased  ST开关触发槽函数
     */
    void onSTSwitchBtnReleased(void);
    /**
     * @brief onAlarmBtnReleased
     */
    void onAlarmBtnReleased(void);

    void onPopupListItemFocusChanged(int volume);

private:
    ConfigEditECGMenuContentPrivate *const d_ptr;
};
