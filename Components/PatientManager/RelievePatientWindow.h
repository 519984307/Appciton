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

class RelievePatientWindowPrivate;
class RelievePatientWindow : public Window
{
    Q_OBJECT

public:
    static RelievePatientWindow *getInstance();
    ~RelievePatientWindow();

    void layoutExec(void);

protected:
    void showEvent(QShowEvent *e);
    void exit(void);

private slots:
    void onYesReleased(void);
    void onNoReleased(void);

private:
    RelievePatientWindow();

    RelievePatientWindowPrivate *const d_ptr;
};
