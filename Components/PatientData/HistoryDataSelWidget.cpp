#include "HistoryDataSelWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "ITableWidget.h"
#include "DataStorageDirManager.h"
#include <QHeaderView>
#include "LanguageManager.h"
#include "TimeDate.h"
#include "HistoryDataReviewWidget.h"
#include <QScrollBar>

#define     ITEM_HEIGHT                 30
#define     DATA_ROW_NUM                15

HistoryDataSelWidget::~HistoryDataSelWidget()
{

}

void HistoryDataSelWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);

    _caclInfo();
}

void HistoryDataSelWidget::_upReleased()
{
    int maxValue = _reviewTable->verticalScrollBar()->maximum();
    int curScroller = _reviewTable->verticalScrollBar()->value();
    if (curScroller > 0)
    {
        QScrollBar *scrollBar = _reviewTable->verticalScrollBar();
        int position = curScroller - (maxValue * DATA_ROW_NUM) / (_reviewTable->rowCount() - DATA_ROW_NUM);
        scrollBar->setSliderPosition(position);
    }
}

void HistoryDataSelWidget::_downReleased()
{
    int maxValue = _reviewTable->verticalScrollBar()->maximum();
    int curScroller = _reviewTable->verticalScrollBar()->value();
    if (curScroller < maxValue)
    {
        QScrollBar *scrollBar = _reviewTable->verticalScrollBar();
        int position = curScroller + (maxValue * DATA_ROW_NUM) / (_reviewTable->rowCount() - DATA_ROW_NUM);
        scrollBar->setSliderPosition(position);
    }
}

void HistoryDataSelWidget::_dataTimeReleased(QModelIndex index)
{
    int dataIndex = index.row() * 2 + index.column();
    QString timeStr = _convertTimeStr(_strList.at(dataIndex));
    historyDataReviewWidget.setHistoryReviewIndex(dataIndex, timeStr);
    exit();
}

HistoryDataSelWidget::HistoryDataSelWidget() : PopupWidget()
{
    setTitleBarText(trs("HistoryDataList"));

    int maxW = windowManager.getPopMenuWidth();
    int maxH = windowManager.getPopMenuHeight();
    setFixedSize(maxW, maxH);

    int fontSize = fontManager.getFontSize(1);
    QFont font = fontManager.textFont(fontSize);

    _reviewTable = new ITableWidget();
    _reviewTable->setColumnCount(2);
    _reviewTable->setFocusPolicy(Qt::NoFocus);
    _reviewTable->horizontalHeader()->setVisible(false);
    _reviewTable->verticalHeader()->setVisible(false);
    _reviewTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    _reviewTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _reviewTable->setSelectionBehavior(QAbstractItemView::SelectItems);
    _reviewTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _reviewTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    _reviewTable->verticalHeader()->setResizeMode(QHeaderView::Interactive);
    _reviewTable->setFixedHeight(ITEM_HEIGHT * DATA_ROW_NUM);
    connect(_reviewTable, SIGNAL(clicked(QModelIndex)), this, SLOT(_dataTimeReleased(QModelIndex)));

    _up = new IButton();
    _up->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _up->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(_up, SIGNAL(realReleased()), this, SLOT(_upReleased()));

    _down = new IButton();
    _down->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _down->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(_down, SIGNAL(realReleased()), this, SLOT(_downReleased()));

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMargin(10);
    hLayout->addWidget(_up);
    hLayout->addWidget(_down);

    contentLayout->addWidget(_reviewTable);
    contentLayout->addStretch();
    contentLayout->addLayout(hLayout);
    contentLayout->addStretch();
}

QString HistoryDataSelWidget::_convertTimeStr(const QString &str)
{
    QString timeStr;
    QDateTime dt = QDateTime::fromString(str, "yyyyMMddHHmmss");
    timeDate.getDateTime(dt.toTime_t(), timeStr, true, true);
    return timeStr;
}

void HistoryDataSelWidget::_caclInfo()
{
    _strList.clear();
    QStringList list;
    dataStorageDirManager.getRescueEvent(list);
    _strList.append(list);
    list.clear();

    int count = _strList.count();
    int row = count / 2;
    row += (count % 2) ? 1 : 0;
    _reviewTable->setRowCount(row);
    for (int i = 0; i < row; i ++)
    {
        QTableWidgetItem *item;
        _reviewTable->setRowHeight(i, ITEM_HEIGHT);
        item = _reviewTable->item(i, 0);
        if (!item)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            _reviewTable->setItem(i, 0, item);
        }
        int index = i * 2;
        if (index > count - 1)
        {
            break;
        }
        QString timeStr = _convertTimeStr(_strList.at(index));
        item->setText(timeStr);

        item = _reviewTable->item(i, 1);
        if (!item)
        {
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            _reviewTable->setItem(i, 1, item);
        }
        index = i * 2 + 1;
        if (index > count - 1)
        {
            break;
        }
        timeStr = _convertTimeStr(_strList.at(index));
        item->setText(timeStr);
    }
}
