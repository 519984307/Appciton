/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/18
 **/

#pragma once
#include "Framework/UI/MenuContent.h"

class UnitSetupMenuContentPrivate;
class UnitSetupMenuContent : public MenuContent
{
    Q_OBJECT
public:
    UnitSetupMenuContent();
    ~UnitSetupMenuContent();

protected:
    virtual void readyShow();

    virtual void layoutExec();
private slots:
    void onComboBoxIndexChanged(int index);

private:
    UnitSetupMenuContentPrivate * const d_ptr;
};

