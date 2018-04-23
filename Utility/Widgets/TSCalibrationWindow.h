#pragma once
#include <QDialog>
#include <QScopedPointer>
#include <QThread>

class TSCalibrationWindowPrivate;
class TSCalibrationWindow : public QDialog
{
    Q_OBJECT
public:
    TSCalibrationWindow();
    ~TSCalibrationWindow();

protected:
    void showEvent(QShowEvent *ev);
    void paintEvent(QPaintEvent *ev);

private slots:
    void onGetTouchPos(const QPoint &pos);

private:
    QScopedPointer<TSCalibrationWindowPrivate> d_ptr;
};

class TSCalibrationThread : public QThread
{
    Q_OBJECT
public:
    TSCalibrationThread(QObject *parent);
    void run();

signals:
    void getTouchPos(const QPoint &pos);
};
