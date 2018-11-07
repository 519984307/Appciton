/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/11/5
 **/


#include "FactoryVersionInfo.h"
#include <QMap>
#include "LanguageManager.h"
#include <QLabel>
#include <QGridLayout>
#include "IConfig.h"
#include "ParamManager.h"
#include "Provider.h"

class FactoryVersionInfoPrivate
{
public:
    enum MenuItem
    {
        ITEM_LAB_U_BOOT,
        ITEM_LAB_KEYBD_MOD,
        ITEM_LAB_ECG_ALGHTP,
        ITEM_LAB_ECG_VERSION,
        ITEM_LAB_NIBP_VERSION,
        ITEM_LAB_SPO2_VERSION,
        ITEM_LAB_TEMP_VERSION,
        ITEM_LAB_PRT72_VERSION,
        ITEM_LAB_MAX,
    };

    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, QLabel *> labs;
};

FactoryVersionInfo::FactoryVersionInfo()
                  : MenuContent(trs("FactoryVersionInfo"),
                                trs("FactoryVersionInfoDesc")),
                    d_ptr(new FactoryVersionInfoPrivate)
{
}

FactoryVersionInfo::~FactoryVersionInfo()
{
    delete d_ptr;
}

void FactoryVersionInfo::readyShow()
{
    d_ptr->loadOptions();
}

void FactoryVersionInfo::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *labelLeft;
    QLabel *labelRight;

    labelLeft = new QLabel(trs("Uboot"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
    d_ptr->labs.insert(FactoryVersionInfoPrivate
                       ::ITEM_LAB_U_BOOT, labelRight);

    labelLeft = new QLabel(trs("SystemboardModule"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
    d_ptr->labs.insert(FactoryVersionInfoPrivate
                       ::ITEM_LAB_KEYBD_MOD, labelRight);

    labelLeft = new QLabel(trs("ECGAlgorithmType"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
    d_ptr->labs.insert(FactoryVersionInfoPrivate
                       ::ITEM_LAB_ECG_ALGHTP, labelRight);

    labelLeft = new QLabel(trs("ECGVersion") + "    ");
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
    labelRight = new QLabel;
    layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
    d_ptr->labs.insert(FactoryVersionInfoPrivate
                       ::ITEM_LAB_ECG_VERSION, labelRight);

    if (systemManager.isSupport(CONFIG_NIBP))
    {
        labelLeft = new QLabel(trs("NIBPVersion") + "    ");
        layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);

        labelRight = new QLabel;
        layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
        d_ptr->labs.insert(FactoryVersionInfoPrivate
                           ::ITEM_LAB_NIBP_VERSION, labelRight);
    }

    if (systemManager.isSupport(CONFIG_SPO2))
    {
        labelLeft = new QLabel(trs("SPO2Version") + "    ");
        layout->addWidget(labelLeft, d_ptr->labs.count(), 0);

        labelRight = new QLabel;
        layout->addWidget(labelRight, d_ptr->labs.count(), 1);
        d_ptr->labs.insert(FactoryVersionInfoPrivate
                           ::ITEM_LAB_SPO2_VERSION, labelRight);
    }

    if (systemManager.isSupport(CONFIG_TEMP))
    {
        labelLeft = new QLabel(trs("TEMPVersion") + "    ");
        layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);

        labelRight = new QLabel;
        layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
        d_ptr->labs.insert(FactoryVersionInfoPrivate
                           ::ITEM_LAB_TEMP_VERSION, labelRight);
    }

    labelLeft = new QLabel(trs("PRT72Version") + "    ");
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);

    labelRight = new QLabel;

    layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
    d_ptr->labs.insert(FactoryVersionInfoPrivate
                       ::ITEM_LAB_PRT72_VERSION, labelRight);

    layout->setRowStretch(d_ptr->labs.count(), 1);
}

void FactoryVersionInfoPrivate::loadOptions()
{
    QString tmpStr;
    tmpStr.clear();
    systemConfig.getStrValue("SoftWareVersion|Uboot", tmpStr);
    labs[ITEM_LAB_U_BOOT]->setText(trs(tmpStr));

    tmpStr.clear();
    systemConfig.getStrValue("SoftWareVersion|KeyboardModule", tmpStr);
    labs[ITEM_LAB_KEYBD_MOD]->setText(trs(tmpStr));

    tmpStr.clear();
    systemConfig.getStrValue("SoftWareVersion|ECGAlgorithmType", tmpStr);
    labs[ITEM_LAB_ECG_ALGHTP]->setText(trs(tmpStr));

    Provider *p = paramManager.getProvider("BLM_E5");
    QString version;
    if (p)
    {
        version = p->getVersionString();
    }
    labs[ITEM_LAB_ECG_VERSION]->setText(version);

    if (labs[ITEM_LAB_NIBP_VERSION])
    {
        Provider *p = paramManager.getProvider("BLM_TN3");
        QString version;
        if (p)
        {
            version = p->getVersionString();
        }
        labs[ITEM_LAB_NIBP_VERSION]->setText(version);
    }

    if (labs[ITEM_LAB_SPO2_VERSION])
    {
        Provider *p = paramManager.getProvider("BLM_S5");
        QString version;
        if (p)
        {
            version = p->getVersionString();
        }
        labs[ITEM_LAB_SPO2_VERSION]->setText(version);
    }

    if (labs[ITEM_LAB_TEMP_VERSION])
    {
        Provider *p = paramManager.getProvider("BLM_T5");
        QString version;
        if (p)
        {
            version = p->getVersionString();
        }
        labs[ITEM_LAB_TEMP_VERSION]->setText(version);
    }

    p = paramManager.getProvider("PRT72");
    if (p)
    {
        version = p->getVersionString();
    }
    labs[ITEM_LAB_PRT72_VERSION]->setText(version);
}
