#pragma once

class KeyActionIFace
{
public:
    //构造与析构
    KeyActionIFace() {}
    virtual ~KeyActionIFace() {}

    //按下
    virtual void keyF1Pressed(bool /*multiBtnPress*/) {}
    virtual void keyF2Pressed(bool /*multiBtnPress*/) {}
    virtual void keyF3Pressed(bool /*multiBtnPress*/) {}
    virtual void keyF4Pressed(bool /*multiBtnPress*/) {}
    virtual void keyF5Pressed(bool /*multiBtnPress*/) {}
    virtual void keyF6Pressed(bool /*multiBtnPress*/) {}
    virtual void keyF7Pressed(bool /*multiBtnPress*/) {}
    virtual void keyF8Pressed(bool /*multiBtnPress*/) {}
    virtual void keyF9Pressed(bool /*multiBtnPress*/) {}
    virtual void keyQPressed(bool /*multiBtnPress*/) {}
    virtual void keyWPressed(bool /*multiBtnPress*/) {}
    virtual void keyEPressed(bool /*multiBtnPress*/) {}
    virtual void keyRPressed(bool /*multiBtnPress*/) {}
    virtual void keyTPressed(bool /*multiBtnPress*/) {}

    //释放
    virtual void keyF1Released(bool /*multiBtnPress*/) {}
    virtual void keyF2Released(bool /*multiBtnPress*/) {}
    virtual void keyF3Released(bool /*multiBtnPress*/) {}
    virtual void keyF4Released(bool /*multiBtnPress*/) {}
    virtual void keyF5Released(bool /*multiBtnPress*/) {}
    virtual void keyF6Released(bool /*multiBtnPress*/) {}
    virtual void keyF7Released(bool /*multiBtnPress*/) {}
    virtual void keyF8Released(bool /*multiBtnPress*/) {}
    virtual void keyF9Released(bool /*multiBtnPress*/) {}
    virtual void keyQReleased(bool /*multiBtnPress*/) {}
    virtual void keyWReleased(bool /*multiBtnPress*/) {}
    virtual void keyEReleased(bool /*multiBtnPress*/) {}
    virtual void keyRReleased(bool /*multiBtnPress*/) {}
    virtual void keyTReleased(bool /*multiBtnPress*/) {}
};

