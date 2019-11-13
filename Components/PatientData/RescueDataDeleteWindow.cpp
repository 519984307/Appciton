/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/22
 **/

#include "RescueDataDeleteWindow.h"
#include "RescueDataListNewWidget.h"
#include "Framework/UI/TableView.h"
#include "Framework/UI/Button.h"
#include "Framework/UI/ThemeManager.h"
#include "Framework/Language/LanguageManager.h"
#include "Framework/Utility/Utility.h"
#include <QPointer>
#include "WindowManager.h"
#include "FontManager.h"
#include "MessageBox.h"
#include <QEventLoop>
#include "DataStorageDirManager.h"
#include <QBoxLayout>
#include <QMutex>

RescueDataDeleteWindow *RescueDataDeleteWindow::_selfObj = NULL;

static QVariant deleteSelectIncidnets(const QVariant &para)
{
    QVariantList list = para.toList();
    foreach(QVariant var, list)
    {
        int index = var.toInt();
        dataStorageDirManager.deleteData(index);
    }
    return QVariant();
}

static QVariant deleteAllData(const QVariant & para)
{
    static QMutex mutex;
    Q_UNUSED(para);
    if (mutex.tryLock())
    {
        dataStorageDirManager.deleteAllData();
        mutex.unlock();
        return 1;
    }
    return QVariant();
}

class RescueDataDeleteWindowPrivate
{
public:
    RescueDataDeleteWindowPrivate()
        : dataListWidget(NULL),
          deleteCase(NULL),
          deleteAll(NULL),
          up(NULL),
          down(NULL),
          widgetHeight(themeManager.getAcceptableControlHeight())
    {}
    ~RescueDataDeleteWindowPrivate(){}

    RescueDataListNewWidget *dataListWidget;
    Button *deleteCase;
    Button *deleteAll;
    Button *up;
    Button *down;
    QPointer<QThread> deleteThreadPtr;
    int widgetHeight;
};

RescueDataDeleteWindow::~RescueDataDeleteWindow()
{
    delete d_ptr;
}

void RescueDataDeleteWindow::_updateWindowTitle()
{
    QString str;
    LanguageManager *langMgr = LanguageManager::getInstance();
    if (langMgr->getCurLanguage() == LanguageManager::English)
    {
        str = QString("%1 (page %2 of %3)")
                .arg(trs("EraseData"))
                .arg(d_ptr->dataListWidget->getCurPage() + 1)
                .arg(d_ptr->dataListWidget->getTotalPage() == 0 ?
                         1 : d_ptr->dataListWidget->getTotalPage());
    }
    else
    {
        str = QString("%1 (%2/%3)")
                .arg(trs("EraseData"))
                .arg(d_ptr->dataListWidget->getCurPage() + 1)
                .arg(d_ptr->dataListWidget->getTotalPage() == 0 ?
                         1 : d_ptr->dataListWidget->getTotalPage());
    }
    setWindowTitle(str);
}

void RescueDataDeleteWindow::_deleteSelectReleased()
{
    QStringList list;
    d_ptr->dataListWidget->getStrList(&list);
    if (list.empty())
    {
        MessageBox msgbox(trs("Prompt"), trs("NoIncidents"), QStringList(trs("EnglishYESChineseSURE")));
        msgbox.exec();
        return;
    }

    QStringList checkList;
    d_ptr->dataListWidget->getCheckList(&checkList);
    if (checkList.isEmpty())
    {
        MessageBox msgbox(trs("Prompt"), trs("SelectIncidents"), QStringList(trs("EnglishYESChineseSURE")));
        msgbox.exec();
        return;
    }

    MessageBox messageBox(trs("Prompt"), trs("EraseSelectedRescueData"));
    if (messageBox.exec() == 0)
    {
        return;
    }

    if (d_ptr->deleteThreadPtr)
    {
        // exist delete thread, do nothing
        MessageBox msgbox(trs("Warn"), trs("WarningSystemBusyTryLater"));
        msgbox.exec();
        return;
    }

    // find delete index
    int count = checkList.count();
    int totalCount = list.count();
    QVariantList indexList;
    for (int i = 0; i < count; ++i)
    {
        int index = list.indexOf(checkList.at(i));
        if (-1 != index)
        {
            indexList.append(totalCount - index - 1);
        }
    }

    d_ptr->deleteThreadPtr = new Util::WorkerThread(deleteSelectIncidnets, indexList);
    QEventLoop eventLoop;
    connect(d_ptr->deleteThreadPtr.data(), SIGNAL(finished()), &eventLoop, SLOT(quit()));
    d_ptr->deleteThreadPtr->start();
    eventLoop.exec();

    d_ptr->dataListWidget->reload();
    checkList.clear();
    list.clear();
}

