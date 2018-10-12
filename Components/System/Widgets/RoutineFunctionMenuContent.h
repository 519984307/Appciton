/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/12
 **/

#pragma once
#include "MenuContent.h"

class RoutineFunctionMenuContentPrivate;
class RoutineFunctionMenuContent : public MenuContent
{
    Q_OBJECT
public:
    RoutineFunctionMenuContent();
    ~RoutineFunctionMenuContent();

protected:
    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    /**
     * @brief onComboBoxIndexChanged
     * @param index
     */
    void onComboBoxIndexChanged(int index);
    /**
     * @brief onBtnReleasd
     */
    void onBtnReleasd(void);

private:
    RoutineFunctionMenuContentPrivate * const d_ptr;
};
