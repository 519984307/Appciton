#include <QHBoxLayout>
#include "RescueDataListWidget.h"
#include "SummaryRescueRangePrint.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "WindowManager.h"
#include "IButton.h"
#include "LabelButton.h"
#include "IDropList.h"
#include "DataStorageDirManager.h"
#include "PrintManager.h"
#include "Debug.h"
#include "SummaryRescueDataWidget.h"
#include "MenuManager.h"
#include "IMessageBox.h"

#define ITEM_HEIGHT (30)

SummaryRescueRangePrint *SummaryRescueRangePrint::_selfObj = NULL;

/**********************************************************************************************************************
 * 构造。
 **********************************************************************************************************************/
SummaryRescueRangePrint::SummaryRescueRangePrint() : PopupWidget(),
    _listWidget(NULL), _incidentBtn(NULL), _eventLogBtn(NULL), _selectSnapshotBtn(NULL),
  _printType(PRINT_SELECTED_HEAD_AND_SNAPSHOTS), _up(NULL), _down(NULL)
{

    int maxw = windowManager.getPopMenuWidth();
    int maxh = windowManager.getPopMenuHeight();

    int fontSize = fontManager.getFontSize(1);
    _listWidget = new RescueDataListWidget(maxw - 12, maxh - ITEM_HEIGHT - 36);
    _listWidget->setSelectSingle(true);

//    _print = new IDropList(trs("PrintOption"));
//    _print->setFixedHeight(ITEM_HEIGHT);
//    _print->setFont(fontManager.textFont(fontSize));
//    connect(_print, SIGNAL(currentIndexChange(int)), this, SLOT(_printReleased(int)));
//    _print->addItem(trs("SelectedHeadAndSnapshot"));
//    _print->addItem(trs("SelectedLogs"));
//    _print->addItem(trs("AllHeader"));
//    _print->addItem(trs("AllSnapshotIncident"));

    QFont font = fontManager.textFont(fontSize);
    _incidentBtn = new LButtonEx();
    _incidentBtn->setFont(font);
    _incidentBtn->setFixedHeight(ITEM_HEIGHT);
    _incidentBtn->setText(trs("PrintIncident"));
    connect(_incidentBtn, SIGNAL(realReleased()), this, SLOT(_printBtnRelease()));

    _eventLogBtn = new LButtonEx();
    _eventLogBtn->setFont(font);
    _eventLogBtn->setFixedHeight(ITEM_HEIGHT);
    _eventLogBtn->setText(trs("PrintEventLog"));
    connect(_eventLogBtn, SIGNAL(realReleased()), this, SLOT(_printBtnRelease()));

    _selectSnapshotBtn = new LButtonEx();
    _selectSnapshotBtn->setFont(font);
    _selectSnapshotBtn->setFixedHeight(ITEM_HEIGHT);
    _selectSnapshotBtn->setText(trs("SelectSnapshotsToPrint"));
    connect(_selectSnapshotBtn, SIGNAL(realReleased()), this, SLOT(_printBtnRelease()));

    _up = new IButton();
    _up->setFixedSize(ITEM_HEIGHT - 2, ITEM_HEIGHT - 2);
    _up->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(_up, SIGNAL(realReleased()), this, SLOT(_upReleased()));

    _down = new IButton();
    _down->setFixedSize(ITEM_HEIGHT - 2, ITEM_HEIGHT - 2);
    _down->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(_down, SIGNAL(realReleased()), this, SLOT(_downReleased()));

    QHBoxLayout *layout = new QHBoxLayout();

    layout->setMargin(1);
    layout->setSpacing(2);
    //layout->addStretch(1);
    //layout->addWidget(_print, 3);
    layout->addWidget(_incidentBtn, 4);
    layout->addWidget(_eventLogBtn, 4);
    layout->addWidget(_selectSnapshotBtn, 4);
    layout->addStretch();
    layout->addWidget(_up, 0, Qt::AlignVCenter);
    layout->addWidget(_down, 0, Qt::AlignVCenter);

    contentLayout->setSpacing(1);
    contentLayout->addWidget(_listWidget);
    contentLayout->addLayout(layout);

    _listWidget->setScrollbarVisiable(false);
    connect(_listWidget, SIGNAL(pageInfoChange()), this, SLOT(_updateWindowTitle()));

    setFixedSize(maxw, maxh);
}

void SummaryRescueRangePrint::_updateWindowTitle()
{
    QString str;
    if (languageManager.getCurLanguage() == LanguageManager::English)
    {
        str = QString("%1 (page %2 of %3)")
                .arg(trs("PrintSelectedRescue"))
                .arg(_listWidget->getCurPage() + 1)
                .arg(_listWidget->getTotalPage() == 0 ? 1 : _listWidget->getTotalPage());
    }
    else
    {
        str = QString("%1 (%2/%3)")
                .arg(trs("PrintSelectedRescue"))
                .arg(_listWidget->getCurPage() + 1)
                .arg(_listWidget->getTotalPage() == 0 ? 1 : _listWidget->getTotalPage());
    }
    setTitleBarText(str);
}

