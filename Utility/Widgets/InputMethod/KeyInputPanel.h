/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/26
 **/
#pragma once
#include "Framework/UI/Dialog.h"
#include <QLineEdit>

class KeyInputPanelPrivate;
class KeyInputPanel: public Dialog
{
    Q_OBJECT

public:
    /**
     * @brief setInitString  设置初始字符串
     * @param text  初始字符串
     * @param isPlaceHolder  初始文本是否placeholder
     */
    void setInitString(const QString &text, bool isPlaceHolder = false);

    /**
     * @brief setMaxInputLength  // 设置最大输入长度。
     * @param len
     */
    void setMaxInputLength(int len);

    /**
     * @brief getStrValue  // 获取当前值。
     * @return
     */
    QString getStrValue(void) const;

    /**
     * @brief setEchoMode  // 设置显示模式
     * @param mode
     */
    void setEchoMode(QLineEdit::EchoMode mode);

    /**
     * @brief setBtnEnable  // 设置使能按钮，正则匹配字符串
     * @param regStr
     */
    void setBtnEnable(const QString &regStr);

    /**
     * @brief setBtnDisable  // 设置非使能按钮，正则匹配字符串
     * @param regStr
     */
    void setBtnDisable(const QString &regStr);

    /**
     * @brief setSpaceEnable  // 设置空格按键使能
     */
    void setSpaceEnable(bool);
    /**
     * @brief setSymbolEnable
     */
    void setSymbolEnable(bool);
    /**
     * @brief setKeytypeSwitchEnable
     */
    void setKeytypeSwitchEnable(bool);

    typedef bool (*CheckValue)(const QString &);
    /**
     * @brief setCheckValueHook
     * @param hook
     */
    void setCheckValueHook(CheckValue hook);

    /**
     * @brief setInvalidHint 设置无效提示信息
     * @param str           无效提示字符串
     */
    void setInvalidHint(const QString &str);

    enum KeyOrder
    {
        KEY_ORDER_BACKSPACE = 0X03,
        KEY_ORDER_CLEAR_ALL = 0X07,
        KEY_ORDER_ENTER     = 0X0B,
        KEY_ORDER_SPACE     = 0X0C,
        KEY_ORDER_SYMBOL    = 0X0E,
        KEY_ORDER_NUMBER    = 0X0F
    };

    enum KeyType
    {
        KEY_TYPE_NUMBER,
        KEY_TYPE_SYMBOL,
        KEY_TYPE_LETTER
    };

    explicit KeyInputPanel(KeyType type = KEY_TYPE_LETTER, bool isShowDecimalPoint = false);
    ~KeyInputPanel();

protected:
    virtual void showEvent(QShowEvent *e);

private slots:
    /**
     * @brief ClickedHelpBtn
     */
    void ClickedHelpBtn();
    /**
     * @brief ClickedBtnText
     */
    void ClickedBtnText();
    /**
     * @brief ClickedBtnId
     */
    void ClickedBtnId();
    /**
     * @brief onKeyClicked
     */
    void onKeyClicked();

    /**
     * @brief onBlankClicked blank clicked
     */
    void onBlankClicked();
    /**
     * @brief ClickedKey
     * @param key
     */
    void ClickedKey(const QString &key);
    /**
     * @brief ClickedBackspace
     */
    void ClickedBackspace(void);
    /**
     * @brief ClickedClear
     */
    void ClickedClear(void);
    /**
     * @brief ClickedEnter
     */
    void ClickedEnter(void);
    /**
     * @brief ClickedNum
     */
    void ClickedNum(void);
    /**
     * @brief ClickedSymbol
     */
    void ClickedSymbol(void);

private:
    /**
     * @brief _loadKeyStatus
     */
    void _loadKeyStatus();
    /**
     * @brief _loadHelpBtn
     * @param key
     */
    void _loadHelpBtn(const QString &key);

private:
    CheckValue _checkValue;
    KeyType _keytype;
    KeyInputPanelPrivate *const d_ptr;
};


