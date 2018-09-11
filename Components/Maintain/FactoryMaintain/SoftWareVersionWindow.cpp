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
#include "IConfig.h"
#include <QProcess>
#include <QVBoxLayout>
#include <ECGParam.h>
#include "ParamManager.h"
#include "Provider.h"

class SoftWareVersionWindowPrivate
{
public:
    enum MenuItem
    {
        ITEM_LAB_SYS_VER = 0,
        ITEM_LAB_BIULD_TIME,
        ITEM_LAB_U_BOOT,
        ITEM_LAB_KERNEL,
        ITEM_LAB_KEYBD_MOD,
        ITEM_LAB_ECG_ALGHTP,
        ITEM_LAB_ECG_VERSION,
        ITEM_LAB_NIBP_VERSION,
        ITEM_LAB_SPO2_VERSION,
        ITEM_LAB_TEMP_VERSION,
        ITEM_LAB_PRT72_VERSION,
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

    if (process.waitForFinished())
    {
        QByteArray byteArray = process.readAll();
        outPut = byteArray;
    }
    QStringList versionList = outPut.split(" ");
    if (versionList.count() >= 3)
    {
        labs[ITEM_LAB_KERNEL]->setText(trs(versionList.at(2)));
    }

    QString tmpStr;

    labs[ITEM_LAB_SYS_VER]->setText(GIT_VERSION);

    labs[ITEM_LAB_BIULD_TIME]->setText(QString("%1 %2").arg(__TIME__).arg(__DATE__));

    tmpStr.clear();
    systemConfig.getStrValue("SoftWareVersion|Uboot", tmpStr);
    labs[ITEM_LAB_U_BOOT]->setText(trs(tmpStr));

    tmpStr.clear();
    systemConfig.getStrValue("SoftWareVersion|KeyboardModule", tmpStr);
    labs[ITEM_LAB_KEYBD_MOD]->setText(trs(tmpStr));

    tmpStr.clear();
    systemConfig.getStrValue("SoftWareVersion|ECGAlgorithmType", tmpStr);
    labs[ITEM_LAB_ECG_ALGHTP]->setText(trs(tmpStr));
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

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setMargin(10);

    QGridLayout *layout = new QGridLayout;
    layout->setVerticalSpacing(10);
    vlayout->addStretch();
    vlayout->addLayout(layout);
    vlayout->addStretch();
    setFixedSize(480, 480);

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

    labelLeft = new QLabel(trs("Uboot"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(SoftWareVersionWindowPrivate
                       ::ITEM_LAB_U_BOOT, labelRight);

    labelLeft = new QLabel(trs("Kernel"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(SoftWareVersionWindowPrivate
                       ::ITEM_LAB_KERNEL, labelRight);

    labelLeft = new QLabel(trs("SystemboardModule"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(SoftWareVersionWindowPrivate
                       ::ITEM_LAB_KEYBD_MOD, labelRight);

    labelLeft = new QLabel(trs("ECGAlgorithmType"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(SoftWareVersionWindowPrivate
                       ::ITEM_LAB_ECG_ALGHTP, labelRight);

    labelLeft = new QLabel(trs("ECGVersion") + ":   ");
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    Provider *p = paramManager.getProvider("BLM_E5");
    QString version;
    if (p)
    {
        version = p->getVersionString();
    }

    labelRight = new QLabel(version);
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(SoftWareVersionWindowPrivate
                       ::ITEM_LAB_ECG_VERSION, labelRight);

    if (systemManager.isSupport(CONFIG_NIBP))
    {
        labelLeft = new QLabel(trs("NIBPVersion") + ":   ");
        layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
        Provider *p = paramManager.getProvider("BLM_TN3");
        QString version;
        if (p)
        {
            version = p->getVersionString();
        }

        labelRight = new QLabel(version);
        labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
        layout->addWidget(labelRight, d_ptr->labs.count(), 1);
        d_ptr->labs.insert(SoftWareVersionWindowPrivate
                           ::ITEM_LAB_NIBP_VERSION, labelRight);
    }

    if (systemManager.isSupport(CONFIG_SPO2))
    {
        labelLeft = new QLabel(trs("SPO2Version") + ":   ");
        layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
        Provider *p = paramManager.getProvider("BLM_S5");
        QString version;
        if (p)
        {
            version = p->getVersionString();
        }

        labelRight = new QLabel(version);
        labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
        layout->addWidget(labelRight, d_ptr->labs.count(), 1);
        d_ptr->labs.insert(SoftWareVersionWindowPrivate
                           ::ITEM_LAB_SPO2_VERSION, labelRight);
    }

    if (systemManager.isSupport(CONFIG_TEMP))
    {
        labelLeft = new QLabel(trs("TEMPVersion") + ":   ");
        layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
        Provider *p = paramManager.getProvider("BLM_T5");
        QString version;
        if (p)
        {
            version = p->getVersionString();
        }


        labelRight = new QLabel(version);
        labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
        layout->addWidget(labelRight, d_ptr->labs.count(), 1);
        d_ptr->labs.insert(SoftWareVersionWindowPrivate
                           ::ITEM_LAB_TEMP_VERSION, labelRight);
    }

    labelLeft = new QLabel(trs("PRT72Version") + ":   ");
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    p = paramManager.getProvider("PRT72");
    if (p)
    {
        version = p->getVersionString();
    }


    labelRight = new QLabel(version);

    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(SoftWareVersionWindowPrivate
                       ::ITEM_LAB_PRT72_VERSION, labelRight);

    layout->setRowStretch(d_ptr->labs.count(), 1);

    setWindowLayout(vlayout);
}










































