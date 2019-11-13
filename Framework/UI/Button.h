/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/8
 **/

#pragma once
#include <QAbstractButton>
#include <QScopedPointer>

class ButtonPrivate;
class Button : public QAbstractButton
{
    Q_OBJECT
public:
    enum ButtonStyle
    {
        ButtonTextOnly,
        ButtonIconOnly,
        ButtonTextBesideIcon,
        ButtonTextUnderIcon,
    };

    /**
     * @brief Button constructor
     * @param text  button text
     * @param icon button icon
     * @param parent
     */
    explicit Button(const QString &text = QString(), const QIcon &icon = QIcon(), QWidget *parent = NULL);

    ~Button();

    /**
     * @brief setBorderWidth
     */
    void setBorderWidth(int width);

    /**
     * @brief borderWidth
     * @return
     */
    int borderWidth() const;

    /**
     * @brief setBorderRadius set the button border radius
     * @param radius
     */
    void setBorderRadius(int radius);

    /**
     * @brief borderRadius get teh border radius
     * @return  the radius value
     */
    int borderRadius() const;

    /**
     * @brief setButtonStyle set the button style
     * @param style
     */
    void setButtonStyle(ButtonStyle style);

    /**
     * @brief buttonStyle get the button style
     * @return
     */
    ButtonStyle buttonStyle() const;

    /* reimplement */
    QSize sizeHint() const;

protected:
    /* reimplement */
    void paintEvent(QPaintEvent *ev);
    /* reimplement */
    void keyPressEvent(QKeyEvent *ev);
    /* reimplement */
    void keyReleaseEvent(QKeyEvent *ev);

    void mousePressEvent(QMouseEvent *e);

private:
    QScopedPointer<ButtonPrivate> d_ptr;
};
