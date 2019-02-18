/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/28
 **/

#pragma once
#include "Dialog.h"

class ScreenLayoutWindowPrivate;
class ScreenLayoutWindow : public Dialog
{
    Q_OBJECT
public:
    /**
     * @brief getInstance get the window instance
     * @return
     */
    static ScreenLayoutWindow *getInstance();
    ~ScreenLayoutWindow();

protected:
    /*reimplement*/
    void hideEvent(QHideEvent *ev);

    /*reimplement*/
    void showEvent(QShowEvent *);

private slots:
    /**
     * @brief onButtonClicked handle the button click signal
     */
    void onButtonClicked();

private:
    ScreenLayoutWindow();
    ScreenLayoutWindowPrivate * const d_ptr;
};
