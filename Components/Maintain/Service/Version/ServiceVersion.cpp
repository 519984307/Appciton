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

    if(len < minLen)
    {
        return;
    }

    const char *P = (const char*)(char*)(data + 1);

//    _ECGVersion += QString(P);
//    _ECGVersion += "-";
//    _ECGVersion += QString(P + 16);
//    _ECGVersion += ",\r\r\r\r";
//    _ECGVersion += QString(P + 32);
//    _ECGVersion += "\r\r";
//    _ECGVersion += QString(P + 48);

    _ECGVersion = QString("SW(%1.%2,    %3  %4), HW(%5)")
            .arg(QString(P))
            .arg(QString(P + 16))
            .arg(QString(P + 32))
            .arg(QString(P + 48))
            .arg(QString(P + 64));
    _ECG->setText(_ECGVersion);
}

/**************************************************************************************************
 * 功能：NIBP版本。
 *************************************************************************************************/
void ServiceVersion::getNIBPVersion(unsigned char *data, int len)
{
    _NIBPVersion.clear();
    _NIBPDaemonVersion.clear();

    if(len < (minLen + minLen - 1))
    {
        return;
    }
    const char *P = (const char*)(char*)(data + 1);

    _NIBPVersion = QString("SW(%1.%2,    %3  %4), HW(%5)")
            .arg(QString(P))
            .arg(QString(P + 16))
            .arg(QString(P + 32))
            .arg(QString(P + 48))
            .arg(QString(P + 64));

    _NIBP->setText(_NIBPVersion);

    _NIBPDaemonVersion = QString("SW(%1.%2,    %3  %4)")
            .arg(QString(P + 72))
            .arg(QString(P + 88))
            .arg(QString(P + 104))
            .arg(QString(P + 120));

    _NIBPDaemon->setText(_NIBPDaemonVersion);
}

/**************************************************************************************************
 * 功能：SPO2版本。
 *************************************************************************************************/
void ServiceVersion::getSPO2Version(unsigned char *data, int len)
{
    _SPO2Version.clear();

    if(len < minLen)
    {
        return;
    }
    const char *P = (const char*)(char*)(data + 1);

    _SPO2Version = QString("SW(%1.%2,    %3  %4)")
            .arg(QString(P))
            .arg(QString(P + 16))
            .arg(QString(P + 32))
            .arg(QString(P + 48));

    _SPO2->setText(_SPO2Version);
}

/**************************************************************************************************
 * 功能：TEMP版本。
 *************************************************************************************************/
void ServiceVersion::getTEMPVersion(unsigned char *data, int len)
{
    _TEMPVersion.clear();

    if(len < minLen)
    {
        return;
    }
    const char *P = (const char*)(char*)(data + 1);

//    _TEMPVersion += QString(P);
//    _TEMPVersion += "-";
//    _TEMPVersion += QString(P + 16);
//    _TEMPVersion += ",\r\r\r";
//    _TEMPVersion += QString(P + 32);
//    _TEMPVersion += "\r\r";
//    _TEMPVersion += QString(P + 48);
    _TEMPVersion = QString("SW(%1.%2,    %3  %4), HW(%5)")
            .arg(QString(P))
            .arg(QString(P + 16))
            .arg(QString(P + 32))
            .arg(QString(P + 48))
            .arg(QString(P + 64));

    _TEMP->setText(_TEMPVersion);
}

/**************************************************************************************************
 * 功能：nPMBoard版本。
 *************************************************************************************************/
void ServiceVersion::getnPMBoardVersion(unsigned char *data, int len)
{
    _nPMBoardVersion.clear();

    if(len < minLen)
    {
        return;
    }
    const char *P = (const char*)(char*)(data + 1);

    _nPMBoardVersion = QString("SW(%1.%2,    %3  %4)")
            .arg(QString(P))
            .arg(QString(P + 16))
            .arg(QString(P + 32))
            .arg(QString(P + 48));

    _nPMBoard->setText(_nPMBoardVersion);
}

/**************************************************************************************************
 * 功能：打印机版本。
 *************************************************************************************************/
void ServiceVersion::getPRT72Version(unsigned char *data, int len)
{
    _PRT72Version.clear();

    if(len < minLen)
    {
        return;
    }
    const char *P = (const char*)(char*)(data + 1);

    _PRT72Version = QString("SW(%1.%2,    %3  %4), HW(%5)")
            .arg(QString(P))
            .arg(QString(P + 16))
            .arg(QString(P + 32))
            .arg(QString(P + 48))
            .arg(QString(P + 64));
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
    debug("%s",qPrintable(fileStr));
    file.close();

    QString Str;
    int x = fileStr.indexOf("Linux version");
    int y = fileStr.indexOf(" (");
    Str = "Linux :";
    Str += fileStr.mid(x+13,y-x-13);
    Str += ",\r\r\r";

    x = fileStr.indexOf("gcc version");
    y = fileStr.indexOf("(GCC)");
    Str += "gcc :";
    Str += fileStr.mid(x+11,y-x-12);
    Str += ",\r\r\r";

    x = fileStr.indexOf("SMP ");
    y = fileStr.indexOf(" CST");
    Str += fileStr.mid(x+8,y-x-8);
    Str += "\r\r";

    x = fileStr.indexOf("CST ");
    Str += fileStr.mid(x+4,4);

    if(QFile::exists("/etc/rcS.d/S40networking"))
    {
        Str += ", Ethernet";
    }

    _CORE->setText(Str);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ServiceVersion::ServiceVersion() : QWidget()
{
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = 500;

    QPalette p;
    p.setColor(QPalette::Foreground, Qt::black);

    QVBoxLayout *labelLayout = new QVBoxLayout();
    labelLayout->setContentsMargins(50, 0, 50, 20);
    labelLayout->setSpacing(2);
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

    _NIBPDaemon = new LabeledLabel(trs("NIBPDaemonVersion") + ":   ", "");
    _NIBPDaemon->setFont(fontManager.textFont(fontSize));
    _NIBPDaemon->setFixedSize(btnWidth, ITEM_H);
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        labelLayout->addWidget(_NIBPDaemon, 0, Qt::AlignCenter);
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

    _nPMBoard = new LabeledLabel(trs("nPMBoardVersion") + ":   ", "");
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

    setLayout(labelLayout);

    p.setColor(QPalette::Window, QColor(209, 203, 183));
    setPalette(p);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ServiceVersion::~ServiceVersion()
{

}



