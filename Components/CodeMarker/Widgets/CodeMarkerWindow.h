/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/6
 **/


#include "Dialog.h"
#include "Button.h"

class CodeMarkerWindowPrivate;

class CodeMarkerWindowButton : public Button
{
    Q_OBJECT
public:
    explicit CodeMarkerWindowButton(int id = 0) : _id(id)
    {
    }
    ~CodeMarkerWindowButton(){}

protected:
    void focusInEvent(QFocusEvent *e);
    void keyPressEvent(QKeyEvent *e);

private:
    int _id;
};

class CodeMarkerWindow : public Dialog
{
    Q_OBJECT
public:
    static CodeMarkerWindow &getInstance(void);
    ~CodeMarkerWindow();
public:
    void setPress(bool flag);
    bool getPress(void);

    //滚动区域变化
    /**
     * @brief ensureWidgetVisiable
     * @param id 哪个控件要被显示在scrollArea
     */
    void ensureWidgetVisiable(int id);

    // 获取code marker Type个数。
    int getCodeMarkerTypeSize();

protected:
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void exit(void);

private slots :
    void _btnReleased();

    void _timerOut(void);

private:
    CodeMarkerWindow();
    CodeMarkerWindowPrivate * const d_ptr;
};
#define codeMarkerWindow (CodeMarkerWindow::getInstance())
#define deleteCodeMarkerWindow (delete CodeMarkerWindow)
