/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/9/9
 **/


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
    explicit TSCalibrationThread(QObject *parent);
    void run();

signals:
    void getTouchPos(const QPoint &pos);
};
