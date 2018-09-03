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

class ConfigManagerMenuContentPrivate;
class ConfigManagerMenuContent : public Window
{
    Q_OBJECT
public:
    ConfigManagerMenuContent();
    ~ConfigManagerMenuContent();

    /**
    * @brief layoutExec
    */
   void layoutExec();

private slots:
    void userInputCorrect(void);

private:
    ConfigManagerMenuContentPrivate * const d_ptr;
};