/**********************************************************************************************************************
 * 析构。
 **********************************************************************************************************************/
SummaryRescueRangePrint::~SummaryRescueRangePrint()
{

}

/**********************************************************************************************************************
 * 打印回调。
 **********************************************************************************************************************/
void SummaryRescueRangePrint::_printReleased(int index)
{
    SummaryDataPrintType type = (SummaryDataPrintType)(PRINT_RESCUE_MORE + index + 1);

    switch (type)
    {
        case PRINT_SELECTED_HEAD_AND_SNAPSHOTS:
        case PRINT_SELECTED_LOG:
        {
            QList<int> checklist;
            checklist.clear();

            if (NULL != _listWidget)
            {
                _listWidget->getCheckList(checklist);
            }

            if (!checklist.isEmpty())
            {
                checklist.clear();
                printManager.requestPrint(PRINT_TYPE_REVIEW, PRINT_LAYOUT_ID_SUMMARY_REVIEW_REPORT);
            }
            break;
        }
        case PRINT_ALL_RESCUE:
        case PRINT_ALL_RESCUE_HEADS:
            printManager.requestPrint(PRINT_TYPE_REVIEW, PRINT_LAYOUT_ID_SUMMARY_REVIEW_REPORT);
            break;
        default:
            break;
    }
}

void SummaryRescueRangePrint::_printBtnRelease()
{
    LButtonEx *btn = qobject_cast<LButtonEx*>(sender());
    QList<int> checklist;

    if (NULL != _listWidget)
    {
        _listWidget->getCheckList(checklist);
    }

    if (checklist.isEmpty())
    {
        QString title = btn->text();
        QString msg = trs("SelectIncidentToPrint");
        IMessageBox(title, msg, QStringList(trs("EnglishYESChineseSURE"))).exec();
        return;
    }

    if (btn == _incidentBtn)
    {
        _printType = PRINT_SELECTED_HEAD_AND_SNAPSHOTS;
        printManager.requestPrint(PRINT_TYPE_REVIEW, PRINT_LAYOUT_ID_SUMMARY_REVIEW_REPORT);
    }
    else if (btn == _eventLogBtn)
    {
        _printType = PRINT_SELECTED_LOG;
        printManager.requestPrint(PRINT_TYPE_REVIEW, PRINT_LAYOUT_ID_SUMMARY_REVIEW_REPORT);
    }
    else if (btn == _selectSnapshotBtn)
    {
        _printType = PRINT_SELECTED_SNAPSHOT;
        summaryRescueDataWidget.setIncidentIndex(checklist.first());
        QRect r = windowManager.getMenuArea();
        int x = r.x() + (r.width() - menuManager.getSubmenuWidth()) / 2;
        int y = r.y() + (r.height() - menuManager.getSubmenuHeight());
        summaryRescueDataWidget.autoShow(x, y);
    }
    else
    {
        qDebug()<<Q_FUNC_INFO<<"Invalid signal sender.";
    }

}

/**********************************************************************************************************************
 * 获取打印路径。
 **********************************************************************************************************************/
void SummaryRescueRangePrint::getPrintPath(QStringList &list)
{
    QList<int> checklist;

    if (NULL != _listWidget)
    {
        _listWidget->getCheckList(checklist);
    }

    SummaryDataPrintType type = getPrintType();

    switch (type)
    {
        case PRINT_SELECTED_HEAD_AND_SNAPSHOTS:
        case PRINT_SELECTED_LOG:
        {
            int num = checklist.count();
            for (int i = 0; i < num; ++i)
            {
                int index = checklist.at(i);
                list << dataStorageDirManager.getRescueDataDir(index);
            }
            break;
        }
        case PRINT_ALL_RESCUE_HEADS:
        case PRINT_ALL_RESCUE:
        {
            int num = dataStorageDirManager.getDirNum();
            for (int i = 0; i < num; ++i)
            {
                list << dataStorageDirManager.getRescueDataDir(i);
            }
            break;
        }
        default:
            summaryRescueDataWidget.getPrintPath(list);
            break;
    }

}

/**********************************************************************************************************************
 * 获取打印类型。
 **********************************************************************************************************************/
SummaryDataPrintType SummaryRescueRangePrint::getPrintType()
{
    return _printType;
    //int index = _print->currentIndex();
    //SummaryDataPrintType type = (SummaryDataPrintType)(PRINT_RESCUE_MORE + index + 1);

    //return type;
}

/**********************************************************************************************************************
 * 按键事件。
 **********************************************************************************************************************/
void SummaryRescueRangePrint::keyPressEvent(QKeyEvent *e)
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
 * 上翻页。
 **********************************************************************************************************************/
void SummaryRescueRangePrint::_upReleased()
{
    if (NULL != _listWidget)
    {
        _listWidget->pageChange(true);
    }
}

/**********************************************************************************************************************
 * 下翻页。
 **********************************************************************************************************************/
void SummaryRescueRangePrint::_downReleased()
{
    if (NULL != _listWidget)
    {
        _listWidget->pageChange(false);
    }
}


