/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/9
 **/

#include "ErrorLogWindow.h"
#include "Framework/Language/LanguageManager.h"
#include "TableView.h"
#include "Button.h"
#include <QLabel>
#include "ErrorLogTableModel.h"
#include "TableHeaderView.h"
#include "TableViewItemDelegate.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "ThemeManager.h"
#include "USBManager.h"
#include <QTimer>
#include "Framework/ErrorLog/ErrorLogItem.h"
#include "Framework/ErrorLog/ErrorLog.h"
#include "ErrorLogViewerWindow.h"
#include "MessageBox.h"
#include "ExportDataWidget.h"
#include "IConfig.h"
#include "WindowManager.h"

#define TABLE_ROW_NR        6

#define HEIGHT_HINT (themeManager.getAcceptableControlHeight())
#define DEFAULT_WIDTH (windowManager.getPopWindowWidth())
#define DEFAULT_HEIGHT (windowManager.getPopWindowHeight())

class ErrorLogWindowPrivate
{
public:
    ErrorLogWindowPrivate()
        : table(NULL), model(NULL), summaryBtn(NULL),
          exportBtn(NULL), eraseBtn(NULL), upPageBtn(NULL),
          downPageBtn(NULL), infoLab(NULL), usbCheckTimer(NULL)
    {}

    /**
     * @brief updatePageBtnStatus  更新翻页按钮状态
     * @param curPage 当前页
     * @param totalPage 总页
     */

public:
    TableView *table;
    ErrorLogTableModel *model;
    Button *summaryBtn;
    Button *exportBtn;
    Button *eraseBtn;
    Button *upPageBtn;
    Button *downPageBtn;
    QLabel *infoLab;
    QTimer *usbCheckTimer;
};
ErrorLogWindow::ErrorLogWindow()
    : Dialog(), d_ptr(new ErrorLogWindowPrivate())
{
    setFixedSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);

    d_ptr->table = new TableView();
    TableHeaderView *horizontalHeader = new TableHeaderView(Qt::Horizontal);
    TableHeaderView *verticalHeader = new TableHeaderView(Qt::Vertical);
    d_ptr->table->setHorizontalHeader(horizontalHeader);
    d_ptr->table->setVerticalHeader(verticalHeader);
    horizontalHeader->setResizeMode(QHeaderView::ResizeToContents);
    verticalHeader->setResizeMode(QHeaderView::ResizeToContents);
    verticalHeader->setVisible(false);
    d_ptr->table->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    d_ptr->table->setShowGrid(false);
    d_ptr->model = new ErrorLogTableModel();
    d_ptr->table->setModel(d_ptr->model);
    d_ptr->table->setFixedHeight(d_ptr->model->getHeaderHeightHint()
                                 + d_ptr->model->getRowHeightHint() * TABLE_ROW_NR);
    d_ptr->table->setFixedWidth(DEFAULT_WIDTH);
    d_ptr->table->setItemDelegate(new TableViewItemDelegate(this));
    connect(d_ptr->table, SIGNAL(rowClicked(int)), this, SLOT(itemClickSlot(int)));
    connect(d_ptr->model, SIGNAL(pageInfoUpdate(int, int)), this, SLOT(onPageInfoUpdated(int, int)));

    d_ptr->summaryBtn = new Button(trs("Summary"));
    d_ptr->summaryBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->summaryBtn->setFixedHeight(HEIGHT_HINT);
    connect(d_ptr->summaryBtn, SIGNAL(released()), this, SLOT(summaryReleased()));

    d_ptr->exportBtn = new Button(trs("ErrorLogExport"));
    d_ptr->exportBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->exportBtn->setFixedHeight(HEIGHT_HINT);
    connect(d_ptr->exportBtn, SIGNAL(released()), this, SLOT(exportReleased()));

    d_ptr->eraseBtn = new Button(trs("ErrorLogErase"));
    d_ptr->eraseBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->eraseBtn->setFixedHeight(HEIGHT_HINT);
    connect(d_ptr->eraseBtn, SIGNAL(released()), this, SLOT(eraseReleased()));

    QIcon ico = themeManager.getIcon(ThemeManager::IconUp, QSize(32, 32));
    d_ptr->upPageBtn = new Button("", ico);
    d_ptr->upPageBtn->setIconSize(QSize(32, 32));
    d_ptr->upPageBtn->setButtonStyle(Button::ButtonIconOnly);
    d_ptr->upPageBtn->setFixedHeight(HEIGHT_HINT);
    connect(d_ptr->upPageBtn, SIGNAL(released()), d_ptr->model, SLOT(upBtnReleased()));

    ico = themeManager.getIcon(ThemeManager::IconDown, QSize(32, 32));
    d_ptr->downPageBtn = new Button("", ico);
    d_ptr->downPageBtn->setIconSize(QSize(32, 32));
    d_ptr->downPageBtn->setButtonStyle(Button::ButtonIconOnly);
    d_ptr->downPageBtn->setFixedHeight(HEIGHT_HINT);
    connect(d_ptr->downPageBtn, SIGNAL(released()), d_ptr->model, SLOT(downBtnReleased()));

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(d_ptr->summaryBtn, 2);
    hLayout->addWidget(d_ptr->exportBtn, 2);
    hLayout->addWidget(d_ptr->eraseBtn, 2);
    hLayout->addWidget(d_ptr->upPageBtn, 1);
    hLayout->addWidget(d_ptr->downPageBtn, 1);

    d_ptr->infoLab = new QLabel(trs("WarningNoUSB"));
    d_ptr->infoLab->setFixedHeight(HEIGHT_HINT);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(d_ptr->table);
    layout->addLayout(hLayout);
    layout->addWidget(d_ptr->infoLab, 1, Qt::AlignRight);

    setWindowLayout(layout);

    // 绑定this父类指针，父类析构时，强制析构子类，防止内存泄露
    d_ptr->usbCheckTimer = new QTimer(this);
    d_ptr->usbCheckTimer->setInterval(500);
    connect(d_ptr->usbCheckTimer, SIGNAL(timeout()), this, SLOT(USBCheckTimeout()));
}

