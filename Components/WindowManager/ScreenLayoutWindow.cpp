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
#include "ScreenLayoutItemDelegate.h"
#include "Button.h"
#include "LanguageManager.h"

class ScreenLayoutWindowPrivate
{
public:
    ScreenLayoutWindowPrivate()
        : view(NULL),
          resetBtn(NULL),
          saveBtn(NULL)
    {}
    TableView *view;
    Button *resetBtn;
    Button *saveBtn;
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
    d_ptr->view->setItemDelegate(new ScreenLayoutItemDelegate(d_ptr->view));
    layout->addWidget(d_ptr->view, 1);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addStretch(1);
    d_ptr->resetBtn = new Button(trs("ResetLayout"));
    d_ptr->resetBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->resetBtn->setMinimumWidth(120);
    connect(d_ptr->resetBtn, SIGNAL(clicked(bool)), this, SLOT(onButtonClicked()));
    hlayout->addWidget(d_ptr->resetBtn);

    d_ptr->saveBtn = new Button(trs("SaveLayout"));
    d_ptr->saveBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->saveBtn->setMinimumWidth(120);
    connect(d_ptr->saveBtn, SIGNAL(clicked(bool)), this, SLOT(onButtonClicked()));
    hlayout->addWidget(d_ptr->saveBtn);
    hlayout->addStretch(1);

    layout->addLayout(hlayout);


    ScreenLayoutModel *model = new ScreenLayoutModel(this);
    model->updateWaveAndParamInfo();
    d_ptr->view->setModel(model);
    resize(800, 600);

    int row = model->rowCount();
    int column = model->columnCount();
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            QSize span = model->span(model->index(i, j));
            if (span.isValid())
            {
                d_ptr->view->setSpan(i, j, span.height(), span.width());
            }
        }
    }

    setWindowTitle(trs("ScreenLayout"));
}

ScreenLayoutWindow::~ScreenLayoutWindow()
{
    delete d_ptr;
}

void ScreenLayoutWindow::onButtonClicked()
{
    Button *btn = qobject_cast<Button *>(sender());

    if (btn == d_ptr->resetBtn)
    {
        // TODO
    }
    else if (btn == d_ptr->saveBtn)
    {
        // TODO
    }
}
