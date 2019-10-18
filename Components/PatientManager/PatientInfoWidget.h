/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/29
 **/


#pragma once
#include "PatientInfoWidgetInterface.h"
#include "PatientDefine.h"
#include <QStackedWidget>
#include <QPainter>

class QLabel;
class PatientInfoWidget : public PatientInfoWidgetInterface
{
    Q_OBJECT

public:
    explicit PatientInfoWidget(QWidget *parent = 0);
    ~PatientInfoWidget();

    void loadPatientInfo(const PatientInfo &info, const QString &bed);

    void getText(QString &bed, QString &name, QString &type);   /* NOLINT */

    /* override */
    void setAlarmPauseTime(int seconds);

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_bed;
    QLabel *_patientName;
    QLabel *_patientType;
    QLabel *_alarmPauseMessage;
    QStackedWidget *_stack;
};
