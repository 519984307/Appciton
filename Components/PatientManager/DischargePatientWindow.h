/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/27
 **/
#pragma once

#include "Window.h"

class DischaregePatientWindowPrivate;
class DischargePatientWindow : public Window
{
    Q_OBJECT

public:
    DischargePatientWindow();
    ~DischargePatientWindow();

    void layoutExec(void);

protected:
    void showEvent(QShowEvent *e);
    void exit(void);

private slots:
    void onBtnRelease();

private:
    DischaregePatientWindowPrivate *const d_ptr;
};
