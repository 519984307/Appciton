/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/5/13
 **/

#pragma once
#include "Framework/UI/MenuContent.h"

class IBPCalibrationMenuContentPrivate;
class IBPCalibrationMenuContent : public MenuContent
{
    Q_OBJECT
public:
    IBPCalibrationMenuContent();
    ~IBPCalibrationMenuContent();

protected:
    /* reimplement */
    void readyShow();

    /* reimplement */
    void layoutExec();

    /* reimplement */
    void timerEvent(QTimerEvent *ev);

private slots:
    /**
     * @brief startCalibrate start calibrate slot
     */
    void startCalibrate();

private:
    const QScopedPointer<IBPCalibrationMenuContentPrivate> pimpl;
};
