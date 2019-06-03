/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/5/27
 **/

#pragma once
#include <QBitArray>

enum KeyType
{
    KEY_F1_PRESSED,
    KEY_F2_PRESSED,
    KEY_F3_PRESSED,
    KEY_F4_PRESSED,
    KEY_F5_PRESSED,
    KEY_F6_PRESSED,
    KEY_F7_PRESSED,
    KEY_F8_PRESSED,
    KEY_F9_PRESSED,
    KEY_Q_PRESSED,
    KEY_W_PRESSED,
    KEY_E_PRESSED,
    KEY_R_PRESSED,
    KEY_T_PRESSED,
    KEY_F1_RELEASED,
    KEY_F2_RELEASED,
    KEY_F3_RELEASED,
    KEY_F4_RELEASED,
    KEY_F5_RELEASED,
    KEY_F6_RELEASED,
    KEY_F7_RELEASED,
    KEY_F8_RELEASED,
    KEY_F9_RELEASED,
    KEY_Q_RELEASED,
    KEY_W_RELEASED,
    KEY_E_RELEASED,
    KEY_R_RELEASED,
    KEY_T_RELEASED,
    KEY_TYPE_NR
};

class KeyActionIFace;
class KeyActionManager
{
public:
    static KeyActionManager &Construction()
    {
        if (0 == _selfObj)
        {
            _selfObj = new KeyActionManager();
        }

        return *_selfObj;
    }

    static KeyActionManager *_selfObj;
    ~KeyActionManager();

    // 注册按键动作
    void registerKetAction(KeyActionIFace *keyAction);

    // 处理按键
    void handleKeyAction(int keyType);

    // 按键状态初始化
    void initKeyStatus(int data1, int data2, int data3);

    // 检查按键是否按下
    bool checkKeyPressed(KeyType key);

    // reset
    void reset();

    /**
     * @brief setKeyEnable 设置按键使能
     * @param enable
     */
    void setKeyEnable(bool enable);

private:
    KeyActionManager();

private:
    KeyActionIFace *_keyAction;
    int _curPressKey;
    QBitArray _f1Tof9KeyStatus;
    bool _keyEnable;
};
#define keyActionManager (KeyActionManager::Construction())
#define deleteKeyActionManager() (delete KeyActionManager::_selfObj)
