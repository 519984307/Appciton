/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <fangtongzhou@blmed.cn>, 2019/5/29
 **/

#include "ServiceErrorLogMenu.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include "FontManager.h"
#include "LanguageManager.h"
#include "LabelButton.h"
#include "IComboList.h"
#include "IButton.h"
#include "ITableWidget.h"
#include "Debug.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"
#include "ErrorLogViewer.h"
#include "ExportDataWidget.h"
#include "IMessageBox.h"
#if 0
#include "UdiskManager.h"
#else
#include "USBManager.h"
#endif
#include "TableItemDelegate.h"
#include <QTextStream>
#include "IConfig.h"
#include "MenuManager.h"

#define ROW_NUM  10
#define COLUMN_NUM  2

ServiceErrorLogMenu *ServiceErrorLogMenu::_selfObj = NULL;
/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ServiceErrorLogMenu::ServiceErrorLogMenu() : MenuWidget(trs("ErrorLogInfo"))
{
    currentPage = 0;
    totalPage = 0;

    int itemW = getSubmenuWidth();
    int fontSize = fontManager.getFontSize(1);

    QVBoxLayout *labelLayout = new QVBoxLayout();
    labelLayout->setContentsMargins(50, 0, 50, 20);
    labelLayout->setSpacing(2);
    labelLayout->setAlignment(Qt::AlignTop);

    _table = new ITableWidget(false);
    _table->setFont(fontManager.textFont(fontSize));
    _table->setRowCount(ROW_NUM);
    _table->setColumnCount(COLUMN_NUM);
    _table->setFixedSize(itemW, ITEM_H * (ROW_NUM + 1));  // 固定尺寸。
    _table->horizontalHeader()->setDefaultSectionSize(ITEM_H);  // 行高。
    _table->horizontalHeader()->setStyleSheet("QHeaderView::section {border:0px;height:30px;}");
    _table->verticalHeader()->setVisible(false);                        //列首隐藏

    QStringList hheader;
    QTableWidgetItem *item;
    int itemTimeWidth = (itemW - 10) / 3;
    int itemLogWidth = itemW - 10 - itemTimeWidth;
    // 创建Item。
    for (int i = 0; i < ROW_NUM; i++)
    {
        for (int j = 0; j < COLUMN_NUM; j++)
        {
            _table->setColumnWidth(j, (j < (COLUMN_NUM - 1)) ? itemTimeWidth : itemLogWidth);
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setBackgroundColor(Qt::white);
            _table->setItem(i, j, item);
        }

        _table->setRowHeight(i, ITEM_H);
    }
    hheader << trs("Time") << trs("ErrorLogInfo");
    _table->setHorizontalHeaderLabels(hheader);

    _delegate = new TableItemDelegate();
    _table->setItemDelegateForColumn(COLUMN_NUM - 1, _delegate);

    connect(_table, SIGNAL(itemEnter(int)), this, SLOT(itemClick(int)));

    QHBoxLayout *helpLayout = new QHBoxLayout();
    helpLayout->setContentsMargins(60, 0, 60, 0);
    helpLayout->setSpacing(2);

    _summaryBtn = new IButton();
    _summaryBtn->setText(trs("Summary"));
    _summaryBtn->setFont(fontManager.textFont(fontSize));
    connect(_summaryBtn, SIGNAL(realReleased()), this, SLOT(summaryBtnClick()));

    _exportBtn = new IButton();
    _exportBtn->setText(trs("ErrorLogExport"));
    _exportBtn->setFont(fontManager.textFont(fontSize));
    connect(_exportBtn, SIGNAL(realReleased()), this, SLOT(exportBtnClick()));

    _eraseBtn = new IButton();
    _eraseBtn->setText(trs("ErrorLogErase"));
    _eraseBtn->setFont(fontManager.textFont(fontSize));
    connect(_eraseBtn, SIGNAL(realReleased()), this, SLOT(eraseBtnClick()));

    _upBtn = new IButton();
    _upBtn->setFixedSize(ITEM_H, ITEM_H);
    _upBtn->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(_upBtn, SIGNAL(realReleased()), this, SLOT(upBtnClick()));

    _downBtn = new IButton();
    _downBtn->setFixedSize(ITEM_H, ITEM_H);
    _downBtn->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(_downBtn, SIGNAL(realReleased()), this, SLOT(downBtnClick()));


    helpLayout->addWidget(_summaryBtn);
    helpLayout->addWidget(_exportBtn);
    helpLayout->addWidget(_eraseBtn);
    helpLayout->addWidget(_upBtn);
    helpLayout->addWidget(_downBtn);

    _info = new QLabel(trs("WarningNoUSB"), this);
    _info->setFont(fontManager.textFont(fontSize));

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 20, 5);
    hLayout->addWidget(_info, 0, Qt::AlignRight);

    labelLayout->addWidget(_table, 0, Qt::AlignCenter);
    labelLayout->addLayout(helpLayout);
    labelLayout->addStretch();
    labelLayout->setSpacing(10);
    labelLayout->addLayout(hLayout);

    mainLayout->addLayout(labelLayout);

    _USBCheckTimer = new QTimer();
    _USBCheckTimer->setInterval(500);
    connect(_USBCheckTimer, SIGNAL(timeout()), this, SLOT(USBCheckTimeout()));
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void ServiceErrorLogMenu::init()
{
    loadData();
    titleInit();

    if (usbManager.isUSBExist())
    {
        _info->hide();
    }

    _USBCheckTimer->start();
}

