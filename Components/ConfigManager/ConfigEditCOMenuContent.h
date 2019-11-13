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

class ConfigEditCOMenuContentPrivate;
class ConfigEditCOMenuContent : public MenuContent
{
    Q_OBJECT
public:
    explicit ConfigEditCOMenuContent(Config *const config);
    ~ConfigEditCOMenuContent();

protected:
    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    void onComboBoxIndexChanged(int index);
    void onButtonReleased(void);
    /**
     * @brief onAlarmBtnReleased
     */
    void onAlarmBtnReleased(void);

private:
    ConfigEditCOMenuContentPrivate *const d_ptr;
};
