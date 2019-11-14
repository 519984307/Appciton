/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/6
 **/

#pragma once
#include "Framework/UI/Dialog.h"

class CalculateWindowPrivate;
class CalculateWindow : public Dialog
{
    Q_OBJECT
public:
    /**
     * @brief getInstance
     * @return
     */
    static CalculateWindow *getInstance(void);

    ~CalculateWindow();

    /**
     * @brief layoutExec
     */
    void layoutExec();

private slots:
    /**
     * @brief onBtnReleased
     */
    void onBtnReleased(void);

private:
    CalculateWindow();
    CalculateWindowPrivate *d_ptr;
};
