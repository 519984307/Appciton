#pragma once
#include <QWidget>


class FloatScrollBarPrivate;
class QScrollArea;
class FloatScrollBar : public QWidget
{
    Q_OBJECT
public:
    FloatScrollBar();
    ~FloatScrollBar();

    bool eventFilter(QObject *obj, QEvent *ev);

    void setWidget(QScrollArea *widget);

protected:
    void paintEvent(QPaintEvent *ev);
    void timerEvent(QTimerEvent *ev);
    void showEvent(QShowEvent *ev);

private:
    QScopedPointer<FloatScrollBarPrivate> d_ptr;
};
