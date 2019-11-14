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
#include "Framework/UI/MenuContent.h"
#include <QScopedPointer>

class ArrhythmiaAnalysisMenuPrivate;
class ArrhythmiaAnalysisMenu : public MenuContent
{
    Q_OBJECT
public:
    ArrhythmiaAnalysisMenu();
    ~ArrhythmiaAnalysisMenu();

protected:
    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:

private:
    QScopedPointer<ArrhythmiaAnalysisMenuPrivate> d_ptr;
};
