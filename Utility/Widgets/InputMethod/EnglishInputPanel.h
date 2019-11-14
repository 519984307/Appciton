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

class EnglishInputPanelPrivate;
class EnglishInputPanel: public Dialog
{
    Q_OBJECT

public:
    /**
     * @brief setInitString
     * @param text
     */
    void setInitString(const QString &text);

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


    EnglishInputPanel();
    ~EnglishInputPanel();

    typedef bool (*CheckValue)(QString &);
    /**
     * @brief setCheckValueHook
     * @param hook
     */
    void setCheckValueHook(CheckValue hook);

protected:
    virtual void showEvent(QShowEvent *e);

private slots:
    /**
     * @brief ClickedKey
     */
    void ClickedKey();
    /**
     * @brief ClickedShift
     */
    void ClickedShift(void);
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

private:
    /**
     * @brief _loadKeyStatus
     */
    void _loadKeyStatus();
    CheckValue _checkValue;

    EnglishInputPanelPrivate *const d_ptr;
};
