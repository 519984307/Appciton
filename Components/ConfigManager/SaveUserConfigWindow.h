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
#include "Framework/UI/Dialog.h"

class SaveUserConfigWindowPrivate;
class SaveUserConfigWindow : public Dialog
{
    Q_OBJECT
public:
    SaveUserConfigWindow();
    ~SaveUserConfigWindow();

protected:
    /*reimplement*/
    void showEvent(QShowEvent *);

private slots:
    void onBtnReleased();
    void onConfigNameBtnReleased();

private:
    SaveUserConfigWindowPrivate *const d_ptr;
};