void RescueDataDeleteWindow::_deleteAllReleased()
{
    QStringList list;
    d_ptr->dataListWidget->getStrList(&list);
    if (list.empty())
    {
        MessageBox msgbox(trs("Prompt"), trs("NoIncidents"), QStringList(trs("EnglishYESChineseSURE")));
        msgbox.exec();
        return;
    }

    MessageBox messageBox(trs("Prompt"), trs("ClearAllRescueData"));
    if (0 == messageBox.exec())
    {
        return;
    }

    if (d_ptr->deleteThreadPtr)
    {
        // exist delete thread, do nothing
        MessageBox msgbox(trs("Warn"), trs("WarningSystemBusyTryLater"));
        msgbox.exec();
        return;
    }

    d_ptr->deleteThreadPtr = new Util::WorkerThread(deleteAllData, QVariant());
    QEventLoop eventLoop;
    connect(d_ptr->deleteThreadPtr.data(), SIGNAL(finished()), &eventLoop, SLOT(quit()));
    d_ptr->deleteThreadPtr->start();
    eventLoop.exec();

    d_ptr->dataListWidget->reload();
}

void RescueDataDeleteWindow::_upReleased()
{
    if (NULL != d_ptr->dataListWidget)
    {
        d_ptr->dataListWidget->pageChange(true);
    }
}

void RescueDataDeleteWindow::_downReleased()
{
    if (NULL != d_ptr->dataListWidget)
    {
        d_ptr->dataListWidget->pageChange(false);
    }
}

void RescueDataDeleteWindow::_updateEraseBtnStatus()
{
    QStringList checkList , strList;
    d_ptr->dataListWidget->getCheckList(&checkList);
    d_ptr->dataListWidget->getStrList(&strList);
    int checkListCount = checkList.count();
    int strListCount = strList.count();
    // erase select button
    if (checkListCount > 0)
    {
        d_ptr->deleteCase->setEnabled(true);
    }
    else
    {
        d_ptr->deleteCase->setEnabled(false);
    }

    // erase all button
    if (strListCount > 0)
    {
        d_ptr->deleteAll->setEnabled(true);
    }
    else
    {
        d_ptr->deleteAll->setEnabled(false);
    }
}

RescueDataDeleteWindow::RescueDataDeleteWindow()
    :Dialog(),
      d_ptr(new RescueDataDeleteWindowPrivate())
{
    QVBoxLayout *contentLayout = new QVBoxLayout();
    int maxw = windowManager.getPopWindowWidth();
    int maxh = windowManager.getPopWindowHeight();

    int margins = contentsMargins().left() * 2 + 10;
    d_ptr->dataListWidget = new RescueDataListNewWidget(maxw - margins,
                                                        maxh - d_ptr->widgetHeight - getTitleHeight() - margins);
    d_ptr->dataListWidget->setShowCurRescue(false);
    connect(d_ptr->dataListWidget , SIGNAL(pageInfoChange()),
            this, SLOT(_updateWindowTitle()));
    connect(d_ptr->dataListWidget, SIGNAL(btnRelease()),
            this, SLOT(_updateEraseBtnStatus()));

    d_ptr->deleteCase = new Button(trs("EraseSelect"));
    d_ptr->deleteCase->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->deleteCase, SIGNAL(released()), this, SLOT(_deleteSelectReleased()));

    d_ptr->deleteAll = new Button(trs("EraseAll"));
    d_ptr->deleteAll->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->deleteAll, SIGNAL(released()), this, SLOT(_deleteAllReleased()));

    QIcon ico = themeManager.getIcon(ThemeManager::IconUp, QSize(32, 32));
    d_ptr->up = new Button("", ico);
    d_ptr->up->setIconSize(QSize(32, 32));
    d_ptr->up->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->up, SIGNAL(released()), this, SLOT(_upReleased()));

    ico = themeManager.getIcon(ThemeManager::IconDown, QSize(32, 32));
    d_ptr->down = new Button("", ico);
    d_ptr->down->setIconSize(QSize(32, 32));
    d_ptr->down->setButtonStyle(Button::ButtonIconOnly);
    connect(d_ptr->down, SIGNAL(released()), this, SLOT(_downReleased()));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(2);
    layout->addStretch(4);
    layout->addWidget(d_ptr->deleteCase, 2);
    layout->addWidget(d_ptr->deleteAll, 2);
    layout->addStretch(2);
    layout->addWidget(d_ptr->up, 1);
    layout->addWidget(d_ptr->down, 1);

    contentLayout->setSpacing(5);
    contentLayout->setMargin(5);
    contentLayout->addWidget(d_ptr->dataListWidget);
    contentLayout->addLayout(layout);

    setFixedSize(maxw, maxh);
    setWindowLayout(contentLayout);
}
