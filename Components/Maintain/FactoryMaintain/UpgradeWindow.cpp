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
#include <QPlainTextEdit>
#include <QLabel>
#include <QProgressBar>
#include "SystemManager.h"
#include "WindowManager.h"
#include "MessageBox.h"
#include "LanguageManager.h"
#include <QProcess>
#include "SystemBoardProvider.h"

class UpgradeWindowPrivate
{
public:
    UpgradeWindowPrivate()
        : upgradeModuleCbo(NULL),
          startBtn(NULL),
          textEdit(NULL),
          progressBar(NULL),
          info(NULL),
          upgradeModule(UpgradeManager::UPGRADE_MOD_NONE),
          isUpdate(false)
    {}

    ComboBox *upgradeModuleCbo;
    Button *startBtn;
    QPlainTextEdit *textEdit;
    QProgressBar *progressBar;
    QLabel *info;
    UpgradeManager::UpgradeModuleType upgradeModule;
    bool isUpdate;
};

UpgradeWindow::UpgradeWindow()
    : Dialog(), d_ptr(new UpgradeWindowPrivate())
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
        trs(UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_LOGO)));

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
        d_ptr->upgradeModuleCbo->addItem(
            trs(UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_N5DAEMON)));
    }

    if (systemManager.isSupport(CONFIG_TEMP))
    {
        d_ptr->upgradeModuleCbo->addItem(
            trs(UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_T5)));
    }

    if (systemManager.isSupport(CONFIG_CO2))
    {
        d_ptr->upgradeModuleCbo->addItem(
            trs(UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_CO2)));
    }

#ifdef ENABLE_O2_APNEASTIMULATION
    if (systemManager.isSupport(CONFIG_O2))
    {
        d_ptr->upgradeModuleCbo->addItem(
            UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_NEONATE));
    }
#endif

    if (systemManager.isSupport(CONFIG_PRINTER))
    {
        d_ptr->upgradeModuleCbo->addItem(
                    trs(UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_PRT48)));
    }

    d_ptr->upgradeModuleCbo->addItem(
        trs(UpgradeManager::getUpgradeModuleName(UpgradeManager::UPGRADE_MOD_nPMBoard)));
    hLayout->addWidget(d_ptr->upgradeModuleCbo, 1);

    hLayout->addStretch(1);

    d_ptr->startBtn = new Button(trs("UpgradeStart"));
    d_ptr->startBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->startBtn, SIGNAL(clicked(bool)), this, SLOT(onStartBtnClick()));
    hLayout->addWidget(d_ptr->startBtn, 1);

    layout->addLayout(hLayout);

    d_ptr->textEdit = new QPlainTextEdit();
    d_ptr->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->textEdit->setReadOnly(true);
    d_ptr->textEdit->setFrameStyle(QFrame::NoFrame);
    d_ptr->textEdit->setFocusPolicy(Qt::NoFocus);
    layout->addWidget(d_ptr->textEdit, 1);

    d_ptr->progressBar = new QProgressBar();
    d_ptr->progressBar->setStyleSheet("QProgressBar{background:rgb(255,255,255);border-radius:5px;text-align: center;}"
                                      "QProgressBar::chunk{background:green;border-radius:5px;}");
    d_ptr->progressBar->setRange(0, 100);
    d_ptr->progressBar->setValue(0);
    d_ptr->progressBar->setTextVisible(false);
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
    connect(UpgradeManager::getInstance(), SIGNAL(reboot()),
            this, SLOT(accept()));

    resize(600, 480);

    startTimer(500);
}

UpgradeWindow::~UpgradeWindow()
{
    delete d_ptr;
}

void UpgradeWindow::exec()
{
    windowManager.closeAllWidows();
    windowManager.setVisible(false);
    QRect r = windowManager.geometry();
    this->move(r.center() - this->rect().center());
    if (QDialog::Rejected == QDialog::exec())
    {
        windowManager.setVisible(true);
    }
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

void UpgradeWindow::hideEvent(QHideEvent *ev)
{
    if (d_ptr->isUpdate)
    {
        d_ptr->isUpdate = false;
        QString hints = trs("MachineConfigIsUpdatedNow");
        hints += "\n";
        hints += trs("IsRebootMachine");
        hints += "?";
        MessageBox box(trs("UpdateTips"), hints);
        QDialog::DialogCode statue = static_cast<QDialog::DialogCode>(box.exec());
        if (statue == QDialog::Accepted)
        {
            systemBoardProvider.requestReset();
        }
    }
    Dialog::hideEvent(ev);
}

void UpgradeWindow::upgradeMessageUpdate(const QString &msg)
{
    d_ptr->textEdit->appendPlainText(msg);
}

void UpgradeWindow::upgradeProgressChanged(int value)
{
    if (!d_ptr->progressBar->isVisible())
    {
        d_ptr->progressBar->setVisible(true);
    }

    d_ptr->progressBar->setValue(value);
}

void UpgradeWindow::onUpgradeFinished(UpgradeManager::UpgradeResult result)
{
    if (result == UpgradeManager::UPGRADE_SUCCESS)
    {
        d_ptr->isUpdate = true;
        d_ptr->textEdit->appendPlainText(trs("UpgradeSuccess"));
        getCloseBtn()->setEnabled(true);
    }
    else if (result == UpgradeManager::UPGRADE_REBOOT)
    {
        d_ptr->isUpdate = false;        // 不需要用户进行判断进行重启
        d_ptr->textEdit->appendPlainText(trs("SystemWillRestartPleaseDoNotPowerOff"));
    }
    else
    {
        d_ptr->textEdit->appendPlainText(trs("UpgradeFailed"));
        getCloseBtn()->setEnabled(true);
        d_ptr->progressBar->setValue(0);
    }

    d_ptr->upgradeModuleCbo->setEnabled(true);
    d_ptr->startBtn->setEnabled(true);
    d_ptr->upgradeModule = UpgradeManager::UPGRADE_MOD_NONE;
}

void UpgradeWindow::onStartBtnClick()
{
    if (!d_ptr->info->text().isEmpty())
    {
        // have some warning info, do nothing
        return;
    }

    QString moduleName = d_ptr->upgradeModuleCbo->currentText();
    UpgradeManager::UpgradeModuleType module;
    for (int i = UpgradeManager::UPGRADE_MOD_HOST; i < UpgradeManager::UPGRADE_MOD_TYPE_NR; i++)
    {
        module = static_cast<UpgradeManager::UpgradeModuleType>(i);
        if (moduleName == trs(UpgradeManager::getUpgradeModuleName(module)))
        {
            if (module == UpgradeManager::UPGRADE_MOD_HOST)
            {
                QStringList slist;
                slist << trs("No") << trs("Ok");
                MessageBox messageBox(trs("Warn"), trs("SureAllDataErase"), slist, true);
                if (messageBox.exec())
                {
                    d_ptr->upgradeModule = module;
                }
                else
                {
                    return;
                }
            }
            else
            {
                d_ptr->upgradeModule = module;
            }
            break;
        }
    }

    if (d_ptr->upgradeModule == UpgradeManager::UPGRADE_MOD_NONE)
    {
        // no found the correspond module
        return;
    }

    d_ptr->textEdit->clear();
    getCloseBtn()->setEnabled(false);
    d_ptr->upgradeModuleCbo->setEnabled(false);
    d_ptr->startBtn->setEnabled(false);
    d_ptr->progressBar->setValue(0);

    UpgradeManager::getInstance()->startModuleUpgrade(module);
}
