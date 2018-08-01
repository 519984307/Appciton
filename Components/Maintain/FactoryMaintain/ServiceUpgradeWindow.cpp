/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/23
 **/
#include "ServiceUpgradeWindow.h"
#include <QMap>
#include <QTextEdit>
#include <QProgressBar>
#include "ComboBox.h"
#include "Button.h"
#include <QLabel>
#include <QGridLayout>
#include "BLMEDUpgradeParam.h"
#include "FontManager.h"
#include <QScrollBar>
#include <QTextTable>
#include "SystemManager.h"



class ServiceUpgradeWindowPrivate
{
public:
    ServiceUpgradeWindowPrivate();


    QTextEdit *testEdit;  // 文本框
    QTextCursor cur;  // 光标
    QProgressBar *progressBar;  // 进步条
    ComboBox *upgradeType;  // 下拉选框
    Button *btnStart;  // 开始按钮
    Button *btnReturn;  // 返回按钮
    QLabel *info;  // 提示信息

    int mStatusCount;  // 当前显示行数
    int mStatusAll;  // 可显示的行数
    bool upgrading;  // 升级中的标志
    bool waitPDCPLDRunCompletion;  // 等待PD CPLD运行完成
    UpgradeWindowType typl;
    QMap<int, UpgradeWindowType> upgradeModule;  // 支持升级的模块。
};

ServiceUpgradeWindowPrivate::ServiceUpgradeWindowPrivate()
    : testEdit(NULL),
      progressBar(NULL),
      upgradeType(NULL),
      btnStart(NULL),
      btnReturn(NULL),
      info(NULL),
      mStatusCount(0),
      mStatusAll(0),
      upgrading(0),
      waitPDCPLDRunCompletion(false),
      typl(UPGRADE_WINDOW_TYPE_NR)

{
    cur.clearSelection();
    upgradeModule.clear();
}

ServiceUpgradeWindow::ServiceUpgradeWindow(): Window(),
    d_ptr(new ServiceUpgradeWindowPrivate)
{
    setWindowTitle(trs("Upgrade"));

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(50, 2, 50, 2);

    QHBoxLayout *hl = new QHBoxLayout();
    QLabel *label = new QLabel(trs("UpgradeModule") + " :");
    hl->addWidget(label);

    d_ptr->upgradeType = new ComboBox;
    d_ptr->upgradeType->setFocusPolicy(Qt::StrongFocus);
    d_ptr->upgradeType->addItem(trs(convert(UPGRADE_WINDOW_TYPE_HOST)));
    d_ptr->upgradeType->addItem(trs(convert(UPGRADE_WINDOW_TYPE_TE3)));
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        d_ptr->upgradeType->addItem(trs(convert(UPGRADE_WINDOW_TYPE_TN3)));
    }
    if (systemManager.isSupport(CONFIG_SPO2))
    {
        d_ptr->upgradeType->addItem(trs(convert(UPGRADE_WINDOW_TYPE_TS3)));
    }
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        d_ptr->upgradeType->addItem(trs(convert(UPGRADE_WINDOW_TYPE_TT3)));
    }
    d_ptr->upgradeType->addItem(trs(convert(UPGRADE_WINDOW_TYPE_PRT72)));
    d_ptr->upgradeType->addItem(trs(convert(UPGRADE_WINDOW_TYPE_nPMBoard)));
    connect(d_ptr->upgradeType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(UpgradeWindowTypeReleased(int)));
    hl->addWidget(d_ptr->upgradeType);

    d_ptr->btnStart = new Button(trs("UpgradeStart"));
    d_ptr->btnStart->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->btnStart, SIGNAL(released()), this, SLOT(startBtnReleased()));
    hl->addWidget(d_ptr->btnStart);
    layout->addLayout(hl, 0, 0);

    d_ptr->testEdit = new QTextEdit();
    d_ptr->testEdit->setFocus();
    d_ptr->testEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->testEdit->setReadOnly(true);
    layout->addWidget(d_ptr->testEdit, 1, 0);

    d_ptr->progressBar = new QProgressBar();
    d_ptr->progressBar->setStyleSheet("QProgressBar{background:rgb(255,255,255);border-radius:5px;text-align: center;}"
                                      "QProgressBar::chunk{background:green;border-radius:5px;}");
    d_ptr->progressBar->setRange(0, 100);
    d_ptr->progressBar->setValue(50);
    layout->addWidget(d_ptr->progressBar, 2, 0);


    d_ptr->btnReturn = new Button(trs("UpgradeReturn"));
    d_ptr->btnReturn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(d_ptr->btnReturn, 3, 0, Qt::AlignRight);

    layout->setRowStretch(4, 1);

    d_ptr->info = new QLabel(trs("WarningNoUSB"), this);
    layout->addWidget(d_ptr->info, 5, 0, Qt::AlignBottom | Qt::AlignRight);

    setWindowLayout(layout);

    init();
}