void ServiceErrorLogMenu::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Up:
    case Qt::Key_Left:
        focusNextPrevChild(false);
        return;
    case Qt::Key_Down:
    case Qt::Key_Right:
        focusNextChild();
        return;
    default:
        break;
    }

    QWidget::keyPressEvent(e);
}

void ServiceErrorLogMenu::showEvent(QShowEvent *e)
{
    init();
    QWidget::showEvent(e);
}

void ServiceErrorLogMenu::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);

    if (NULL != _USBCheckTimer)
    {
        _USBCheckTimer->stop();
    }
}

/***************************************************************************************************
 * display the error log summary info
 **************************************************************************************************/
void ServiceErrorLogMenu::summaryBtnClick()
{
    QString str;
    QTextStream stream(&str);
    ErrorLog::Summary summary = errorLog.getSummary();
    stream << trs("NumberOfErrors") << summary.NumOfErrors << endl;
    stream << trs("NumberOfCriticalFaults") << summary.numOfCriticalErrors << endl;
    stream << trs("MostRecentError") << summary.mostRecentErrorDate << endl;
    stream << trs("MostRecentCriticalFault") << summary.mostRecentCriticalErrorDate << endl;
    stream << trs("OldestError") << summary.oldestErrorDate << endl;
    stream << trs("LastEraseTime") << summary.lastEraseTimeDate << endl;

    ErrorLogViewer viewer;
    viewer.setTitleBarText(trs("Summary"));
    viewer.setText(str);
    viewer.exec();
}

/***************************************************************************************************
 * itemClick : handle item click signal
 **************************************************************************************************/
void ServiceErrorLogMenu::itemClick(int index)
{
    if (index >= 0)
    {
        // view the log
        int realIndex = currentPage * ROW_NUM + index;
        ErrorLogItemBase *item = errorLog.getLog(realIndex);
        if (item->isLogEmpty())
        {
            delete item;
            return;
        }

        ErrorLogViewer(item).exec();
        delete item;
    }
}

void ServiceErrorLogMenu::USBCheckTimeout()
{
    // 检查U盘
    if (!usbManager.isUSBExist())
    {
        _info->show();
    }
    else
    {
        _info->hide();
    }
}

/***************************************************************************************************
 * init : data initialize
 **************************************************************************************************/
void ServiceErrorLogMenu::titleInit()
{
    int count = errorLog.count();
    totalPage = (count + ROW_NUM - 1) / ROW_NUM;
    title = trs("ErrorLog");
    refreshTitle();
}

/***************************************************************************************************
 * loadData : load data to the table
 **************************************************************************************************/
void ServiceErrorLogMenu::loadData()
{
    int index = currentPage * ROW_NUM;
    for (int i = 0; i < ROW_NUM; i++)
    {
        QTableWidgetItem *timeItem = NULL;
        QTableWidgetItem *infoItem = NULL;
        ErrorLogItemBase *errlogItem = NULL;
        timeItem = _table->item(i, 0);
        if (!timeItem)
        {
            timeItem = new QTableWidgetItem();
            _table->setItem(i, 0, timeItem);
        }
        infoItem = _table->item(i, 1);
        if (!infoItem)
        {
            infoItem = new QTableWidgetItem();
            _table->setItem(i, 1, infoItem);
        }

        errlogItem = errorLog.getLog(index);
        if (errlogItem)
        {
            timeItem->setText(errlogItem->getTime());
            infoItem->setText(errlogItem->name());
            infoItem->setIcon(icons[!errlogItem->isLogEmpty()]);
            if (errlogItem->type() == CriticalFaultLogItem::Type)
            {
                infoItem->setTextColor(Qt::red);
            }
            else
            {
                infoItem->setTextColor(Qt::black);
            }
            delete errlogItem;
            errlogItem = NULL;
        }
        else
        {
            timeItem->setText(QString());
            infoItem->setText(QString());
            infoItem->setIcon(icons[0]);
        }
        index++;
    }
}

/***************************************************************************************************
 * refreshTitle : refresh title when page change
 **************************************************************************************************/
