#pragma once
#include "PopupWidget.h"
#include <QKeyEvent>
#include <QButtonGroup>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include "PopupWidget.h"
#include "IButton.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "SystemManager.h"
#include "WindowManager.h"
#include <QPixmap>
#include <QSignalMapper>
#include "ColorManager.h"
#include "ConfigExportImportMenu.h"

class ImportSubWidget :public PopupWidget
{
    Q_OBJECT
public:
    explicit ImportSubWidget(QStringList *iListWidgetName,QList<QListWidgetItem*>&listWidgetItem): PopupWidget()
    {
        //标题栏
        setTitleBarText(trs("Import File List"));

        QHBoxLayout *hlayout = new QHBoxLayout();
        hlayout->setContentsMargins(0, 0, 0, 0);
        QVBoxLayout *vlayout = new QVBoxLayout();
        vlayout->setContentsMargins(10, 0, 40, 0);
        //列表说明
        QLabel *l = new QLabel(trs("Import File"));
        l->setFont(fontManager.textFont(15));
        hlayout->addWidget(l,0,Qt::AlignHCenter);
        hlayout->addStretch();
        vlayout->addLayout(hlayout);


        myIListWidget = new IListWidget;

        myIListWidget->addItems(*iListWidgetName);

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
        connect(myIListWidget, SIGNAL(realRelease()), this, SLOT(onConfigClickImport()));
        myIListWidget->setFixedHeight(90); //size for 2.8 items
        vlayout->addWidget(myIListWidget);
        vlayout->addStretch();

        ibutton = new IButton(trs("Ok"));
        ibutton->setFont(fontManager.textFont(15));
        connect(ibutton, SIGNAL(realReleased()), this, SLOT(getSelsectItems()));


        vlayout->addWidget(ibutton,0,Qt::AlignHCenter);
        vlayout->addStretch();
        contentLayout->addLayout(vlayout);
        QDesktopWidget *w = QApplication::desktop();
        setFixedSize(w->width() / 4, w->height() / 6);
        selectItems = &listWidgetItem;
    }
 signals:
    void configClickSignals(IListWidget*);
    void selectItemSignal();
public slots:
    void onConfigClickImport();
    void getSelsectItems(){*selectItems = SelectItemsImport;done(1);}
public:
     QList<QListWidgetItem*> SelectItemsImport;
     QList<QListWidgetItem*> *selectItems;
     IListWidget *myIListWidget;
     IButton *ibutton;
};
