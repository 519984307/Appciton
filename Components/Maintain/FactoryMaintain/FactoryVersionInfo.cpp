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
#include <QFile>
#include <QTextStream>
#include "FontManager.h"
#include "WindowManager.h"

#define HARDWARE_VERSION ("/sys/bus/iio/devices/iio:device0/in_voltage7_raw")
#define HW_VER_TOTAL_NUMBERS (10)
#define HW_VER_SAMP_REF_VAL (4096)

class FactoryVersionInfoPrivate
{
public:
    enum MenuItem
    {
        ITEM_LAB_U_BOOT,
        ITEM_LAB_HARDWARE_VERSION,
        ITEM_LAB_SYSBD_MOD,
        ITEM_LAB_SYSBD_BOOTLOADER,
        ITEM_LAB_ECG_ALGHTP,
        ITEM_LAB_ECG_VERSION,
        ITEM_LAB_ECG_BOOTLOADER,
        ITEM_LAB_NIBP_VERSION,
        ITEM_LAB_NIBP_BOOTLOADER,
        ITEM_LAB_NIBP_SLAVE_VERSION,
        ITEM_LAB_NIBP_SLAVE_BOOTLOADER,
        ITEM_LAB_SPO2_VERSION,
        ITEM_LAB_SPO2_BOOTLOADER,
        ITEM_LAB_TEMP_VERSION,
        ITEM_LAB_TEMP_BOOTLOADER,
        ITEM_LAB_PRT48_VERSION,
        ITEM_LAB_PRT48_BOOTLOADER,
        ITEM_LAB_SCREEN_INCH,
        ITEM_LAB_SCREEN_RESOLUTION,
        ITEM_LAB_MAX,
    };

    FactoryVersionInfoPrivate()
                   : n5SlaveLab(NULL)
                   , n5SlaveBootloaderLab(NULL)
    {
    }
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, QLabel *> labs;

    QLabel *n5SlaveLab;
    QLabel *n5SlaveBootloaderLab;
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

    // set the font size for showing all version info
    setFont(fontManager.textFont(fontManager.getFontSize(2)));

    QLabel *labelLeft;
    QLabel *labelRight;

