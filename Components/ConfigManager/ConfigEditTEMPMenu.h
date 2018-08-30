/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/24
 **/
#pragma once

#include "MenuContent.h"
class ConfigEditTEMPMenuPrivate;
class Config;
class ConfigEditTEMPMenu : public MenuContent
{
    Q_OBJECT
public:
    explicit ConfigEditTEMPMenu(Config *const config);
    ~ConfigEditTEMPMenu();
    /**
     * @brief layoutExec
     */
    virtual void layoutExec();
    /**
     * @brief readyShow
     */
    virtual void readyShow();
private slots:
    /**
     * @brief onComboIndexUpdated
     * @param index
     */
    void onComboIndexUpdated(int index);
    /**
     * @brief onAlarmBtnReleased
     */
    void onAlarmBtnReleased(void);

private:
    ConfigEditTEMPMenuPrivate *const d_ptr;
};

