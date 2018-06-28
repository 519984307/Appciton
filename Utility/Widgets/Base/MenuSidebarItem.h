#pragma once
#include<QAbstractButton>

class MenuSidebarItem : public QAbstractButton
{
    Q_OBJECT
public:
    explicit MenuSidebarItem(QWidget *parent = NULL);

    QSize sizeHint() const;


signals:
    void focusChanged(bool in);

protected:
    /* reimplement */
    void paintEvent(QPaintEvent *ev);

    /* reimplement */
    void nextCheckState();

    /* reimplement */
    void focusInEvent(QFocusEvent *ev);

    /* reimplement */
    void focusOutEvent(QFocusEvent *ev);
};
