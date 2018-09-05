/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/4
 **/
#pragma once

#include "Window.h"

class DemoModeWindowPrivate;
class DemoModeWindow : public Window
{
    Q_OBJECT
public:
    DemoModeWindow();
    ~DemoModeWindow();
    /**
     * @brief layoutExec
     */
    void layoutExec();
    /**
     * @brief isUserInputCorrect
     * @return
     */
    bool isUserInputCorrect(void) const;

private slots:
    void userInputCorrect(void);

private:
    DemoModeWindowPrivate * const d_ptr;
};


