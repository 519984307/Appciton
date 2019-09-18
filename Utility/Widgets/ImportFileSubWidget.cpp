/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/6
 **/

#include "ImportFileSubWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include "Button.h"
#include "LanguageManager.h"
#include "ListView.h"
#include "ListViewItemDelegate.h"
#include "ListDataModel.h"
#include <QMap>

#define LISTVIEW_MAX_VISIABLE_TIME 5

class ImportSubWidgetPrivate
{
public:
    ImportSubWidgetPrivate();

    Button *oKBtn;
    ListView *importListView;
    ListDataModel *model;
    QMap<int, bool> rowsMap;
};

ImportSubWidgetPrivate::ImportSubWidgetPrivate()
    : oKBtn(NULL),
      importListView(NULL),
      model(NULL)
{
    rowsMap.clear();
}

ImportSubWidget::ImportSubWidget(const QStringList &iListWidgetName, const QString &path)
    : Dialog(),
      d_ptr(new ImportSubWidgetPrivate)
{
    setWindowTitle(trs("ImportFiles"));
    setFixedSize(480, 450);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignTop);

    QLabel *label;

    label = new QLabel(path);
    layout->addWidget(label);

    ListView *listView = new ListView;
    ListDataModel *model = new ListDataModel(this);
    listView->setItemDelegate(new ListViewItemDelegate(listView));
    listView->setFixedHeight(LISTVIEW_MAX_VISIABLE_TIME * model->getRowHeightHint()
                             + listView->spacing() * (LISTVIEW_MAX_VISIABLE_TIME * 2));
    model->setStringList(iListWidgetName);
    listView->setModel(model);
    listView->setRowsSelectMode(true);
    connect(listView, SIGNAL(enterSignal()), this, SLOT(updateBtnStatus()));
    layout->addWidget(listView);
    d_ptr->importListView = listView;
    d_ptr->model = model;

    Button *button = new Button(trs("Ok"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(getSelsectItems()));
    layout->addWidget(button, 1, Qt::AlignRight);
    button->setFixedWidth(100);
    button->setEnabled(false);
    d_ptr->oKBtn = button;

    layout->addStretch(1);
    setWindowLayout(layout);
}

void ImportSubWidget::getSelsectItems()
{
    d_ptr->rowsMap = d_ptr->importListView->getRowsSelectMap();
    done(1);
}

QMap<int, bool> &ImportSubWidget::readRowsMap()
{
    return d_ptr->rowsMap;
}

void ImportSubWidget::updateBtnStatus()
{
    int row = d_ptr->importListView->curCheckedRow();
    bool isEnabled;
    if (row == -1)
    {
        isEnabled = false;
    }
    else
    {
        isEnabled = true;
    }
    d_ptr->oKBtn->setEnabled(isEnabled);
}