void ServiceErrorLogMenu::refreshTitle()
{
    //    if(totalPage)
    //    {
    //        setTitleBarText(QString("%1(%2/%3)").arg(title).arg(currentPage+1).arg(totalPage));
    //    }
    //    else
    //    {
    //        setTitleBarText(QString("%1(0/0)").arg(title));
    //    }
}

/***************************************************************************************************
 * focusNexPrevChild : handle focus issue
 **************************************************************************************************/
// bool FactoryErrorLogMenu::focusNextPrevChild(bool next)
//{
//    Q_D(FactoryErrorLogMenu);
//    if( next &&  downBtn->hasFocus())
//    {
//        //the table can't be focused when it's empty, focus next
//        if(!table->isEmpty())
//        {
//            table->setFocus(Qt::TabFocusReason);
//        }
//        else
//        {
//            exportBtn->setFocus(Qt::TabFocusReason);
//        }
//        return true;
//    }
//    else if (!next && table->hasFocus())
//    {
//        downBtn->setFocus();
//        return true;
//    }
//    return SubMenu::focusNextPrevChild(next);
//}

#if 0
void ServiceErrorLogMenu::exportBtnClick()
{
    if (udiskManager.isUSBExist())
    {
        udiskManager.exportErrorLog();
        ExportDataWidget exportDataWidget(EXPORT_ERROR_LOG_BY_USB);
        connect(&udiskManager, SIGNAL(exportProcessChanged(unsigned char)),
                &exportDataWidget, SLOT(setBarValue(unsigned char)));
        connect(&udiskManager, SIGNAL(exportError()), &exportDataWidget, SLOT(reject()));
        connect(&exportDataWidget, SIGNAL(cancel()), &udiskManager, SLOT(cancelExport()));
        if (0 == exportDataWidget.exec())
        {
            QString msg;
            UdiskManager::ExportStatus status = udiskManager.getExportStatus();
            if (status == UdiskManager::Cancel)
            {
                msg = trs("TransferCancel");
            }
            else if (status == UdiskManager::Disconnect)
            {
                msg = trs("TransferDisconnect");
            }
            else
            {
                msg = trs("TransferFailed");
            }
            IMessageBox messageBox(trs("Warn"), msg, QStringList(trs("EnglishYESChineseSURE")));
            messageBox.exec();
        }
    }
    else
    {
        IMessageBox messageBox(trs("Warn"), trs("WarningNoUSB"), QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
}
#else
void ServiceErrorLogMenu::exportBtnClick()
{
    if (usbManager.isUSBExist())
    {
        ExportDataWidget exportDataWidget(EXPORT_ERROR_LOG_BY_USB);
        connect(&usbManager, SIGNAL(exportProcessChanged(unsigned char)),
                &exportDataWidget, SLOT(setBarValue(unsigned char)));
        connect(&usbManager, SIGNAL(exportError()), &exportDataWidget, SLOT(reject()));
        connect(&exportDataWidget, SIGNAL(cancel()), &usbManager, SLOT(cancelExport()));

        // start export
        if (usbManager.exportErrorLog())
        {
            if (0 == exportDataWidget.exec())
            {
                QString msg;
                DataExporterBase::ExportStatus status = usbManager.getLastExportStatus();
                if (status == DataExporterBase::Cancel || exportDataWidget.isTransferCancel())
                {
                    msg = trs("TransferCancel");
                }
                else if (status == DataExporterBase::Disconnect)
                {
                    msg = trs("TransferDisconnect");
                }
                else if (status == DataExporterBase::NoSpace)
                {
                    msg = trs("WarnLessUSBFreeSpace");
                }
                else
                {
                    msg = trs("TransferFailed");
                }
                IMessageBox messageBox(trs("Warn"), msg, QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
        }
    }
    else
    {
        IMessageBox messageBox(trs("Warn"), trs("WarningNoUSB"), QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
}

#endif

void ServiceErrorLogMenu::eraseBtnClick()
{
    IMessageBox messageBox(trs("ClearErrorLog"), trs("ConfirmClearErrorLog"));
    if (1 == messageBox.exec())
    {
        errorLog.clear();
        loadData();
        init();
        unsigned int timestamp = QDateTime::currentDateTime().toTime_t();
        systemConfig.setNumValue("ErrorLogEraseTime", timestamp);
        systemConfig.save();
        systemConfig.saveToDisk();
    }
}

void ServiceErrorLogMenu::upBtnClick()
{
    if (currentPage > 0)
    {
        currentPage--;
        loadData();
        refreshTitle();
    }
}

void ServiceErrorLogMenu::downBtnClick()
{
    if (currentPage < totalPage - 1)
    {
        currentPage++;
        loadData();
        refreshTitle();
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ServiceErrorLogMenu::~ServiceErrorLogMenu()
{
    if (_delegate)
    {
        delete _delegate;
    }
    if (NULL != _USBCheckTimer)
    {
        delete _USBCheckTimer;
    }
}
