#pragma once
#include <QWidget>

class QBasicTimer;
class FloatScrollBar : public QWidget
{
    Q_OBJECT
public:
    explicit FloatScrollBar(QWidget *parent = NULL);
    ~FloatScrollBar();

    /* reimplement */
    void setVisible(bool visiable);

protected:
    /* reimplement */
    void paintEvent(QPaintEvent *ev);
    /* reimplement */
    void timerEvent(QTimerEvent *ev);
    /* reimplement */
    void showEvent(QShowEvent *ev);
    /* reimplement */
    void moveEvent(QMoveEvent *ev);

private:
    QBasicTimer *_timer;
};
