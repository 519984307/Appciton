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
#include "Framework/Language/LanguageManager.h"
#include "IConfig.h"

/*
 * DV SW完整版本号命名规则
 * DV SW Version: DBA1.0.0.A.CN
 * D ------------------------ 产品类别识别号。“D”表示监护设备。
 * B ------------------------ 注册单元识别号。“B”表示产品属于B注册单元；新生儿专用机器为A注册单元。
 * A ------------------------ 产品识别号。 D6、D7、D8、D6S、D7S、D8S用“A”表示。
 * 1 ------------------------ 重大增强类软件更新识别号。从“1”开始。
 * 0 ------------------------ 轻微增强类软件更新识别号。从"0”开始。
 * 0 ------------------------ 纠正类软件更新识别号。从"0”开始。
 * A ------------------------ 软件构建识别号。 “A”表示主程序,“B”表示副程序,如有多个副程序,则用“B”、“C”、“D”......分别表示。
 * CN------------------------ 语种识别号。“CN”表示中文或通用,“EN”表示英文。具体按照公司内部“软件语种识别号规定”执行。
 *
 * 详细软件版本信息查看戴维产品技术要求
 * */

SoftwareVersionWindow::SoftwareVersionWindow(): Dialog()
{
    layoutExec();
}

void SoftwareVersionWindow::layoutExec()
{
    setWindowTitle(trs("SoftwareVersionMenu"));

    QGridLayout *glayout = new QGridLayout;

    QLabel *label = new QLabel;
#if 0
    QPixmap picSrc = QPixmap("/usr/local/nPM/icons/betterLife.png");
    label->setPixmap(picSrc);
#else
    QPixmap picSrc = QPixmap("/usr/local/nPM/icons/David.png");
    label->setPixmap(picSrc.scaled(picSrc.width() / 2.5, picSrc.height() / 2.5, Qt::KeepAspectRatio));
#endif
    glayout->addWidget(label, 0, 0, 3, 1, Qt::AlignCenter);
    glayout->setColumnStretch(0, 1);

    QString softwareVersion = getSoftwareVersion();
    QString releaseVersion = softwareVersion.section(".", 0, 0);   // Release Version: DBA1

    // Release Version
    label = new QLabel(QString("%1:").arg(trs("ReleaseVersion")));
    glayout->addWidget(label, 0, 1, Qt::AlignLeft | Qt::AlignHCenter);
    label = new QLabel;
    label->setText(releaseVersion);
    glayout->addWidget(label, 0, 2, Qt::AlignLeft | Qt::AlignHCenter);

    // Software Version
    label = new QLabel(QString("%1:").arg(trs("SoftwareVersion")));
    glayout->addWidget(label, 1, 1, Qt::AlignLeft | Qt::AlignHCenter);
    label = new QLabel;
    label->setText(softwareVersion);
    glayout->addWidget(label, 1, 2, Qt::AlignLeft | Qt::AlignHCenter);

    // Compile Time
    label = new QLabel(QString("%1:").arg(trs("CompileTime")));
    glayout->addWidget(label, 2, 1, Qt::AlignLeft | Qt::AlignHCenter);
    label = new QLabel;
    label->setText(QString("%1 %2").arg(__TIME__).arg(__DATE__));
    glayout->addWidget(label, 2, 2, Qt::AlignLeft | Qt::AlignHCenter);

    setWindowLayout(glayout);

    setFixedSize(600, 240);
}

QString SoftwareVersionWindow::getSoftwareVersion()
{
    QString softwareVersion;    // Software Version
    QString verStr;
    int isNeoMachine = 0;       // Neonate Machine status
    machineConfig.getNumValue("NeonateMachine", isNeoMachine);
    // Get Software Model ID
    if (isNeoMachine)
    {
        machineConfig.getStrValue("SoftwareVersion|NeoSoftwareModel", verStr);     // Version Info: DA
    }
    else
    {
        machineConfig.getStrValue("SoftwareVersion|SoftwareModel", verStr);        // Version Info: DB
    }
    softwareVersion = verStr;                          // SW Version: DB or DA

    // Get Product ID
    verStr.clear();
    machineConfig.getStrValue("SoftwareVersion|ProductID", verStr);     // Version Info:A
    softwareVersion += verStr;                         // SW Version: DBA or DAA

    // Get git Version
    QString gitVersion = QString(GIT_VERSION);         // git version: V1.0.2.****-DV
    gitVersion = gitVersion.section(".", 0, 2, QString::SectionIncludeTrailingSep);  // V1.0.2.
    gitVersion.remove("V");                            // 1.0.2.
    softwareVersion += gitVersion;                     // SW Version: DBA1.0.2.  or ..

    // Get Software Bulid ID
    verStr.clear();
    machineConfig.getStrValue("SoftwareVersion|SoftwareBulidID", verStr);     // A
    softwareVersion += verStr;                         //  SW Version: DBA1.0.2.A

    // Get Language ID
    verStr.clear();
    machineConfig.getStrValue("SoftwareVersion|LanguageID", verStr);     // CN
    softwareVersion += "." + verStr;                   //  SW Version: DBA1.0.2.A.CN

    return softwareVersion;
}
