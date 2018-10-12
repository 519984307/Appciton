/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/11
 **/

#include "UpgradeWindow.h"
#include "UpgradeManager.h"
#include "USBManager.h"
#include "ComboBox.h"
#include "Button.h"
#include <QBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QProgressBar>
#include "SystemManager.h"

class UpgradeWindowPrivate
{
public:
    UpgradeWindowPrivate()
        : upgradeModuleCbo(NULL),
          startBtn(NULL),
          textEdit(NULL),
          progressBar(NULL),
          info(NULL)
    {}

    ComboBox *upgradeModuleCbo;
    Button *startBtn;
    QTextEdit *textEdit;
    QProgressBar *progressBar;
    QLabel *info;
};

UpgradeWindow::UpgradeWindow()
    : Window(), d_ptr(new UpgradeWindowPrivate())
{
    setWindowTitle(trs("Upgrade"));

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;
    QLabel *label = new QLabel(trs("UpgradeModule") + ":");
    hLayout->addWidget(label, 1);

    d_ptr->upgradeModuleCbo = new ComboBox;
    d_ptr->upgradeModuleCbo->addItem(
        trs(UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_HOST)));
    d_ptr->upgradeModuleCbo->addItem(
        trs(UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_E5)));

    if (systemManager.isSupport(CONFIG_SPO2))
    {
        d_ptr->upgradeModuleCbo->addItem(
            trs(UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_S5)));
    }

    if (systemManager.isSupport(CONFIG_NIBP))
    {
        d_ptr->upgradeModuleCbo->addItem(
            trs(UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_N5)));
    }

    if (systemManager.isSupport(CONFIG_TEMP))
    {
        d_ptr->upgradeModuleCbo->addItem(
            trs(UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_T5)));
    }

    d_ptr->upgradeModuleCbo->addItem(
        trs(UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_PRT48)));

    d_ptr->upgradeModuleCbo->addItem(
        trs(UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_nPMBoard)));
    hLayout->addWidget(d_ptr->upgradeModuleCbo, 1);

    hLayout->addStretch(1);

    d_ptr->startBtn = new Button(trs("UpgradeStart"));
    d_ptr->startBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->startBtn, SIGNAL(clicked(bool)), this, SLOT(onStartBtnClick()));
    hLayout->addWidget(d_ptr->startBtn, 1);

    layout->addLayout(hLayout);

    d_ptr->textEdit = new QTextEdit();
    d_ptr->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->textEdit->setReadOnly(true);
    layout->addWidget(d_ptr->textEdit, 1);

    d_ptr->progressBar = new QProgressBar();
    d_ptr->progressBar->setStyleSheet("QProgressBar{background:rgb(255,255,255);border-radius:5px;text-align: center;}"
                                      "QProgressBar::chunk{background:green;border-radius:5px;}");
    d_ptr->progressBar->setRange(0, 100);
    d_ptr->progressBar->setValue(50);
    layout->addWidget(d_ptr->progressBar);

    d_ptr->info = new QLabel();
    layout->addWidget(d_ptr->info);

    setWindowLayout(layout);

    connect(UpgradeManager::getInstance(), SIGNAL(upgradeInfoChanged(QString)),
            this, SLOT(upgradeMessageUpdate(QString)));
    connect(UpgradeManager::getInstance(), SIGNAL(upgradeProgressChanged(int)),
            this, SLOT(upgradeProgressChanged(int)));
    connect(UpgradeManager::getInstance(), SIGNAL(upgradeResult(UpgradeManager::UpgradeResult)),
            this, SLOT(onUpgradeFinished(UpgradeManager::UpgradeResult)));

    resize(600, 480);

    startTimer(500);
}

UpgradeWindow::~UpgradeWindow()
{
    delete d_ptr;
}

void UpgradeWindow::timerEvent(QTimerEvent *ev)
{
    Q_UNUSED(ev);

    if (usbManager.isUSBExist())
    {
        d_ptr->info->setText(QString());
    }
    else
    {
        d_ptr->info->setText(trs("WarningNoUSB"));
    }
}

void UpgradeWindow::upgradeMessageUpdate(const QString &msg)
{
}

void UpgradeWindow::upgradeProgressChanged(int value)
{
}

void UpgradeWindow::onUpgradeFinished(UpgradeManager::UpgradeResult result)
{
}

void UpgradeWindow::onStartBtnClick()
{
}
