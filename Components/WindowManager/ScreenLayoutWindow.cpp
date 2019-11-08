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
#include "Framework/Language/LanguageManager.h"
#include "LayoutManager.h"
#include "MessageBox.h"
#include "WindowManager.h"

class ScreenLayoutWindowPrivate
{
public:
    ScreenLayoutWindowPrivate()
        : view(NULL),
          resetBtn(NULL),
          defaultBtn(NULL),
          saveBtn(NULL)
    {}
    TableView *view;
    Button *resetBtn;
    Button *defaultBtn;
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
    : Dialog(), d_ptr(new ScreenLayoutWindowPrivate())
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
    hlayout->setSpacing(20);
    hlayout->addStretch(1);
    d_ptr->resetBtn = new Button(trs("ResetLayout"));
    d_ptr->resetBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->resetBtn->setMinimumWidth(140);
    connect(d_ptr->resetBtn, SIGNAL(clicked(bool)), this, SLOT(onButtonClicked()));
    hlayout->addWidget(d_ptr->resetBtn);

    d_ptr->defaultBtn = new Button(trs("DefaultLayout"));
    d_ptr->defaultBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->defaultBtn->setMinimumWidth(140);
    connect(d_ptr->defaultBtn, SIGNAL(clicked(bool)), this, SLOT(onButtonClicked()));
    hlayout->addWidget(d_ptr->defaultBtn);

    d_ptr->saveBtn = new Button(trs("SaveLayout"));
    d_ptr->saveBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->saveBtn->setMinimumWidth(140);
    connect(d_ptr->saveBtn, SIGNAL(clicked(bool)), this, SLOT(onButtonClicked()));
    hlayout->addWidget(d_ptr->saveBtn);
    hlayout->addStretch(1);

    layout->addLayout(hlayout);


    ScreenLayoutModel *model = new ScreenLayoutModel(this);
    model->updateWaveAndParamInfo();
    model->loadLayoutInfo();
    d_ptr->view->setModel(model);
    connect(model, SIGNAL(spanChanged(QModelIndex)), d_ptr->view, SLOT(onSpanChanged(QModelIndex)));

    setFixedSize(windowManager.getPopWindowWidth(), windowManager.getPopWindowHeight());

    setWindowTitle(trs("StandardScreenLayout"));
}

ScreenLayoutWindow::~ScreenLayoutWindow()
{
    delete d_ptr;
}

void ScreenLayoutWindow::hideEvent(QHideEvent *ev)
{
    Dialog::hideEvent(ev);
    ScreenLayoutModel *model = qobject_cast<ScreenLayoutModel*>(d_ptr->view->model());
    if (model->isChangeLayoutInfo())
    {
        MessageBox message(trs("Prompt"), trs("isSaveLayout"));
        int ret = message.exec();
        if (ret == QDialog::Accepted)
        {
            model->saveLayoutInfo();
            layoutManager.reloadLayoutConfig();
        }
    }
}

void ScreenLayoutWindow::showEvent(QShowEvent *ev)
{
    ScreenLayoutModel *model = qobject_cast<ScreenLayoutModel *>(d_ptr->view->model());
    model->loadLayoutInfo();
    Dialog::showEvent(ev);
}

void ScreenLayoutWindow::onButtonClicked()
{
    Button *btn = qobject_cast<Button *>(sender());

    ScreenLayoutModel *model = qobject_cast<ScreenLayoutModel*>(d_ptr->view->model());
    if (!model)
    {
        return;
    }
    if (btn == d_ptr->resetBtn)
    {
        model->loadLayoutInfo();
    }
    else if (btn == d_ptr->saveBtn)
    {
        model->saveLayoutInfo();
        layoutManager.reloadLayoutConfig();
        this->close();
    }
    else if (btn == d_ptr->defaultBtn)
    {
        model->loadLayoutInfo(true);
    }
}
