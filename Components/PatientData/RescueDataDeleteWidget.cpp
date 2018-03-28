#include <QHBoxLayout>
#include "RescueDataListWidget.h"
#include "RescueDataDeleteWidget.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "WindowManager.h"
#include "IButton.h"
#include "IComboList.h"
#include "ExportDataWidget.h"
#include "DataStorageDirManager.h"
#include "IMessageBox.h"
#include "LabelButton.h"
#include "Utility.h"

#define ITEM_HEIGHT (30)

static long deleteSelectIncidnets(const QVariant &para)
{
    QVariantList list = para.toList();
    foreach(QVariant var, list)
    {
        int index = var.toInt();
        dataStorageDirManager.deleteData(index);
    }
    return 0;
}

RescueDataDeleteWidget *RescueDataDeleteWidget::_selfObj = NULL;

/**********************************************************************************************************************
 * 构造。
 **********************************************************************************************************************/
RescueDataDeleteWidget::RescueDataDeleteWidget() : PopupWidget()
{

    int maxw = windowManager.getPopMenuWidth();
    int maxh = windowManager.getPopMenuHeight();

    int fontSize = fontManager.getFontSize(1);
    _listWidget = new RescueDataListWidget(maxw - 20, maxh - 30 - 36);
    _listWidget->setShowCurRescue(false);

    _delete = new LButtonEx();
    _delete->setFixedHeight(ITEM_HEIGHT);
    _delete->setFont(fontManager.textFont(fontSize));
    _delete->setText(trs("EraseSelected"));
    connect(_delete, SIGNAL(realReleased()), this, SLOT(_deleteSelectReleased()));

    _deleteAll = new LButtonEx();
    _deleteAll->setFixedHeight(ITEM_HEIGHT);
    _deleteAll->setFont(fontManager.textFont(fontSize));
    _deleteAll->setText(trs("EraseAll"));
    connect(_deleteAll, SIGNAL(realReleased()), this, SLOT(_deleteAllReleased()));

    _up = new IButton();
    _up->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _up->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(_up, SIGNAL(realReleased()), this, SLOT(_upReleased()));

    _down = new IButton();
    _down->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _down->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(_down, SIGNAL(realReleased()), this, SLOT(_downReleased()));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(2);
    layout->addStretch(3);
    layout->addWidget(_delete, 2);
    layout->addWidget(_deleteAll, 2);
    layout->addStretch(2);
    layout->addWidget(_up, 1);
    layout->addWidget(_down, 1);

    contentLayout->setSpacing(1);
    contentLayout->addWidget(_listWidget);
    contentLayout->addLayout(layout);

    _listWidget->setScrollbarVisiable(false);
    connect(_listWidget, SIGNAL(pageInfoChange()), this, SLOT(_updateWindowTitle()));

    setFixedSize(maxw, maxh);
}

/**********************************************************************************************************************
 * 析构。
 **********************************************************************************************************************/
RescueDataDeleteWidget::~RescueDataDeleteWidget()
{

}

/**********************************************************************************************************************
 * 按键事件。
 **********************************************************************************************************************/
void RescueDataDeleteWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Left:
        case Qt::Key_Up:
            focusNextPrevChild(false);
            break;
        case Qt::Key_Right:
        case Qt::Key_Down:
            focusNextChild();
            break;
        default:
            break;
    }
}

/**********************************************************************************************************************
 * 删除回调。
 **********************************************************************************************************************/
void RescueDataDeleteWidget::_deleteSelectReleased()
{
    QStringList list;
    _listWidget->getStrList(list);
    if (list.empty())
    {
        IMessageBox msgbox(trs("Prompt"), trs("NoIncidents"), QStringList(trs("EnglishYESChineseSURE")));
        msgbox.exec();
        return;
    }

    QStringList checkList;
    _listWidget->getCheckList(checkList);
    if (checkList.isEmpty())
    {
        IMessageBox msgbox(trs("Prompt"), trs("SelectIncidents"), QStringList(trs("EnglishYESChineseSURE")));
        msgbox.exec();
        return;
    }

    IMessageBox messageBox(trs("Prompt"), trs("EraseSelectedRescueData"));
    if(messageBox.exec() == 0)
    {
        return;
    }

    if(_deleteThreadPtr)
    {
        //exist delete thread, do nothing
        IMessageBox msgbox(trs("Warn"), trs("WarningSystemBusyTryLater"));
        msgbox.exec();
        return;
    }

    //find delete index
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

    _deleteThreadPtr = new Util::WorkerThread(deleteSelectIncidnets, indexList);
    QEventLoop eventLoop;
    connect(_deleteThreadPtr.data(), SIGNAL(finished()), &eventLoop, SLOT(quit()));
    _deleteThreadPtr->start();
    eventLoop.exec();

    _listWidget->reload();
    checkList.clear();
    list.clear();
}

/**********************************************************************************************************************
 * 删除回调。
 **********************************************************************************************************************/
static long deleteAllData(const QVariant & para)
{
    static QMutex mutex;
    Q_UNUSED(para);
    if(mutex.tryLock())
    {
        dataStorageDirManager.deleteAllData();
        mutex.unlock();
        return 1;
    }
    return 0;
}

/**********************************************************************************************************************
 * 删除回调。
 **********************************************************************************************************************/
void RescueDataDeleteWidget::_deleteAllReleased()
{
    QStringList list;
    _listWidget->getStrList(list);
    if (list.empty())
    {
        IMessageBox msgbox(trs("Prompt"), trs("NoIncidents"), QStringList(trs("EnglishYESChineseSURE")));
        msgbox.exec();
        return;
    }

    IMessageBox messageBox(trs("Prompt"), trs("ClearAllRescueData"));
    if (0 == messageBox.exec())
    {
        return;
    }

    if(_deleteThreadPtr)
    {
        //exist delete thread, do nothing
        IMessageBox msgbox(trs("Warn"), trs("WarningSystemBusyTryLater"));
        msgbox.exec();
        return;
    }

    _deleteThreadPtr = new Util::WorkerThread(deleteAllData, QVariant());
    QEventLoop eventLoop;
    connect(_deleteThreadPtr.data(), SIGNAL(finished()), &eventLoop, SLOT(quit()));
    _deleteThreadPtr->start();
    eventLoop.exec();

    _listWidget->reload();
}

/**********************************************************************************************************************
 * 上翻页。
 **********************************************************************************************************************/
void RescueDataDeleteWidget::_upReleased()
{
    if (NULL != _listWidget)
    {
        _listWidget->pageChange(true);
    }
}

/**********************************************************************************************************************
 * 下翻页。
 **********************************************************************************************************************/
void RescueDataDeleteWidget::_downReleased()
{
    if (NULL != _listWidget)
    {
        _listWidget->pageChange(false);
    }
}

void RescueDataDeleteWidget::_updateWindowTitle()
{

    QString str;
    if (languageManager.getCurLanguage() == LanguageManager::English)
    {
        str = QString("%1 (page %2 of %3)")
                .arg(trs("EraseData"))
                .arg(_listWidget->getCurPage() + 1)
                .arg(_listWidget->getTotalPage() == 0 ? 1 : _listWidget->getTotalPage());
    }
    else
    {
        str = QString("%1 (%2/%3)")
                .arg(trs("EraseData"))
                .arg(_listWidget->getCurPage() + 1)
                .arg(_listWidget->getTotalPage() == 0 ? 1 : _listWidget->getTotalPage());
    }
    setTitleBarText(str);
}
