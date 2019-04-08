/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/


#include "ServiceVersion.h"
#include <QVBoxLayout>
#include "SystemBoardProvider.h"
#include "SystemManager.h"
#include <QTextTable>
#include <QScrollBar>
#include "Debug.h"
#include "ParamManager.h"
#include "IConfig.h"
#include "MenuManager.h"
#include "LanguageManager.h"
#include "FontManager.h"

ServiceVersion *ServiceVersion::_selfObj = NULL;

/**************************************************************************************************
 * 功能：初始化。
 *************************************************************************************************/
void ServiceVersion::init()
{
    paramManager.getVersion();
    systemBoardProvider.sendVersion();
    QString _nPMstr;
    systemManager.getSoftwareVersion(_nPMstr);
    _nPM->setText(_nPMstr);

   _coreVersion();
}

/**************************************************************************************************
 * 功能：ECG版本。
 *************************************************************************************************/
void ServiceVersion::getECGVersion(unsigned char *data, int len)
{
    _ECGVersion.clear();

    if (len < minLen)
    {
        return;
    }

    char *p = reinterpret_cast<char *>(data + 1);
    const char *ptr = const_cast<const char *>(p);

//    _ECGVersion += QString(P);
//    _ECGVersion += "-";
//    _ECGVersion += QString(P + 16);
//    _ECGVersion += ",\r\r\r\r";
//    _ECGVersion += QString(P + 32);
//    _ECGVersion += "\r\r";
//    _ECGVersion += QString(P + 48);

    _ECGVersion = QString("SW(%1.%2,    %3  %4), HW(%5)")
            .arg(QString(ptr))
            .arg(QString(ptr + 16))
            .arg(QString(ptr + 32))
            .arg(QString(ptr + 48))
            .arg(QString(ptr + 64));
    _ECG->setText(_ECGVersion);
}

/**************************************************************************************************
 * 功能：NIBP版本。
 *************************************************************************************************/
void ServiceVersion::getNIBPVersion(unsigned char *data, int len)
{
    _NIBPVersion.clear();

    if (len < (minLen + minLen - 1))
    {
        return;
    }
    char *p = reinterpret_cast<char *>(data + 1);
    const char *ptr = const_cast<const char *>(p);

    _NIBPVersion = QString("SW(%1.%2,    %3  %4), HW(%5)")
            .arg(QString(ptr))
            .arg(QString(ptr + 16))
            .arg(QString(ptr + 32))
            .arg(QString(ptr + 48))
            .arg(QString(ptr + 64));

    _NIBP->setText(_NIBPVersion);
}

/**************************************************************************************************
 * 功能：SPO2版本。
 *************************************************************************************************/
void ServiceVersion::getSPO2Version(unsigned char *data, int len)
{
    _SPO2Version.clear();

    if (len < minLen)
    {
        return;
    }

    char *p = reinterpret_cast<char *>(data + 1);
    const char *ptr = const_cast<const char *>(p);

    _SPO2Version = QString("SW(%1.%2,    %3  %4)")
            .arg(QString(ptr))
            .arg(QString(ptr + 16))
            .arg(QString(ptr + 32))
            .arg(QString(ptr + 48));

    _SPO2->setText(_SPO2Version);
}

/**************************************************************************************************
 * 功能：TEMP版本。
 *************************************************************************************************/
void ServiceVersion::getTEMPVersion(unsigned char *data, int len)
{
    _TEMPVersion.clear();

    if (len < minLen)
    {
        return;
    }

    char *p = reinterpret_cast<char *>(data + 1);
    const char *ptr = const_cast<const char *>(p);

//    _TEMPVersion += QString(P);
//    _TEMPVersion += "-";
//    _TEMPVersion += QString(P + 16);
//    _TEMPVersion += ",\r\r\r";
//    _TEMPVersion += QString(P + 32);
//    _TEMPVersion += "\r\r";
//    _TEMPVersion += QString(P + 48);
    _TEMPVersion = QString("SW(%1.%2,    %3  %4), HW(%5)")
            .arg(QString(ptr))
            .arg(QString(ptr + 16))
            .arg(QString(ptr + 32))
            .arg(QString(ptr + 48))
            .arg(QString(ptr + 64));

    _TEMP->setText(_TEMPVersion);
}


/**************************************************************************************************
 * 功能：nPMBoard版本。
 *************************************************************************************************/
void ServiceVersion::getnPMBoardVersion(unsigned char *data, int len)
{
    _nPMBoardVersion.clear();

    if (len < minLen)
    {
        return;
    }
    char *p = reinterpret_cast<char *>(data + 1);
    const char *ptr = const_cast<const char *>(p);

    _nPMBoardVersion = QString("SW(%1.%2,    %3  %4)")
            .arg(QString(ptr))
            .arg(QString(ptr + 16))
            .arg(QString(ptr + 32))
            .arg(QString(ptr + 48));

    _nPMBoard->setText(_nPMBoardVersion);
}

/**************************************************************************************************
 * 功能：打印机版本。
 *************************************************************************************************/
