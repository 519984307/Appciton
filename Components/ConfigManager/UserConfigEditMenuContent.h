/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.14
 **/
#pragma once
#include "Framework/UI/MenuContent.h"

class UserConfigEditMenuContentPrivate;
class UserConfigEditMenuContent: public MenuContent
{
    Q_OBJECT
public:
    UserConfigEditMenuContent();
    ~UserConfigEditMenuContent();

protected:
    /**
     * @brief layoutExec
     */
    virtual void layoutExec(void);
    /**
     * @brief readyShow
     */
    virtual void readyShow(void);

private slots:
    /**
     * @brief onBtnClick
     */
    void onBtnClick();
    /**
     * @brief updateBtnStatus
     */
    void updateBtnStatus();

    /**
     * @brief onEditFinished
     */
    void onEditFinished();

private:
    UserConfigEditMenuContentPrivate *const d_ptr;
};
