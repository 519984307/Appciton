/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/8/12
 **/

#pragma once
#include "MenuContent.h"

class SPO2SeniorMenuContentPrivate;
class SPO2SeniorMenuContent : public MenuContent
{
    Q_OBJECT
public:
    SPO2SeniorMenuContent();
    ~SPO2SeniorMenuContent();

    /* reimplement */
    void layoutExec();

    /* reimplement */
    void readyShow();

private slots:
    void onComboIndexChanged(int index);

private:
    SPO2SeniorMenuContentPrivate * const d_ptr;
};
