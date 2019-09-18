/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/23
 **/

#pragma once
#include "Dialog.h"

/**
 * @brief The PasswordWindow class use to verify the input password
 */

class PasswordWindow : public Dialog
{
    Q_OBJECT
public:
    /**
     * @brief PasswordWindow
     * @param name the password name
     * @param password the password text
     */
    PasswordWindow(const QString &name, const QString &password);
};

