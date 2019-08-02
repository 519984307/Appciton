/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang fangtongzhou@blmed.cn, 2019/8/2
 **/
#pragma once
#include "MenuContent.h"

class FactoryCO2MenuContentPrivate;
class FactoryCO2MenuContent : public MenuContent
{
    Q_OBJECT

public:
    FactoryCO2MenuContent();

    ~FactoryCO2MenuContent();

    void showError(QString str);

protected:
    virtual void readyShow();

    virtual void layoutExec();

    void timerEvent(QTimerEvent *ev);

private slots:
    void onBtnReleased();

    void updateCO2Content(bool status);

private:
    FactoryCO2MenuContentPrivate *const d_ptr;
};
