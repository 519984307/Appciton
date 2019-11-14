/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/11/8
 **/


#include "KeyActionManager.h"
#include "KeyActionIFace.h"
#include "SystemManager.h"
#include "Debug.h"

KeyActionManager *KeyActionManager::_selfObj = 0;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
KeyActionManager::KeyActionManager()
    : _keyAction(NULL),
      _curPressKey(KEY_TYPE_NR),
      _f1Tof9KeyStatus(KEY_F9_PRESSED + 1),
      _keyEnable(true)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
KeyActionManager::~KeyActionManager()
{
    if (NULL != _keyAction)
    {
        delete _keyAction;
        _keyAction = NULL;
    }
}

/**************************************************************************************************
 * 注册按键动作。
 *************************************************************************************************/
void KeyActionManager::registerKetAction(KeyActionIFace *keyAction)
{
    _keyAction = keyAction;
}

/**************************************************************************************************
 * 处理状态初始化，system自检时会请求所有按键状态，只需调用一次。
 *************************************************************************************************/
void KeyActionManager::initKeyStatus(int data1, int data2, int /*data3*/)
{
    unsigned int status = data1 | (data2 << 8);
    for (int i = 0; i <= KEY_F9_PRESSED; ++i)
    {
        bool keyStatus = status & (0x01 << i);
        _f1Tof9KeyStatus.setBit(i, keyStatus);

        //初始开机时，发送开机前就一直按下的按键。
        if (keyStatus)
        {
            unsigned char keyData[2];
            keyData[0] = 0x61 + i;   // 按键字符
            keyData[1] = 1;   // 按下弹起状态
            systemManager.parseKeyValue(keyData, sizeof(keyData));
        }
    }
}

/**************************************************************************************************
 * 检查按键状态。
 *************************************************************************************************/
bool KeyActionManager::checkKeyPressed(KeyType key)
{
    if (key > KEY_F9_PRESSED)
    {
        return false;
    }

    return _f1Tof9KeyStatus.testBit(key) || (_curPressKey == key);
}

/***************************************************************************************************
 * reset the key action manager
 **************************************************************************************************/
void KeyActionManager::reset()
{
    _f1Tof9KeyStatus.fill(false);
    _curPressKey =  KEY_TYPE_NR;
}

void KeyActionManager::setKeyEnable(bool enable)
{
    _keyEnable = enable;
}

/**************************************************************************************************
 * 处理按键。
 *************************************************************************************************/
void KeyActionManager::handleKeyAction(int keyType)
{
    if (NULL == _keyAction || _keyEnable == false)
    {
        return;
    }

    bool multiBtnPress = false;
    if (keyType <= KEY_F9_PRESSED)
    {
        if (_curPressKey != KEY_TYPE_NR || _f1Tof9KeyStatus.count(true))
        {
            multiBtnPress = true;
        }
        else
        {
            _curPressKey = keyType;
        }

        _f1Tof9KeyStatus.setBit(keyType, true);
    }
    else if (keyType >= KEY_F1_RELEASED && keyType <= KEY_F9_RELEASED)
    {
        int pressType = keyType - KEY_F1_RELEASED;
        if (_curPressKey != pressType)
        {
            _f1Tof9KeyStatus.setBit(pressType, false);
            if (_f1Tof9KeyStatus.count(true))
            {
                multiBtnPress = true;
            }
        }
        else
        {
            _f1Tof9KeyStatus.setBit(_curPressKey, false);
            _curPressKey = KEY_TYPE_NR;
        }
    }

    switch (keyType)
    {
    case KEY_F1_PRESSED:
        _keyAction->keyF1Pressed(multiBtnPress);
        break;
    case KEY_F2_PRESSED:
        _keyAction->keyF2Pressed(multiBtnPress);
        break;
    case KEY_F3_PRESSED:
        _keyAction->keyF3Pressed(multiBtnPress);
        break;
    case KEY_F4_PRESSED:
        _keyAction->keyF4Pressed(multiBtnPress);
        break;
    case KEY_F5_PRESSED:
        _keyAction->keyF5Pressed(multiBtnPress);
        break;
    case KEY_F6_PRESSED:
        _keyAction->keyF6Pressed(multiBtnPress);
        break;
    case KEY_F7_PRESSED:
        _keyAction->keyF7Pressed(multiBtnPress);
        break;
    case KEY_F8_PRESSED:
        _keyAction->keyF8Pressed(multiBtnPress);
        break;
    case KEY_F9_PRESSED:
        _keyAction->keyF9Pressed(multiBtnPress);
        break;
    case KEY_Q_PRESSED:
        _keyAction->keyQPressed(multiBtnPress);
        break;
    case KEY_W_PRESSED:
        _keyAction->keyWPressed(multiBtnPress);
        break;
    case KEY_E_PRESSED:
        _keyAction->keyEPressed(multiBtnPress);
        break;
    case KEY_R_PRESSED:
        _keyAction->keyRPressed(multiBtnPress);
        break;
    case KEY_T_PRESSED:
        _keyAction->keyTPressed(multiBtnPress);
        break;
    case KEY_F1_RELEASED:
        _keyAction->keyF1Released(multiBtnPress);
        break;
    case KEY_F2_RELEASED:
        _keyAction->keyF2Released(multiBtnPress);
        break;
    case KEY_F3_RELEASED:
        _keyAction->keyF3Released(multiBtnPress);
        break;
    case KEY_F4_RELEASED:
        _keyAction->keyF4Released(multiBtnPress);
        break;
    case KEY_F5_RELEASED:
        _keyAction->keyF5Released(multiBtnPress);
        break;
    case KEY_F6_RELEASED:
        _keyAction->keyF6Released(multiBtnPress);
        break;
    case KEY_F7_RELEASED:
        _keyAction->keyF7Released(multiBtnPress);
        break;
    case KEY_F8_RELEASED:
        _keyAction->keyF8Released(multiBtnPress);
        break;
    case KEY_F9_RELEASED:
        _keyAction->keyF9Released(multiBtnPress);
        break;
    case KEY_Q_RELEASED:
        _keyAction->keyQReleased(multiBtnPress);
        break;
    case KEY_W_RELEASED:
        _keyAction->keyWReleased(multiBtnPress);
        break;
    case KEY_E_RELEASED:
        _keyAction->keyEReleased(multiBtnPress);
        break;
    case KEY_R_RELEASED:
        _keyAction->keyRReleased(multiBtnPress);
        break;
    case KEY_T_RELEASED:
        _keyAction->keyTReleased(multiBtnPress);
        break;
    default:
        break;
    }
}

