/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/14
 **/

#pragma once
#include "MenuContent.h"

class O2CalibrationMenuContentPrivate;
class O2CalibrationMenuContent : public MenuContent
{
    Q_OBJECT
public:
    O2CalibrationMenuContent();
    ~O2CalibrationMenuContent();

protected:
    virtual void readyShow();

    virtual void layoutExec();

    void timerEvent(QTimerEvent *ev);
private slots:
    void percent21Released(int);

    void percent100Released(int);

private:
    O2CalibrationMenuContentPrivate *const d_ptr;
};
