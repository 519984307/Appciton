#include "OxyCRGEventWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "ITableWidget.h"
#include "IButton.h"
#include "IDropList.h"
#include "IMoveButton.h"
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>

#define ITEM_HEIGHT     30
#define ITEM_WIDTH      100

OxyCRGEventWidget *OxyCRGEventWidget::_selfObj = NULL;

class OxyCRGEventWidgetPrivate
{
public:


    ITableWidget *eventTable;
    IButton *detail;
    IDropList *type;
    IButton *upTable;
    IButton *downTable;

    QWidget *tableWidget;
    QWidget *chartWidget;
    QStackedLayout *stackLayout;
};

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGEventWidget::~OxyCRGEventWidget()
{

}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void OxyCRGEventWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);

}

void OxyCRGEventWidget::_upMoveEventReleased()
{

}

void OxyCRGEventWidget::_downMoveEventReleased()
{

}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
OxyCRGEventWidget::OxyCRGEventWidget() : d_ptr(new OxyCRGEventWidgetPrivate())
{
    int windowWidth = windowManager.getPopMenuWidth();
    int windowHeight = windowManager.getPopMenuHeight();
    setFixedSize(windowWidth, windowHeight);

    int fontSize = fontManager.getFontSize(1);
    QFont font = fontManager.textFont(fontSize);

    d_ptr->eventTable = new ITableWidget();
    d_ptr->eventTable->setColumnCount(2);
    d_ptr->eventTable->setFocusPolicy(Qt::NoFocus);
    d_ptr->eventTable->verticalHeader()->setVisible(false);
    d_ptr->eventTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d_ptr->eventTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->eventTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    d_ptr->eventTable->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->eventTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    QStringList tableTitle;
    tableTitle << trs("Time") << trs("Incident");
    d_ptr->eventTable->setHorizontalHeaderLabels(tableTitle);

    d_ptr->detail = new IButton(trs("WaveInfo"));
    d_ptr->detail->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->detail->setFont(font);

    d_ptr->type = new IDropList(trs("Type"));
    d_ptr->type->setFixedSize(ITEM_WIDTH, ITEM_H);
    d_ptr->type->setFont(font);

    d_ptr->upTable = new IButton();
    d_ptr->upTable->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    d_ptr->upTable->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(d_ptr->upTable, SIGNAL(realReleased()), this, SLOT(_upMoveEventReleased()));

    d_ptr->downTable = new IButton();
    d_ptr->downTable->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    d_ptr->downTable->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(d_ptr->downTable, SIGNAL(realReleased()), this,  SLOT(_downMoveEventReleased()));

    QHBoxLayout *hTableLayout = new QHBoxLayout();
    hTableLayout->setMargin(0);
    hTableLayout->setSpacing(2);
    hTableLayout->addWidget(d_ptr->detail);
    hTableLayout->addWidget(d_ptr->type);
    hTableLayout->addWidget(d_ptr->upTable);
    hTableLayout->addWidget(d_ptr->downTable);

    QVBoxLayout *vTableLayout = new QVBoxLayout();
    vTableLayout->addWidget(d_ptr->eventTable);
    vTableLayout->addLayout(hTableLayout);

    d_ptr->tableWidget = new QWidget();
    d_ptr->tableWidget->setLayout(vTableLayout);


    d_ptr->chartWidget = new QWidget();


    d_ptr->stackLayout = new QStackedLayout();
    d_ptr->stackLayout->addWidget(d_ptr->tableWidget);
    d_ptr->stackLayout->addWidget(d_ptr->chartWidget);

    contentLayout->addLayout(d_ptr->stackLayout);
}