    labelLeft = new QLabel(trs("Uboot"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
    d_ptr->labs.insert(FactoryVersionInfoPrivate
                       ::ITEM_LAB_U_BOOT, labelRight);
    labelLeft->setFocusPolicy(Qt::StrongFocus);

    labelLeft = new QLabel(trs("Hardware"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
    d_ptr->labs.insert(FactoryVersionInfoPrivate
                       ::ITEM_LAB_HARDWARE_VERSION, labelRight);

    labelLeft = new QLabel(trs("SystemboardModule"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
    d_ptr->labs.insert(FactoryVersionInfoPrivate
                       ::ITEM_LAB_SYSBD_MOD, labelRight);

    labelLeft = new QLabel(trs("SystemboardBootloader"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
    labelRight = new QLabel("");
    layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
    d_ptr->labs.insert(FactoryVersionInfoPrivate
                       ::ITEM_LAB_SYSBD_BOOTLOADER, labelRight);

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

    labelLeft = new QLabel(trs("ECGBootloader") + "    ");
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
    labelRight = new QLabel;
    layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
    d_ptr->labs.insert(FactoryVersionInfoPrivate
                       ::ITEM_LAB_ECG_BOOTLOADER, labelRight);

    if (systemManager.isSupport(CONFIG_NIBP))
    {
        labelLeft = new QLabel(trs("NIBPVersion") + "    ");
        layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);

        labelRight = new QLabel;
        layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
        d_ptr->labs.insert(FactoryVersionInfoPrivate
                           ::ITEM_LAB_NIBP_VERSION, labelRight);

        QString str;
        machineConfig.getStrValue("NIBP", str);
        if (str == "BLM_N5")
        {
            labelLeft = new QLabel(trs("NIBPBootloader") + "    ");
            layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);

            labelRight = new QLabel;
            layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
            d_ptr->labs.insert(FactoryVersionInfoPrivate
                               ::ITEM_LAB_NIBP_BOOTLOADER, labelRight);

            // nibp 从片
            labelLeft = new QLabel(trs("NIBPSlaveVersion") + "    ");
            layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
            d_ptr->n5SlaveLab = labelLeft;

            labelRight = new QLabel;
            layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
            d_ptr->labs.insert(FactoryVersionInfoPrivate
                               ::ITEM_LAB_NIBP_SLAVE_VERSION, labelRight);

            labelLeft = new QLabel(trs("NIBPSlaveBootloader") + "    ");
            layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
            d_ptr->n5SlaveBootloaderLab = labelLeft;

            labelRight = new QLabel;
            layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
            d_ptr->labs.insert(FactoryVersionInfoPrivate
                               ::ITEM_LAB_NIBP_SLAVE_BOOTLOADER, labelRight);
        }
    }

    if (systemManager.isSupport(CONFIG_SPO2))
    {
        labelLeft = new QLabel(trs("SPO2Version") + "    ");
        layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);

        labelRight = new QLabel;
        layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
        d_ptr->labs.insert(FactoryVersionInfoPrivate
                           ::ITEM_LAB_SPO2_VERSION, labelRight);

        QString str;
        machineConfig.getStrValue("SPO2", str);
        if (str == "BLM_S5")
        {
            labelLeft = new QLabel(trs("SPO2Bootloader") + "    ");
            layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);

            labelRight = new QLabel;
            layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
            d_ptr->labs.insert(FactoryVersionInfoPrivate
                               ::ITEM_LAB_SPO2_BOOTLOADER, labelRight);
        }
    }

    if (systemManager.isSupport(CONFIG_TEMP))
    {
        labelLeft = new QLabel(trs("TEMPVersion") + "    ");
        layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);

        labelRight = new QLabel;
        layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
        d_ptr->labs.insert(FactoryVersionInfoPrivate
                           ::ITEM_LAB_TEMP_VERSION, labelRight);

        labelLeft = new QLabel(trs("TEMPBootloader") + "    ");
        layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);

        labelRight = new QLabel;
        layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
        d_ptr->labs.insert(FactoryVersionInfoPrivate
                           ::ITEM_LAB_TEMP_BOOTLOADER, labelRight);
    }

    if (systemManager.isSupport(CONFIG_PRINTER))
    {
        labelLeft = new QLabel(trs("PRT48Version") + "    ");
        layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
        labelRight = new QLabel;
        layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
        d_ptr->labs.insert(FactoryVersionInfoPrivate
                           ::ITEM_LAB_PRT48_VERSION, labelRight);

        labelLeft = new QLabel(trs("PRT48Bootloader") + "    ");
        layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
        labelRight = new QLabel;
        layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
        d_ptr->labs.insert(FactoryVersionInfoPrivate
                           ::ITEM_LAB_PRT48_BOOTLOADER, labelRight);
    }

    // screen size
    labelLeft = new QLabel(trs("ScreenSize") + "    ");
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);

    labelRight = new QLabel;

    layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
    d_ptr->labs.insert(FactoryVersionInfoPrivate
                       ::ITEM_LAB_SCREEN_INCH, labelRight);

    // screen resolution
    labelLeft = new QLabel(trs("ScreenResolution") + "    ");
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0, Qt::AlignLeft);
    labelLeft->setFocusPolicy(Qt::StrongFocus);

    labelRight = new QLabel;

    layout->addWidget(labelRight, d_ptr->labs.count(), 1, Qt::AlignRight);
    d_ptr->labs.insert(FactoryVersionInfoPrivate
                       ::ITEM_LAB_SCREEN_RESOLUTION, labelRight);

    layout->setRowStretch(d_ptr->labs.count(), 1);
}

