/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/28
 **/

#include "ScreenLayoutWindow.h"
#include "TableView.h"
#include <QBoxLayout>
#include <QHeaderView>
#include "ScreenLayoutModel.h"

class ScreenLayoutWindowPrivate
{
public:
    ScreenLayoutWindowPrivate()
        : view(NULL)
    {}
    TableView *view;
};

ScreenLayoutWindow *ScreenLayoutWindow::getInstance()
{
    static ScreenLayoutWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new ScreenLayoutWindow();
    }
    return instance;
}


ScreenLayoutWindow::ScreenLayoutWindow()
    : Window(), d_ptr(new ScreenLayoutWindowPrivate())
{
    QVBoxLayout *layout = new QVBoxLayout();
    setWindowLayout(layout);
    d_ptr->view = new TableView();
    d_ptr->view->verticalHeader()->setVisible(false);
    d_ptr->view->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    d_ptr->view->horizontalHeader()->setVisible(false);
    d_ptr->view->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    d_ptr->view->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->view->setSelectionBehavior(QAbstractItemView::SelectItems);
    layout->addWidget(d_ptr->view, 1);

    QAbstractTableModel *model = new ScreenLayoutModel(this);
    d_ptr->view->setModel(model);
    resize(800, 600);

    int row = model->rowCount();
    int column = model->columnCount();
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            QSize span = model->span(model->index(i, j));
            d_ptr->view->setSpan(i, j, span.height(), span.width());
        }
    }
}

ScreenLayoutWindow::~ScreenLayoutWindow()
{
    delete d_ptr;
}
