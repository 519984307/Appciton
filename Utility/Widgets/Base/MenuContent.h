/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/5
 **/
#pragma once
#include <QWidget>

class MenuContentPrivate;
class MenuWindow;
class MenuContent : public QWidget
{
    Q_OBJECT
public:
    MenuContent(const QString &name, const QString &description,
                QWidget *parent = NULL);
    ~MenuContent();

    /**
     * @brief name get the menu name
     * @return name
     */
    const QString &name() const;

    /**
     * @brief description get the description
     * @return description
     */
    const QString &description() const;

    /**
     * @brief layoutExec do the layout
     */
    virtual void layoutExec() {}

    /**
     * @brief readyShow start to show
     */
    virtual void readyShow() {}

    /**
     * @brief getMenuWindow get the menu window the window place in
     * @return the menu window or NULL if the menu hasn't placed in the window yet
     */
    MenuWindow *getMenuWindow();

protected:
    void showEvent(QShowEvent *ev);

    /* reimplement */
    void focusInEvent(QFocusEvent *ev);

private:
    MenuContentPrivate *const d_ptr;
};

