/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/9
 **/

#pragma once
#include "Framework/UI/MenuContent.h"
#include <QScopedPointer>

class ErrorLogEntranceContentPrivate;
class ErrorLogEntranceContent : public MenuContent
{
    Q_OBJECT
public:
    ErrorLogEntranceContent();
    ~ErrorLogEntranceContent();

protected:
    /* reimplement */
    void layoutExec();

private slots:
    void onButtonReleased(void);

private:
    QScopedPointer<ErrorLogEntranceContentPrivate> d_ptr;
};
