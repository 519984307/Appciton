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
#include "MenuContent.h"

class FactoryMaintainMenuContentPrivate;
class FactoryMaintainMenuContent : public MenuContent
{
    Q_OBJECT
public:
    FactoryMaintainMenuContent();
    ~FactoryMaintainMenuContent();

protected:
    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    void userInputCorrect(void);

private:
    FactoryMaintainMenuContentPrivate * const d_ptr;
};
