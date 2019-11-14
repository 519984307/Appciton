/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#pragma once
#include "Framework/UI/MenuContent.h"

class LoadConfigMenuContentPrivate;
class LoadConfigMenuContent: public MenuContent
{
    Q_OBJECT
public:
    /**
     * @brief LoadConfigMenuContent
     */
    LoadConfigMenuContent();
    ~LoadConfigMenuContent();

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
     * @brief onBtnClick
     */
    void onBtnClick();
    /**
     * @brief updateBtnStatus
     */
    void updateBtnStatus(void);
private:
    LoadConfigMenuContentPrivate * const d_ptr;
};