void ServiceUpgradeWindow::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::FontChange)
    {
        setFont(fontManager.textFont(font().pixelSize()));
    }
}

void ServiceUpgradeWindow::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);
    d_ptr->btnStart->setFixedWidth(d_ptr->upgradeType->width());
}

ServiceUpgradeWindow::~ServiceUpgradeWindow()
{
    delete d_ptr;
}


void ServiceUpgradeWindow::btnShow(bool flag)
{
    d_ptr->upgrading = !flag;
    d_ptr->upgradeType->setEnabled(flag);
    d_ptr->btnStart->setEnabled(flag);
    d_ptr->btnReturn->setEnabled(flag);
    if (flag)
    {
        d_ptr->btnReturn->setFocus();
    }
}

void ServiceUpgradeWindow::progressBarValue(int value)
{
    d_ptr->progressBar->show();
    d_ptr->progressBar->setValue(value);
}

void ServiceUpgradeWindow::infoShow(bool flag)
{
    if (flag)
    {
        d_ptr->info->show();
    }
    else
    {
        d_ptr->info->hide();
    }
}

ServiceUpgradeWindow *ServiceUpgradeWindow::getInstance()
{
    static ServiceUpgradeWindow *instance = NULL;
    if (!instance)
    {
        instance = new ServiceUpgradeWindow();
    }
    return instance;
}

UpgradeWindowType ServiceUpgradeWindow::getType()
{
    return d_ptr->typl;
}

bool ServiceUpgradeWindow::isUpgrading()
{
    return d_ptr->upgrading;
}

void ServiceUpgradeWindow::exit()
{
    blmedUpgradeParam.fileClose();
    close();
}

void ServiceUpgradeWindow::init()
{
    btnShow(true);
    d_ptr->progressBar->reset();
    d_ptr->progressBar->hide();
    d_ptr->info->hide();
    d_ptr->upgradeType->setFocus();
    d_ptr->upgradeType->setCurrentIndex(0);
    d_ptr->typl = UPGRADE_WINDOW_TYPE_HOST;
    d_ptr->testEdit->clear();
    d_ptr->mStatusCount = 0;

    int textH = fontManager.textHeightInPixels(fontManager.textFont(font().pixelSize()));
    int editH = d_ptr->testEdit->height();
    d_ptr->mStatusAll = editH / (textH + 5);
}


void ServiceUpgradeWindow::setDebugText(QString str)
{
    d_ptr->cur = d_ptr->testEdit->textCursor();
    d_ptr->cur.movePosition(QTextCursor::End);
    QTextTableFormat tableFormat;
    tableFormat.setBorder(0);
    // 插入table，用于显示状态信息
    QTextTable *table = d_ptr->cur.insertTable(1, 1, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText(str);
    d_ptr->cur.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor, 1);
    d_ptr->cur.select(QTextCursor::BlockUnderCursor);
    d_ptr->cur.clearSelection();

    // 滚动条置底
    QScrollBar *bar = d_ptr->testEdit->verticalScrollBar();
    bar->setValue(bar->maximum());

    // 状态信息条数+1
    d_ptr->mStatusCount++;
    // 如果信息数>50，则删除最早的一条记录
    if (d_ptr->mStatusCount > d_ptr->mStatusAll)
    {
        // 将选择光标移至第一个字符
        d_ptr->cur.setPosition(1, QTextCursor::MoveAnchor);
        // 选择最早含有最早状态信息的table
        d_ptr->cur.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor, 1);
        d_ptr->cur.select(QTextCursor::BlockUnderCursor);
        // 删除该信息
        d_ptr->cur.removeSelectedText();
        // 光标移至QTextEdit末行
        d_ptr->cur.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        d_ptr->mStatusCount--;
    }
}

void ServiceUpgradeWindow::startBtnReleased()
{
    d_ptr->testEdit->clear();
    d_ptr->progressBar->reset();
    d_ptr->mStatusCount = 0;
    blmedUpgradeParam.startUpgrade();
}

void ServiceUpgradeWindow::UpgradeWindowTypeReleased(int index)
{
    QMap<int, UpgradeWindowType>::Iterator iter = d_ptr->upgradeModule.find(index);
    if (iter != d_ptr->upgradeModule.end())
    {
        d_ptr->typl = iter.value();
    }
    else
    {
        d_ptr->typl = UPGRADE_WINDOW_TYPE_NR;
    }
}


void ServiceUpgradeWindow::setWaitPDCPLDRunCompletion(bool flag)
{
    d_ptr->waitPDCPLDRunCompletion = flag;
}

bool ServiceUpgradeWindow::isWaitPDCPLDRunCompletion()
{
    return d_ptr->waitPDCPLDRunCompletion;
}













































