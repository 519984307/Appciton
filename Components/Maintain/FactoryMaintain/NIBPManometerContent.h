/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2019/7/24
 **/

#pragma once
#include "MenuContent.h"

#define CALIBRATION_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER                    (5000 / CALIBRATION_INTERVAL_TIME)

class NIBPManometerContentPrivate;
class NIBPManometerContent : public MenuContent
{
    Q_OBJECT

public:
    static NIBPManometerContent *getInstance();
    ~NIBPManometerContent();

    void init(void);

protected:
    virtual void layoutExec(void);
    void timerEvent(QTimerEvent *ev);
    void hideEvent(QHideEvent *e);

private slots:
    void enterManometerReleased(void);

private:
    NIBPManometerContent();

    NIBPManometerContentPrivate *const d_ptr;
};
