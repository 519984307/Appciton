/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/22
 **/

#pragma once
#include "Dialog.h"

class RescueDataDeleteWindowPrivate;
class RescueDataDeleteWindow : public Dialog
{
    Q_OBJECT
public:
    static RescueDataDeleteWindow &construction()
    {
        if (_selfObj == NULL)
        {
            _selfObj = new RescueDataDeleteWindow();
        }
        return *_selfObj;
    }
    static RescueDataDeleteWindow *_selfObj;
    ~RescueDataDeleteWindow();

private slots:
    void _updateWindowTitle();
    void _deleteSelectReleased();
    void _deleteAllReleased();
    void _upReleased();
    void _downReleased();
    void _updateEraseBtnStatus();

private:
    RescueDataDeleteWindow();
    RescueDataDeleteWindowPrivate *d_ptr;
};

#define rescueDataDeleteWindow (RescueDataDeleteWindow::construction())
