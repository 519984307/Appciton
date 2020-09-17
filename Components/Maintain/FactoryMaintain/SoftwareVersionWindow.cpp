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
 * D ------------------------ 产品类别设别号。“D”表示监护设备。
 * B ------------------------ 注册单元设别号。“B”表示产品属于B注册单元；新生儿专用机器为A注册单元。
 * A ------------------------ 产品设别号。 D6、D7、D8、D6S、D7S、D8S用“A”表示。
 * 1 ------------------------ 重大增强类软件更新设别号。从“1”开始。
 * 0 ------------------------ 轻微增强类软件更新设别号。从"0”开始。
 * 0 ------------------------ 纠正类软件更新设别号。从"0”开始。
 * A ------------------------ 软件构建识别号。 “A”表示主程序,“B”表示副程序,如有多个副程序,则用“B”、“C”、“D”......分别表示。
 * CN------------------------ 语种识别号。“CN”表示中文或通用,“EN”表示英文。具体按照公司内部“软件语种识别号规定”执行。
 *
 * 详细软件版本信息查看戴维产品技术要求
 * */
#define DV_RELEASE_VERSION "DBA1"
#define DV_RELEASE_VERSION_NEO "DAA1"    // 新生儿专用机器发布版本
#define DV_SW_VERSION "DBA1.0.0.A.CN"
#define DV_SW_VERSION_NEO "DAA1.0.0.A.CN"    // 新生儿专用机器软件版本
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

    label = new QLabel(QString("%1:").arg(trs("ReleaseVersion")));
    glayout->addWidget(label, 0, 1, Qt::AlignLeft | Qt::AlignHCenter);
    label = new QLabel;
    QString softwaveVersion;
    int isNeoMachine = 0;
    machineConfig.getNumValue("NeonateMachine", isNeoMachine);
    if (isNeoMachine)
    {
        softwaveVersion = DV_RELEASE_VERSION_NEO;
    }
    else
    {
        softwaveVersion = DV_RELEASE_VERSION;
    }
    label->setText(softwaveVersion);
    glayout->addWidget(label, 0, 2, Qt::AlignLeft | Qt::AlignHCenter);

    label = new QLabel(QString("%1:").arg(trs("SoftwareVersion")));
    glayout->addWidget(label, 1, 1, Qt::AlignLeft | Qt::AlignHCenter);
    label = new QLabel;
    if (isNeoMachine)
    {
        softwaveVersion = DV_SW_VERSION_NEO;
    }
    else
    {
        softwaveVersion = DV_SW_VERSION;
    }
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
