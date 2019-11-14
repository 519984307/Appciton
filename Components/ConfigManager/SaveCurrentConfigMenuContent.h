/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/10/26
 **/

#pragma once
#include "Framework/UI/MenuContent.h"
#include <QScopedPointer>

class SaveCurrentConfigMenuContentPrivate;
class SaveCurrentConfigMenuContent : public MenuContent
{
    Q_OBJECT
public:
    SaveCurrentConfigMenuContent();
    ~SaveCurrentConfigMenuContent();

protected:
    /*reimplement*/
    void layoutExec();

private slots:
    void onBtnReleased();

private:
    const QScopedPointer<SaveCurrentConfigMenuContentPrivate> d_ptr;
};
