#include <QHBoxLayout>
#include "RescueDataListWidget.h"
#include "TrendRescueRangePrint.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "WindowManager.h"
#include "IButton.h"
#include "PrintTrendDataLayout.h"
#include "PrintManager.h"
#include "DataStorageDirManager.h"
#include "TrendDataWidget.h"
#include "LabelButton.h"
#include "IMessageBox.h"

#define ITEM_HEIGHT (30)

TrendRescueRangePrint *TrendRescueRangePrint::_selfObj = NULL;

/**********************************************************************************************************************
 * 构造。
 **********************************************************************************************************************/
TrendRescueRangePrint::TrendRescueRangePrint() : PopupWidget()
{
    int maxw = windowManager.getPopMenuWidth();
    int maxh = windowManager.getPopMenuHeight();

    int fontSize = fontManager.getFontSize(1);
    _listWidget = new RescueDataListWidget(maxw - 12, maxh - ITEM_HEIGHT - 36);

    _printBtn = new LButtonEx();
    _printBtn->setFixedHeight(ITEM_HEIGHT);
    _printBtn->setFont(fontManager.textFont(fontSize));
    _printBtn->setText(trs("Print"));
    connect(_printBtn, SIGNAL(realReleased()), this, SLOT(_printReleased()));

    _backBtn = new LButtonEx();
    _backBtn->setFixedHeight(ITEM_HEIGHT);
    _backBtn->setFont(fontManager.textFont(fontSize));
    _backBtn->setText(trs("Back"));
    connect(_backBtn, SIGNAL(realReleased()), this, SLOT(close()));

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
    layout->addStretch();
    layout->addWidget(_printBtn);
    layout->addWidget(_backBtn);
    layout->addStretch();
    layout->addWidget(_up);
    layout->addWidget(_down);

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
TrendRescueRangePrint::~TrendRescueRangePrint()
{

}

/**********************************************************************************************************************
 * 获取打印路径。
 **********************************************************************************************************************/
void TrendRescueRangePrint::getPrintPath(QStringList &list)
{
    QList<int> checklist;
    checklist.clear();

    if (NULL != _listWidget)
    {
        _listWidget->getCheckList(checklist);
    }

    if (checklist.isEmpty())
    {
        return;
    }

    int num = checklist.count();
    for (int i = 0; i < num; ++i)
    {
        int index = checklist.at(i);
        list << dataStorageDirManager.getRescueDataDir(index);
    }

    checklist.clear();
}

void TrendRescueRangePrint::keyPressEvent(QKeyEvent *e)
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
 * 打印回调。
 **********************************************************************************************************************/
void TrendRescueRangePrint::_printReleased()
{
    QList<int> checklist;
    checklist.clear();

    if (NULL != _listWidget)
    {
        _listWidget->getCheckList(checklist);
    }

    if (checklist.isEmpty())
    {
        QString title = trs("PrintIncident");
        QString msg = trs("SelectIncidentsToPrint");
        IMessageBox(title, msg, QStringList(trs("EnglishYESChineseSURE"))).exec();
        return;
    }

    printManager.requestPrint(PRINT_TYPE_REVIEW, PRINT_LAYOUT_ID_TREND_REPORT);
    checklist.clear();
}

/**********************************************************************************************************************
 * 上翻页。
 **********************************************************************************************************************/
void TrendRescueRangePrint::_upReleased()
{
    if (NULL != _listWidget)
    {
        _listWidget->pageChange(true);
    }
}

/**********************************************************************************************************************
 * 下翻页。
 **********************************************************************************************************************/
void TrendRescueRangePrint::_downReleased()
{
    if (NULL != _listWidget)
    {
        _listWidget->pageChange(false);
    }
}

void TrendRescueRangePrint::_updateWindowTitle()
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
