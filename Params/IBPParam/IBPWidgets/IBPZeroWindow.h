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
#include "Framework/UI/Dialog.h"
#include <QScopedPointer>

class IBPZeroWindowPrivate;
class IBPZeroWindow : public Dialog
{
    Q_OBJECT
public:
    IBPZeroWindow();
    ~IBPZeroWindow();

protected:
    /* reimplement */
    void timerEvent(QTimerEvent *ev);

private slots:
    /**
     * @brief startZero start zero
     */
    void startZero();

private:
    const QScopedPointer<IBPZeroWindowPrivate> pimpl;
};
