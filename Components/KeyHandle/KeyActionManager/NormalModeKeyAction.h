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

