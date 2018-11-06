/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/
#include "SoftWareVersionWindow.h"
#include <QLabel>
#include <QMap>
#include <QProcess>
#include <QVBoxLayout>
#include "SystemManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

class SoftWareVersionWindowPrivate
{
public:
    SoftWareVersionWindowPrivate() {}
};


SoftWareVersionWindow::SoftWareVersionWindow(): Window()
{
    layoutExec();
}

void SoftWareVersionWindow::layoutExec()
{
    setWindowTitle(trs("SoftWareVersionMenu"));

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setMargin(10);
    QVBoxLayout *vlayout = new QVBoxLayout;

    setFixedSize(520, 260);

    QLabel *label;

    label = new QLabel;
    label->setPixmap(QPixmap("/usr/local/nPM/icons/betterLife.png"));
    hlayout->addWidget(label, 1, Qt::AlignCenter);

    label = new QLabel(QString("%1:").arg(trs("SystemSoftwareVersion")));
    vlayout->addWidget(label, 1, Qt::AlignLeft | Qt::AlignHCenter);
    label = new QLabel;
    label->setText(GIT_VERSION);
    vlayout->addWidget(label, 1, Qt::AlignLeft | Qt::AlignHCenter);

    label = new QLabel(QString("%1:").arg(trs("BuildTime")));
    vlayout->addWidget(label, 1, Qt::AlignLeft | Qt::AlignHCenter);
    label = new QLabel;
    label->setText(QString("%1 %2").arg(__TIME__).arg(__DATE__));
    vlayout->addWidget(label, 1, Qt::AlignLeft | Qt::AlignHCenter);

    hlayout->addLayout(vlayout, 1);

    setWindowLayout(hlayout);
}
