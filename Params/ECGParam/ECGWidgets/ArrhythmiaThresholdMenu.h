/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/15
 **/

#pragma once
#include "MenuContent.h"
#include <QScopedPointer>

class ArrhythmiaThresholdMenuPrivate;
class ArrhythmiaThresholdMenu : public MenuContent
{
    Q_OBJECT
public:
    ArrhythmiaThresholdMenu();
    ~ArrhythmiaThresholdMenu();

    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    void onButtonReleased(void);
    void pauseChangeSlot(int);

private:
    QScopedPointer<ArrhythmiaThresholdMenuPrivate> d_ptr;
};
