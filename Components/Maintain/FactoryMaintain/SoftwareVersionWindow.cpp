/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/
#include "SoftwareVersionWindow.h"
#include <QLabel>
#include <QMap>
#include <QProcess>
#include <QVBoxLayout>
#include "SystemManager.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include "LanguageManager.h"

SoftwareVersionWindow::SoftwareVersionWindow(): Dialog()
{
    layoutExec();
}

void SoftwareVersionWindow::layoutExec()
{
    setWindowTitle(trs("SoftwareVersionMenu"));

    QGridLayout *glayout = new QGridLayout;

    QLabel *label = new QLabel;
    label->setPixmap(QPixmap("/usr/local/nPM/icons/betterLife.png"));
    glayout->addWidget(label, 0, 0, 2, 1, Qt::AlignCenter);
    glayout->setColumnStretch(0, 1);

    label = new QLabel(QString("%1:").arg(trs("ReleaseVersion")));
    glayout->addWidget(label, 0, 1, Qt::AlignLeft | Qt::AlignHCenter);
    label = new QLabel;
    QString softwaveVersion = QString(GIT_VERSION).left(2);
    label->setText(softwaveVersion);
    glayout->addWidget(label, 0, 2, Qt::AlignLeft | Qt::AlignHCenter);

    label = new QLabel(QString("%1:").arg(trs("SoftwareVersion")));
    glayout->addWidget(label, 1, 1, Qt::AlignLeft | Qt::AlignHCenter);
    label = new QLabel;
    softwaveVersion = QString(GIT_VERSION);
    label->setText(softwaveVersion);
    glayout->addWidget(label, 1, 2, Qt::AlignLeft | Qt::AlignHCenter);

    label = new QLabel(QString("%1:").arg(trs("CompileTime")));
    glayout->addWidget(label, 2, 1, Qt::AlignLeft | Qt::AlignHCenter);
    label = new QLabel;
    label->setText(QString("%1 %2").arg(__TIME__).arg(__DATE__));
    glayout->addWidget(label, 2, 2, Qt::AlignLeft | Qt::AlignHCenter);

    setWindowLayout(glayout);

    setFixedSize(600, 240);
}