ErrorLogWindow *ErrorLogWindow::getInstance()
{
    static ErrorLogWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new ErrorLogWindow();
    }
    return instance;
}

ErrorLogWindow::~ErrorLogWindow()
{
}

void ErrorLogWindow::init()
{
    d_ptr->model->loadData();

    if (usbManager.isUSBExist())
    {
        d_ptr->infoLab->hide();
    }
    if (d_ptr->table->model()->rowCount() == 0)
    {
        d_ptr->exportBtn->setEnabled(false);
        d_ptr->eraseBtn->setEnabled(false);
    }
    else
    {
        d_ptr->eraseBtn->setEnabled(true);
        d_ptr->table->setFocusPolicy(Qt::StrongFocus);
    }
    d_ptr->usbCheckTimer->start();
}

void ErrorLogWindow::showEvent(QShowEvent *ev)
{
    init();
    Dialog::showEvent(ev);
}

void ErrorLogWindow::itemClickSlot(int row)
{
    int realIndex = d_ptr->model->getErrorLogIndex(row);
    ErrorLogItemBase *item = errorLog.getLog(realIndex);
    if (item->isLogEmpty())
    {
        delete item;
        return;
    }

    ErrorLogViewerWindow(item).exec();
    delete item;
    d_ptr->table->selectRow(row);
}

void ErrorLogWindow::summaryReleased()
{
    QString str;
    QTextStream stream(&str);
    ErrorLog::Summary summary = errorLog.getSummary();
    stream << trs("NumberOfErrors") << summary.NumOfErrors << endl;
    stream << trs("NumberOfCriticalFaults") << summary.numOfCriticalErrors << endl;
    stream << trs("MostRecentError") << summary.mostRecentErrorDate << endl;
    stream << trs("MostRecentCriticalFault") << summary.mostRecentCriticalErrorDate << endl;
    stream << trs("OldestError") << summary.oldestErrorDate << endl;
    unsigned int num = 0;
    systemConfig.getNumValue("ErrorLogEraseTime", num);
    if (num > 0)
    {
        stream << trs("LastEraseTime") << QDateTime::fromTime_t(num).toString("yyyy-MM-dd HH:mm:ss")
               << endl;
    }

    ErrorLogViewerWindow viewer;
    viewer.setWindowTitle(trs("Summary"));
    viewer.setText(str);
    windowManager.showWindow(&viewer, WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorNoAutoClose);
}

void ErrorLogWindow::exportReleased()
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
            QDialog::DialogCode statue = static_cast<QDialog::DialogCode>(exportDataWidget.exec());
            if (QDialog::Rejected == statue)
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
                MessageBox messageBox(trs("Warn"), msg, QStringList(trs("EnglishYESChineseSURE")));
                windowManager.showWindow(&messageBox,
                                         WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorNoAutoClose);
            }
            else if (QDialog::Accepted == statue)  // 导出成功
            {
                eraseReleased();  // 询问是否擦除errorlog
            }
        }
    }
    else
    {
        MessageBox messageBox(trs("Warn"), trs("WarningNoUSB"), QStringList(trs("EnglishYESChineseSURE")));
        windowManager.showWindow(&messageBox,
                                 WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorNoAutoClose);
    }
}

void ErrorLogWindow::eraseReleased()
{
    MessageBox messageBox(trs("ErrorLogErase"), trs("ConfirmClearErrorLog"));
    windowManager.showWindow(&messageBox, WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorNoAutoClose);
    if (QDialog::Accepted == messageBox.result())
    {
        errorLog.clear();
        init();
        unsigned int timestamp = QDateTime::currentDateTime().toTime_t();
        systemConfig.setNumValue("ErrorLogEraseTime", timestamp);
        systemConfig.requestSave();
        systemConfig.saveToDisk();
    }
}

void ErrorLogWindow::USBCheckTimeout()
{
    // 检查U盘
    if (!usbManager.isUSBExist())
    {
        d_ptr->infoLab->show();
        d_ptr->exportBtn->setEnabled(false);
    }
    else
    {
        d_ptr->infoLab->hide();
        if (d_ptr->table->model()->rowCount() != 0)
        {
            d_ptr->exportBtn->setEnabled(true);
        }
    }
}

void ErrorLogWindow::onPageInfoUpdated(int curPage, int totalPage)
{
    // 传进来的当前页是从索引0开始的计数的
    // 显示页码时加1显示
    curPage += 1;
    if (totalPage < 1)
    {
        totalPage = curPage = 1;
    }
    else if (curPage > totalPage)
    {
        curPage = totalPage;
    }
    QString title = trs("ErrorLog");
    title += " (";
    title += QString::number(curPage);
    title += "/";
    title += QString::number(totalPage);
    title += " ";
    title += trs("PageNum");
    title += ")";
    setWindowTitle(title);
}
