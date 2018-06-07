#include "ImportFileSubWidget.h"



void ImportSubWidget::onExitList(bool backTab)
{
    if(backTab)
    {
        focusPreviousChild();
    }
    else
    {
        focusNextChild();
    }
}

void ImportSubWidget::onConfigClickImport()
{
    QListWidgetItem *item = myIListWidget->currentItem();
    /*加入链表*/
    int indexFlag = 0;
    if(!selectItemsImport.isEmpty())/*链表不为空，进入比较*/
    {
        for(int index = 0; index < selectItemsImport.count(); index ++)/*轮询比较是否再次选中对应选择项*/
        {
            if(selectItemsImport.at(index)==item)/*只能进行指针比较，不能变量比较*/
            {
                item->setIcon(QIcon());
                selectItemsImport.removeAt(index);
                indexFlag = 1;
                break;
            }
        }
        if(indexFlag == 0)
        {
            selectItemsImport.append(item);/*将选择项指针压入链表中*/
            item->setIcon(QIcon("/usr/local/nPM/icons/select.png"));
        }
        indexFlag = 0;/*复位标志位*/
    }
    else
    {
        selectItemsImport.append(item);/*将选择项指针压入链表中*/
        if(item!=NULL)
        {
            item->setIcon(QIcon("/usr/local/nPM/icons/select.png"));
        }
    }
}

QList<QListWidgetItem*>& ImportSubWidget::readSelectItemsImport()
{
    return selectItemsImport;
}

ImportSubWidget::ImportSubWidget(const QStringList &iListWidgetName): PopupWidget()
{
    //标题栏
    setTitleBarText(trs("Import Files"));

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(10, 0, 40, 0);

    //列表说明
    QLabel *l = new QLabel(trs("Select Files"));
    l->setFont(fontManager.textFont(15));
    hlayout->addWidget(l,0,Qt::AlignHCenter);
    hlayout->addStretch();
    vlayout->addLayout(hlayout);


    myIListWidget = new IListWidget;

    myIListWidget->addItems(iListWidgetName);

    myIListWidget->setFont(fontManager.textFont(15));
    myIListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    myIListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    myIListWidget->setFrameStyle(QFrame::Plain);
    myIListWidget->setSpacing(2);
    myIListWidget->setUniformItemSizes(true);
    myIListWidget->setIconSize(QSize(16,16));

    QString configListStyleSheet = QString("QListWidget { margin-left: 15px; border:1px solid #808080; border-radius: 2px; background-color: transparent; outline: none; }\n "
    "QListWidget::item {padding: 5px; border-radius: 2px; border: none; background-color: %1;}\n"
    "QListWidget::item:focus {background-color: %2;}").arg("white").arg(colorManager.getHighlight().name());

    myIListWidget->setStyleSheet(configListStyleSheet);
    connect(myIListWidget, SIGNAL(exitList(bool)), this, SLOT(onExitList(bool)));
    connect(myIListWidget, SIGNAL(realRelease()), this, SLOT(onConfigClickImport()));
    myIListWidget->setFixedHeight(174); //size for 5 items
    vlayout->addWidget(myIListWidget);
    vlayout->addStretch();

    ibutton = new IButton(trs("Ok"));
    ibutton->setFont(fontManager.textFont(15));
    connect(ibutton, SIGNAL(realReleased()), this, SLOT(getSelsectItems()));


    vlayout->addWidget(ibutton,0,Qt::AlignHCenter);
    vlayout->addStretch();
    contentLayout->addLayout(vlayout);
    QDesktopWidget *w = QApplication::desktop();
    setFixedSize(w->width() / 3, w->height() / 4);
}
