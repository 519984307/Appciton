/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/29
 **/

#pragma once
#include "Framework/UI/Dialog.h"


class NurseCallSetWindowPrivate;
class NurseCallSetWindow : public Dialog
{
    Q_OBJECT
public:
    NurseCallSetWindow();
    ~NurseCallSetWindow();

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void signalTypeComboBoxChange(int index);
    void contactTypeComboBoxChange(int index);
    void onBtnReleased(void);

private:
    NurseCallSetWindowPrivate *const d_ptr;
};
