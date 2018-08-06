/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/6
 **/


#include "CodeMarkerWindow.h"

class CodeMarkerWindowPrivate
{
public:
};

CodeMarkerWindow::CodeMarkerWindow() : Window()
        , d_ptr(new CodeMarkerWindowPrivate())
{
}

CodeMarkerWindow::~CodeMarkerWindow()
{
    delete d_ptr;
}
