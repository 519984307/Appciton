#pragma once
#include<QAbstractButton>

class MenuSidebarItem : public QAbstractButton
{
    Q_OBJECT
public:
    MenuSidebarItem(QWidget *parent = NULL);

    QSize sizeHint() const;

protected:
    /* reimplement */
    void paintEvent(QPaintEvent *ev);

    /* reimplement */
    void nextCheckState();
};
