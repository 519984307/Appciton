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
class TEMPMenuPrivate;
class TEMPMenu : public MenuContent
{
    Q_OBJECT
public:
    TEMPMenu();
    ~TEMPMenu();
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

private:
    TEMPMenuPrivate *const d_ptr;
};

