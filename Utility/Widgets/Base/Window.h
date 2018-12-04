/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/


#pragma once
#include <QDialog>
#include <QScopedPointer>

class WindowPrivate;
class Button;
class Window : public QDialog
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = NULL);
    ~Window();

    QLayout *getWindowLayout();
    void setWindowLayout(QLayout *layout);

    /**
     * @brief showMask show the window mask or not
     * @param flag
     */
    void showMask(bool flag);

    /**
     * @brief isShowingMask check whether showing mask
     * @return
     */
    bool isShowingMask() const;

    /**
     * @brief getCloseBtn  获取关闭按钮的指针
     * @return
     */
    Button *getCloseBtn(void) const;

    /* reimpelment */
    int exec();

signals:
    /**
     * @brief windowHide emit when the window going to hide
     */
    void windowHide(Window *w);

protected:
    /* reimplement */
    void changeEvent(QEvent *ev);
    /* reimplement */
    void resizeEvent(QResizeEvent *ev);
    /* reimplement */
    void keyPressEvent(QKeyEvent *ev);
    /* reimplement */
    void keyReleaseEvent(QKeyEvent *ev);
    /* reimplement */
    void hideEvent(QHideEvent *ev);
    /* reimplement */
    void paintEvent(QPaintEvent *ev);


    /**
     * @brief getTitleHeight 获取标题栏高度
     * @return
     */
    const int getTitleHeight();

private:
    QScopedPointer<WindowPrivate> d_ptr;
};
