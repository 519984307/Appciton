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

class CodeMarkerWindow : public Window
{
    Q_OBJECT
public:
    static CodeMarkerWindow *getInstance(void);
    ~CodeMarkerWindow();
public:
    void setPress(bool flag);
    bool getPress(void);

    //滚动区域变化
    void changeScrollValue(int value);

    // 获取code marker Type个数。
    int getCodeMarkerTypeSize();

    // 启动定时器。
    void startTimer(void);


protected:
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void exit(void);

private slots :
    void _btnReleased();
    // 5秒后关闭code Marker窗口(当5s内未点击)
    void _closeWidgetTimerFun(void);

    void _timerOut(void);

private:
    CodeMarkerWindow();
    CodeMarkerWindowPrivate * const d_ptr;
};
