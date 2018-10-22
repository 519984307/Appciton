/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/22
 **/

#pragma once
#include <QList>
#include "PopupWidget.h"
#include <qscrollarea.h>
#include "LabelButton.h"
#include "SystemDefine.h"

class CodeMarkerButton: public LButton
{
    Q_OBJECT

public:
    explicit CodeMarkerButton(int id = 0);
    ~CodeMarkerButton();

protected:
    void focusInEvent(QFocusEvent *e);
    void paintEvent(QPaintEvent *e);
//    void keyReleaseEvent(QKeyEvent *e);
    void keyPressEvent(QKeyEvent *e);
//    void mouseReleaseEvent(QMouseEvent *e);
};

// Code marker数据窗口
class QTimer;
class CodeMarkerWidget: public PopupWidget
{
    Q_OBJECT

public:
    static CodeMarkerWidget &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new CodeMarkerWidget();
        }

        return *_selfObj;
    }
    static CodeMarkerWidget *_selfObj;
    ~CodeMarkerWidget();

public:
    //滚动区域变化
    void changeScrollValue(int value);

    // 启动定时器。
    void startTimer(void);

    // 获取code marker Type个数。
    int getCodeMarkerTypeSize();

    void setPress(bool flag);
    bool getPress(void);

#ifdef CONFIG_UNIT_TEST
    friend class TestCodeMarker;
#endif

protected:
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *event);
    void exit(void);

private slots:
    // 按键事件回调函数。
    void _btnReleased(int);

    // 5秒后关闭code Marker窗口(当5s内未点击)
    void _closeWidgetTimerFun();

private slots:
    void _timerOut();

private:
    CodeMarkerWidget();

private:
    QScrollArea *_scorllArea;
    QList<CodeMarkerButton *> _lButton;
    CodeMarkerButton *_codeMarkerButton[30];
    QStringList _origCodeMarker;      // 未经过翻译的code emarker。
    QStringList _localeCodeMarker;    // 翻译后的code emarker。
    QTimer *_timer;
    QTimer *_closeTimer;
    bool _isPress;                    // codemark按钮被按下
    bool _isChosen;                   // codemark按钮已按下选中
    int _codeMarkerNum;
};
#define codeMarkerWidget (CodeMarkerWidget::construction())
#define deleteCodeMarkerWidget() (delete CodeMarkerWidget::_selfObj)


