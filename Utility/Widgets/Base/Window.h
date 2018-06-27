#pragma once
#include <QDialog>
#include <QScopedPointer>

class WindowPrivate;
class Window : public QDialog
{
    Q_OBJECT
public:
    Window(QWidget *parent = NULL);
    ~Window();

    QLayout *getWindowLayout();
    void setWindowLayout(QLayout *layout);

protected:
    /* reimplement */
    void changeEvent(QEvent *ev);

private:
    QScopedPointer<WindowPrivate> d_ptr;
};
