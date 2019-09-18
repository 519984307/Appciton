/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/26
 **/

#pragma once
#include "MenuContent.h"

class ApneaStimulationMenuContentPrivate;
class ApneaStimulationMenuContent : public MenuContent
{
    Q_OBJECT
public:
    ApneaStimulationMenuContent();
    ~ApneaStimulationMenuContent();

    virtual void layoutExec();

    virtual void readyShow();
private slots:
    void onComboBoxIndexChanged(int index);

    void onSpinBoxValueChanged(int value, int scale);

private:
    ApneaStimulationMenuContentPrivate *const d_ptr;
};
