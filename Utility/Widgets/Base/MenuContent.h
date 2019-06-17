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
     * @brief doLayout perform the layout
     */
    void doLayout();


    /**
     * @brief getMenuWindow get the menu window the window place in
     * @return the menu window or NULL if the menu hasn't placed in the window yet
     */
    MenuWindow *getMenuWindow();

    /**
     * @brief setShowParam set the menu content param that should be handled before show
     *                       the param can be read only once by the takeShowParam interface
     * @param param the param
     */
    virtual void setShowParam(const QVariant &param);

protected:
    /* reimplement */
    void showEvent(QShowEvent *ev);

    /* reimplement */
    void focusInEvent(QFocusEvent *ev);

    /**
     * @brief readyShow start to show
     */
    virtual void readyShow() {}

    /**
     * @brief layoutExec do the layout
     */
    virtual void layoutExec() {}

private:
    MenuContentPrivate *const d_ptr;
};

