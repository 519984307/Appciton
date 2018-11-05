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
#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include <QProcess>
#include <QVBoxLayout>
#include "SystemManager.h"

class SoftWareVersionWindowPrivate
{
public:
    enum MenuItem
    {
        ITEM_LAB_SYS_VER = 0,
        ITEM_LAB_BIULD_TIME,
        ITEM_LAB_KERNEL,
        ITEM_LAB_LOGO,
        ITEM_LAB_MAX,
    };

    SoftWareVersionWindowPrivate() {}
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, QLabel *> labs;
};


void SoftWareVersionWindowPrivate::loadOptions()
{
    QString outPut;
    QProcess process;
    outPut.clear();
    process.start("cat /proc/version");

    if (process.waitForFinished(1000))
    {
        QByteArray byteArray = process.readAll();
        outPut = byteArray;
    }
    QStringList versionList = outPut.split(" ");
    if (versionList.count() >= 3)
    {
        labs[ITEM_LAB_KERNEL]->setText(trs(versionList.at(2)));
    }

    labs[ITEM_LAB_SYS_VER]->setText(GIT_VERSION);

    labs[ITEM_LAB_BIULD_TIME]->setText(QString("%1 %2").arg(__TIME__).arg(__DATE__));
}

SoftWareVersionWindow::SoftWareVersionWindow():
    Window(),
    d_ptr(new SoftWareVersionWindowPrivate)
{
    layoutExec();
    readyShow();
}

SoftWareVersionWindow::~SoftWareVersionWindow()
{
    delete d_ptr;
}

void SoftWareVersionWindow::readyShow()
{
    d_ptr->loadOptions();
}

void SoftWareVersionWindow::layoutExec()
{
    setWindowTitle(trs("SoftWareVersionMenu"));

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setMargin(10);

    QGridLayout *layout = new QGridLayout;
    layout->setVerticalSpacing(10);
    vlayout->addStretch();
    vlayout->addLayout(layout);
    vlayout->addStretch();
    setFixedSize(480, 260);

    QLabel *labelLeft;
    QLabel *labelRight;

    labelLeft = new QLabel(trs("SystemSoftwareVersion"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(SoftWareVersionWindowPrivate
                       ::ITEM_LAB_SYS_VER, labelRight);

    labelLeft = new QLabel(trs("BuildTime"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(SoftWareVersionWindowPrivate
                       ::ITEM_LAB_BIULD_TIME, labelRight);


    labelLeft = new QLabel(trs("Kernel"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(SoftWareVersionWindowPrivate
                       ::ITEM_LAB_KERNEL, labelRight);

    labelLeft = new QLabel(trs("Logo"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel;
    labelRight->setPixmap(QPixmap("/usr/local/nPM/icons/betterLife128.png"));
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(SoftWareVersionWindowPrivate
                       ::ITEM_LAB_LOGO, labelRight);

    layout->setRowStretch(d_ptr->labs.count(), 1);

    setWindowLayout(vlayout);
}
