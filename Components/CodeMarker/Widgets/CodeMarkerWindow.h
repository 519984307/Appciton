/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/6
 **/


#include "Window.h"

class CodeMarkerWindowPrivate;
class CodeMarkerWindow : public Window
{
public:
    static CodeMarkerWindow &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new CodeMarkerWindow();
        }
        return *_selfObj;
    }
    static CodeMarkerWindow *_selfObj;
    ~CodeMarkerWindow();
private:
    CodeMarkerWindow();
    CodeMarkerWindowPrivate * const d_ptr;
};
