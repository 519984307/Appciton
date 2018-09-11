/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/6
 **/

#pragma once
#include "MenuContent.h"

class CalculateMenuContentPrivate;
class CalculateMenuContent : public MenuContent
{
    Q_OBJECT
public:
    CalculateMenuContent();
    ~CalculateMenuContent();

protected:
    virtual void layoutExec();

private slots:
    void _btnReleased(void);

private:
    CalculateMenuContentPrivate *d_ptr;
};
