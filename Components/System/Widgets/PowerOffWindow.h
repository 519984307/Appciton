/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/17
 **/

#pragma once
#include <QDialog>

class PowerOffWindowPrivate;
class PowerOffWindow : public QDialog
{
    Q_OBJECT
public:
    static PowerOffWindow &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new PowerOffWindow();
        }
        return *_selfObj;
    }
    static PowerOffWindow* _selfObj;
    ~PowerOffWindow();

    /* reimplement */
    int exec();

private:
    PowerOffWindow();
    PowerOffWindowPrivate * const d_ptr;
};
#define powerOffWindow (PowerOffWindow::construction())
