/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/3
 **/

#pragma once
#include "KeyActionIFace.h"

class NormalModeKeyAction : public KeyActionIFace
{
public:
    NormalModeKeyAction();
    ~NormalModeKeyAction();

    //按下
    virtual void keyF1Pressed(bool multiBtnPress);
    virtual void keyF2Pressed(bool multiBtnPress);
    virtual void keyF3Pressed(bool multiBtnPress);
    virtual void keyF4Pressed(bool multiBtnPress);
    virtual void keyF5Pressed(bool multiBtnPress);
    virtual void keyF6Pressed(bool multiBtnPress);

    //释放
    virtual void keyF1Released(bool multiBtnPress);
    virtual void keyF2Released(bool multiBtnPress);
    virtual void keyF3Released(bool multiBtnPress);
    virtual void keyF4Released(bool multiBtnPress);
    virtual void keyF5Released(bool multiBtnPress);
    virtual void keyF6Released(bool multiBtnPress);
};

