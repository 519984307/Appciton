/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/25
 **/

#include "BigFontLayoutWindow.h"
#include "TableView.h"
#include "BigFontLayoutModel.h"
#include "ScreenLayoutItemDelegate.h"
#include <QLayout>
#include <QHeaderView>
#include "Button.h"
#include "LanguageManager.h"
#include "LayoutManager.h"
#include "MessageBox.h"
#include "WindowManager.h"

class BigFontLayoutWindowPrivate
{
public:
    BigFontLayoutWindowPrivate()
        : tableView(NULL),
          resetBtn(NULL),
          defaultBtn(NULL),
          saveBtn(NULL)
    {}
    ~BigFontLayoutWindowPrivate(){}

    TableView *tableView;
    Button *resetBtn;
    Button *defaultBtn;
    Button *saveBtn;
};


BigFontLayoutWindow *BigFontLayoutWindow::getInstance()
{
    static BigFontLayoutWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new BigFontLayoutWindow();
    }
    return instance;
}

BigFontLayoutWindow::~BigFontLayoutWindow()
{
    delete d_ptr;
}

void BigFontLayoutWindow::hideEvent(QHideEvent *ev)
{
    Window::hideEvent(ev);
    BigFontLayoutModel *model = qobject_cast<BigFontLayoutModel*>(d_ptr->tableView->model());
    if (model->isChangeLayoutInfo())
    {
        MessageBox message(trs("Prompt"), trs("isSaveLayout"));
        int ret = message.exec();
        if (ret == QDialog::Accepted)
        {
            model->saveLayoutInfo();
            layoutManager.updateLayout();
        }
    }
}

void BigFontLayoutWindow::showEvent(QShowEvent *ev)
{
    BigFontLayoutModel *model = qobject_cast<BigFontLayoutModel *>(d_ptr->tableView->model());
    model->loadLayoutInfo();
    Window::showEvent(ev);
}

void BigFontLayoutWindow::onButtonClicked()
{
    Button *btn = qobject_cast<Button *>(sender());

    BigFontLayoutModel *model = qobject_cast<BigFontLayoutModel*>(d_ptr->tableView->model());
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
        layoutManager.updateLayout();
        this->close();
    }
    else if (btn == d_ptr->defaultBtn)
    {
        model->loadLayoutInfo(true);
    }
}

BigFontLayoutWindow::BigFontLayoutWindow()
    : Window(),
      d_ptr(new BigFontLayoutWindowPrivate())
{
    QVBoxLayout *vLayout = new QVBoxLayout();
    setWindowLayout(vLayout);

    d_ptr->tableView = new TableView();
    d_ptr->tableView->verticalHeader()->setVisible(false);
    d_ptr->tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    d_ptr->tableView->horizontalHeader()->setVisible(false);
    d_ptr->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    d_ptr->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    d_ptr->tableView->setItemDelegate(new ScreenLayoutItemDelegate(d_ptr->tableView));
    d_ptr->tableView->setShowGrid(false);

    vLayout->addWidget(d_ptr->tableView);

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

    vLayout->addLayout(hlayout);


    BigFontLayoutModel *model = new BigFontLayoutModel(this);
    model->updateWaveAndParamInfo();
    model->loadLayoutInfo();
    d_ptr->tableView->setModel(model);
    connect(model, SIGNAL(spanChanged(QModelIndex)), d_ptr->tableView, SLOT(onSpanChanged(QModelIndex)));

    setFixedSize(windowManager.getPopMenuWidth(), windowManager.getPopMenuHeight());

    setWindowTitle(trs("BigFontScreenLayout"));
}
