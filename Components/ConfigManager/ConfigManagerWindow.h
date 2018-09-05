/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/12
 **/

#pragma once
#include "Window.h"

class ConfigManagerWindowPrivate;
class ConfigManagerWindow : public Window
{
    Q_OBJECT
public:
    ConfigManagerWindow();
    ~ConfigManagerWindow();

    /**
    * @brief layoutExec
    */
   void layoutExec();

private slots:
    void userInputCorrect(void);

private:
    ConfigManagerWindowPrivate * const d_ptr;
};
