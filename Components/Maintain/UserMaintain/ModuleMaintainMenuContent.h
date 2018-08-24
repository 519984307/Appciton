/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/16
 **/

#pragma once
#include "MenuContent.h"

class ModuleMaintainMenuContentPrivate;
class ModuleMaintainMenuContent : public MenuContent
{
    Q_OBJECT
public:
    ModuleMaintainMenuContent();
    ~ModuleMaintainMenuContent();

protected:
    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    void onButtonReleased(void);

private:
    ModuleMaintainMenuContentPrivate * const d_ptr;
};
