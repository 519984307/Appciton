/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/
#include "SelectStarterLogoContent.h"
#include <QMap>
#include <QHBoxLayout>
#include <QLabel>
#include <ComboBox.h>
#include "IConfig.h"
#include <QDir>
#include <QProcess>

#define usb0DeviceNode     ("/dev/sda2")
#define selectLogoPath     ("/mnt/usb0/logo")
class SelectStarterLogoContentPrivate
{
public:
    SelectStarterLogoContentPrivate();

    /**
     * @brief loadOptions
     */
    void loadOptions();

    ComboBox *combo;
    QStringList logoNames;
    int rtnFlag;
};

SelectStarterLogoContentPrivate::SelectStarterLogoContentPrivate():
    combo(NULL),
    rtnFlag(1)
{
    logoNames.clear();
}

void SelectStarterLogoContentPrivate::loadOptions()
{
    // read the logo in the usb
    if (rtnFlag)
    {
        rtnFlag = QProcess::execute(QString("mount -t vfat %1 %2").arg(usb0DeviceNode).arg(selectLogoPath));
        QProcess::execute("sync");
    }

    // mount the usb success
    if (!rtnFlag)
    {
        QDir dir(QString("%1%2").arg(selectLogoPath).arg("/"));
        QStringList nameFilter;
        nameFilter.append("*.bmp");
        logoNames = dir.entryList(nameFilter, QDir::Files | QDir::Readable, QDir::Name);
    }

    QDir dir(QString("%1%2").arg(selectLogoPath).arg("/"));
    QStringList nameFilter;
    nameFilter.append("*.bmp");
    logoNames = dir.entryList(nameFilter, QDir::Files | QDir::Readable, QDir::Name);

    combo->blockSignals(true);
    combo->clear();
    combo->addItems(logoNames);
    if (logoNames.isEmpty())
    {
        combo->addItem("NULL");
    }
    int index = 0;
    systemConfig.getNumValue("SelectStarterLogo", index);
    combo->setCurrentIndex(index);
    combo->blockSignals(false);
}

SelectStarterLogoContent::SelectStarterLogoContent()
    : MenuContent(trs("SelectStarterLogoMenu"),
                  trs("SelectStarterLogoMenuDesc")),
      d_ptr(new SelectStarterLogoContentPrivate)
{
}

SelectStarterLogoContent::~SelectStarterLogoContent()
{
    delete d_ptr;
}

void SelectStarterLogoContent::readyShow()
{
    d_ptr->loadOptions();
}

void SelectStarterLogoContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(10);

    QLabel *label = new QLabel(trs("SelectStarterLogo"));
    layout->addWidget(label);
    ComboBox *combo = new ComboBox;
    layout->addWidget(combo);
    d_ptr->combo = combo;
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    layout->setAlignment(Qt::AlignTop);
}

void SelectStarterLogoContent::onComboBoxIndexChanged(int index)
{
    systemConfig.setNumValue("SelectStarterLogo", index);
}




