void FactoryVersionInfoPrivate::loadOptions()
{
    QString tmpStr;
    tmpStr.clear();
    systemConfig.getStrValue("SoftWareVersion|Uboot", tmpStr);
    labs[ITEM_LAB_U_BOOT]->setText(trs(tmpStr));

    tmpStr.clear();
    QFile file(HARDWARE_VERSION);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
         while (!in.atEnd())
         {
            QString tmp = in.readLine();
            tmp = tmp.trimmed();
            int ad = tmp.toInt();

            int quotient = ad * HW_VER_TOTAL_NUMBERS / HW_VER_SAMP_REF_VAL;
            int version = quotient + 1;
            tmpStr = "V";
            tmpStr += QString::number(version);
            break;
        }
    }
    labs[ITEM_LAB_HARDWARE_VERSION]->setText(tmpStr);

    // system board version
    Provider *p = paramManager.getProvider("SystemBoard");
    QString version;
    QString versionSuffix;
    if (p)
    {
        version = p->getVersionString();
        versionSuffix = version.section(' ', -1);
        version.remove(versionSuffix);
    }
    if (labs[ITEM_LAB_SYSBD_MOD])
    {
        labs[ITEM_LAB_SYSBD_MOD]->setText(version);
    }
    if (labs[ITEM_LAB_SYSBD_BOOTLOADER])
    {
        labs[ITEM_LAB_SYSBD_BOOTLOADER]->setText(versionSuffix);
    }

    // ecg algorithm
    tmpStr.clear();
    systemConfig.getStrValue("SoftWareVersion|ECGAlgorithmType", tmpStr);
    labs[ITEM_LAB_ECG_ALGHTP]->setText(trs(tmpStr));

    // ecg version
    QString str;
    version.clear();
    machineConfig.getStrValue("ECG", str);
    p = paramManager.getProvider(str);
    if (p)
    {
        version = p->getVersionString();
        versionSuffix = version.section(' ', -1);
        version.remove(versionSuffix);
    }
    labs[ITEM_LAB_ECG_VERSION]->setText(version);
    labs[ITEM_LAB_ECG_BOOTLOADER]->setText(versionSuffix);

    // nibp version
    str.clear();
    version.clear();
    machineConfig.getStrValue("NIBP", str);
    p = paramManager.getProvider(str);
    if (p)
    {
        version = p->getVersionString();
        versionSuffix = version.section(' ', -1);
        version.remove(versionSuffix);
    }
    if (labs[ITEM_LAB_NIBP_VERSION])
    {
        labs[ITEM_LAB_NIBP_VERSION]->setText(version);
    }
    if (labs[ITEM_LAB_NIBP_BOOTLOADER])
    {
        labs[ITEM_LAB_NIBP_BOOTLOADER]->setText(versionSuffix);
    }

    // N5 从片
    if (p)
    {
        version = p->getExtraVersionString();
        versionSuffix = version.section(' ', -1);
        version.remove(versionSuffix);
    }
    if (labs[ITEM_LAB_NIBP_SLAVE_VERSION])
    {
        if (version.isEmpty())
        {
            labs[ITEM_LAB_NIBP_SLAVE_VERSION]->hide();
            n5SlaveLab->hide();
        }
        else
        {
            n5SlaveLab->show();
            labs[ITEM_LAB_NIBP_SLAVE_VERSION]->show();
            labs[ITEM_LAB_NIBP_SLAVE_VERSION]->setText(version);
        }
    }
    if (labs[ITEM_LAB_NIBP_SLAVE_BOOTLOADER])
    {
        if (version.isEmpty())  // 如果版本号不存在，不再显示其bootloader
        {
            n5SlaveBootloaderLab->hide();
            labs[ITEM_LAB_NIBP_SLAVE_BOOTLOADER]->hide();
        }
        else
        {
            n5SlaveBootloaderLab->show();
            labs[ITEM_LAB_NIBP_SLAVE_BOOTLOADER]->show();
            labs[ITEM_LAB_NIBP_SLAVE_BOOTLOADER]->setText(versionSuffix);
        }
    }

    // spo2 version
    str.clear();
    version.clear();
    machineConfig.getStrValue("SPO2", str);
    p = paramManager.getProvider(str);
    if (p)
    {
        version = p->getVersionString();
        versionSuffix = version.section(' ', -1);
        version.remove(versionSuffix);
    }
    if (labs[ITEM_LAB_SPO2_VERSION])
    {
        labs[ITEM_LAB_SPO2_VERSION]->setText(version);
    }
    if (labs[ITEM_LAB_SPO2_BOOTLOADER])
    {
        labs[ITEM_LAB_SPO2_BOOTLOADER]->setText(versionSuffix);
    }

    // t5 version
    p = paramManager.getProvider("BLM_T5");
    version.clear();
    if (p)
    {
        version = p->getVersionString();
        versionSuffix = version.section(' ', -1);
        version.remove(versionSuffix);
    }
    if (labs[ITEM_LAB_TEMP_VERSION])
    {
        labs[ITEM_LAB_TEMP_VERSION]->setText(version);
    }
    if (labs[ITEM_LAB_TEMP_BOOTLOADER])
    {
        labs[ITEM_LAB_TEMP_BOOTLOADER]->setText(versionSuffix);
    }

    // prt48 version
    p = paramManager.getProvider("PRT48");
    version.clear();
    if (p)
    {
        version = p->getVersionString();
        versionSuffix = version.section(' ', -1);
        version.remove(versionSuffix);
    }
    if (labs[ITEM_LAB_PRT48_VERSION])
    {
        labs[ITEM_LAB_PRT48_VERSION]->setText(version);
    }
    if (labs[ITEM_LAB_PRT48_BOOTLOADER])
    {
        labs[ITEM_LAB_PRT48_BOOTLOADER]->setText(versionSuffix);
    }

    // screen size
    str.clear();
    str = QString::number(systemManager.getScreenSize(), 'f', 1);
    str += " ";
    str += trs("Inch");
    labs[ITEM_LAB_SCREEN_INCH]->setText(str);

    // screen resolution
    str.clear();
    QString width = windowManager.width();
    str += width;
    str += " x ";
    QString height = windowManager.height();
    str += height;
    labs[ITEM_LAB_SCREEN_RESOLUTION]->setText(str);
}
