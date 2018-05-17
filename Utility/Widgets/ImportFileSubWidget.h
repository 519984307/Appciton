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
    explicit ImportSubWidget(const QStringList &iListWidgetName);
public slots:
    void onConfigClickImport();
    void onExitList(bool);
    void getSelsectItems(){done(1);}
public:
     QList<QListWidgetItem*>  selectItemsImport;
     IListWidget             *myIListWidget;
     IButton                 *ibutton;
     QList<QListWidgetItem*> &readSelectItemsImport();
};