void ServiceVersion::getPRT72Version(unsigned char *data, int len)
{
    _PRT72Version.clear();

    if (len < minLen)
    {
        return;
    }
    char *p = reinterpret_cast<char *>(data + 1);
    const char *ptr = const_cast<const char *>(p);

    _PRT72Version = QString("SW(%1.%2,    %3  %4), HW(%5)")
            .arg(QString(ptr))
            .arg(QString(ptr + 16))
            .arg(QString(ptr + 32))
            .arg(QString(ptr + 48))
            .arg(QString(ptr + 64));
    _PRT72->setText(_PRT72Version);
}


/**************************************************************************************************
 * 功能：内核版本。
 *************************************************************************************************/
void ServiceVersion::_coreVersion(void)
{
    QFile file("/proc/version");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QString fileStr = file.readAll();
    debug("%s", qPrintable(fileStr));
    file.close();

    QString str;
    int x = fileStr.indexOf("Linux version");
    int y = fileStr.indexOf(" (");
    str = "Linux :";
    str += fileStr.mid(x+13, y-x-13);
    str += ",\r\r\r";

    x = fileStr.indexOf("gcc version");
    y = fileStr.indexOf("(GCC)");
    str += "gcc :";
    str += fileStr.mid(x+11, y-x-12);
    str += ",\r\r\r";

    x = fileStr.indexOf("SMP ");
    y = fileStr.indexOf(" CST");
    str += fileStr.mid(x+8, y-x-8);
    str += "\r\r";

    x = fileStr.indexOf("CST ");
    str += fileStr.mid(x+4, 4);

    if (QFile::exists("/etc/rcS.d/S40networking"))
    {
        str += ", Ethernet";
    }

    _CORE->setText(str);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ServiceVersion::ServiceVersion() : MenuWidget(trs("ServiceVersion"))
{
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setFixedSize(submenuW, submenuH);

    int fontSize = fontManager.getFontSize(1);
    int btnWidth = submenuW - 100;

    QVBoxLayout *labelLayout = new QVBoxLayout();
    labelLayout->setContentsMargins(50, 0, 50, 0);
    labelLayout->setSpacing(5);
    labelLayout->setAlignment(Qt::AlignTop);

    _nPM = new LabeledLabel(trs("nPMVersion") + ":   ", "");
    _nPM->setFont(fontManager.textFont(fontSize));
    _nPM->setFixedSize(btnWidth, ITEM_H);
    labelLayout->addWidget(_nPM, 0, Qt::AlignCenter);

    _ECG = new LabeledLabel(trs("ECGVersion") + ":   ", "");
    _ECG->setFont(fontManager.textFont(fontSize));
    _ECG->setFixedSize(btnWidth, ITEM_H);
    labelLayout->addWidget(_ECG, 0, Qt::AlignCenter);

    _NIBP = new LabeledLabel(trs("NIBPVersion") + ":   ", "");
    _NIBP->setFont(fontManager.textFont(fontSize));
    _NIBP->setFixedSize(btnWidth, ITEM_H);
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        labelLayout->addWidget(_NIBP, 0, Qt::AlignCenter);
    }

    _SPO2 = new LabeledLabel(trs("SPO2Version") + ":   ", "");
    _SPO2->setFont(fontManager.textFont(fontSize));
    _SPO2->setFixedSize(btnWidth, ITEM_H);
    if (systemManager.isSupport(CONFIG_SPO2))
    {
        labelLayout->addWidget(_SPO2, 0, Qt::AlignCenter);
    }
    QString str;
    machineConfig.getStrValue("SPO2", str);
    if (str != "BLM_TS3")
    {
        _SPO2->setVisible(false);
    }

    _TEMP = new LabeledLabel(trs("TEMPVersion") + ":   ", "");
    _TEMP->setFont(fontManager.textFont(fontSize));
    _TEMP->setFixedSize(btnWidth, ITEM_H);
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        labelLayout->addWidget(_TEMP, 0, Qt::AlignCenter);
    }

    _nPMBoard = new LabeledLabel(trs("iDMBoardVersion") + ":   ", "");
    _nPMBoard->setFont(fontManager.textFont(fontSize));
    _nPMBoard->setFixedSize(btnWidth, ITEM_H);
    labelLayout->addWidget(_nPMBoard, 0, Qt::AlignCenter);

    _PRT72 = new LabeledLabel(trs("PRT72Version") + ":   ", "");
    _PRT72->setFont(fontManager.textFont(fontSize));
    _PRT72->setFixedSize(btnWidth, ITEM_H);
    labelLayout->addWidget(_PRT72, 0, Qt::AlignCenter);

    _CORE = new LabeledLabel(trs("COREVersion") + ":   ", "");
    _CORE->setFont(fontManager.textFont(fontSize));
    _CORE->setFixedSize(btnWidth, ITEM_H);
    labelLayout->addWidget(_CORE, 0, Qt::AlignCenter);

    labelLayout->addStretch();

    labelLayout->setSpacing(5);

    mainLayout->addLayout(labelLayout);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ServiceVersion::~ServiceVersion()
{
}



