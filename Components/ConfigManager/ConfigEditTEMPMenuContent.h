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

#include "Framework/UI/MenuContent.h"
class ConfigEditTEMPMenuContentPrivate;
class Config;
class ConfigEditTEMPMenuContent : public MenuContent
{
    Q_OBJECT
public:
    explicit ConfigEditTEMPMenuContent(Config *const config);
    ~ConfigEditTEMPMenuContent();
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
    ConfigEditTEMPMenuContentPrivate *const d_ptr;
};

