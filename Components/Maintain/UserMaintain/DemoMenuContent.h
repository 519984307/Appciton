/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#pragma once
#include "Framework/UI/MenuContent.h"

class DemoMenuContent : public MenuContent
{
    Q_OBJECT
public:
    DemoMenuContent();

protected:
    /* reimplement */
    void layoutExec();

private slots:
    void onButtonClicked(void);
};
