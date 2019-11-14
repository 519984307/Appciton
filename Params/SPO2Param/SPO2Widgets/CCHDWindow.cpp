/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/11
 **/

#include "CCHDWindow.h"
#include "Framework/UI/Button.h"
#include "Framework/UI/TableView.h"
#include "Framework/UI/TableHeaderView.h"
#include "Framework/UI/TableViewItemDelegate.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QLayout>
#include "BaseDefine.h"
#include "SPO2Param.h"
#include "SPO2Symbol.h"
#include "CCHDDataModel.h"
#include "MessageBox.h"

class CCHDWindowPrivate
{
public:
    CCHDWindowPrivate()
        : handLabel(NULL),
          footLabel(NULL),
          handButton(NULL),
          footButton(NULL),
          resultLabel(NULL),
          tableView(NULL),
          model(NULL),
          handValue(InvData()),
          footValue(InvData())
    {}
    ~CCHDWindowPrivate(){}

    void loadOption();
    void update();

    QLabel *handLabel;
    QLabel *footLabel;
    Button *handButton;
    Button *footButton;
    QLabel *resultLabel;
    TableView *tableView;
    CCHDDataModel *model;
    short handValue;
    short footValue;
};

CCHDWindow::CCHDWindow()
    : Dialog(),
      d_ptr(new CCHDWindowPrivate())
{
    int windowWidth = 700;
    setWindowTitle(trs("CCHDCheck"));
    setFixedSize(windowWidth, 330);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(5);

    // result Data
    d_ptr->tableView = new TableView();
    d_ptr->tableView->setShowGrid(true);
    d_ptr->model = new CCHDDataModel();
    TableHeaderView *hTableHeader = new TableHeaderView(Qt::Horizontal);
    hTableHeader->setResizeMode(QHeaderView::ResizeToContents);
    d_ptr->tableView->setHorizontalHeader(hTableHeader);
    d_ptr->tableView->verticalHeader()->setVisible(false);
    d_ptr->tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    d_ptr->tableView->setFocusPolicy(Qt::NoFocus);
    d_ptr->tableView->setSelectionMode(QAbstractItemView::NoSelection);
    d_ptr->tableView->setShowGrid(true);
    d_ptr->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->model->setTableViewWidth(windowWidth - layout->margin() * 2);
    d_ptr->tableView->setModel(d_ptr->model);
    d_ptr->tableView->setItemDelegate(new TableViewItemDelegate(this));
    int rowCount = d_ptr->model->rowCount(QModelIndex()) + 1;
    d_ptr->tableView->setFixedHeight(rowCount * d_ptr->model->getHeightEachRow());
    layout->addWidget(d_ptr->tableView);

    QHBoxLayout *hLayout = new QHBoxLayout();
    // 手部测量
    d_ptr->handButton = new Button;
    d_ptr->handButton->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->handButton->setText(trs("SpO2Value"));
    connect(d_ptr->handButton, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(d_ptr->handButton);

    // 足部测量
    d_ptr->footButton = new Button;
    d_ptr->footButton->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->footButton->setText(trs("SpO2_2Value"));
    connect(d_ptr->footButton, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(d_ptr->footButton);

    // 清空
    Button *cleanBtn = new Button;
    cleanBtn->setButtonStyle(Button::ButtonTextOnly);
    cleanBtn->setText(trs("Clear"));
    connect(cleanBtn, SIGNAL(released()), this, SLOT(onButtonReleased()));
    hLayout->addWidget(cleanBtn);

    layout->addStretch();
    layout->addLayout(hLayout);
    setWindowLayout(layout);
}

CCHDWindow::~CCHDWindow()
{
    delete d_ptr;
}

void CCHDWindow::showEvent(QShowEvent *e)
{
    d_ptr->loadOption();
    Dialog::showEvent(e);
}

void CCHDWindow::hideEvent(QHideEvent *ev)
{
    spo2Param.clearCCHDData();
    Dialog::hideEvent(ev);
}

void CCHDWindow::onButtonReleased()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == d_ptr->handButton)
    {
        if (spo2Param.getSPO2() == InvData())
        {
            MessageBox msgBox(trs("Prompt"), trs("SPO2ValueInvaild"), false, true);
            msgBox.exec();
            return;
        }
        d_ptr->handValue = spo2Param.getSPO2();
        btn->setEnabled(false);
        spo2Param.setCCHDData(d_ptr->handValue, true);
        d_ptr->update();
    }
    else if (btn == d_ptr->footButton)
    {
        if (spo2Param.getSPO2(true) == InvData())
        {
            MessageBox msgBox(trs("Prompt"), trs("SPO2ValueInvaild"), false, true);
            msgBox.exec();
            return;
        }
        d_ptr->footValue = spo2Param.getSPO2(true);
        btn->setEnabled(false);
        spo2Param.setCCHDData(d_ptr->footValue, false);
        d_ptr->update();
    }
    else
    {
        d_ptr->handValue = InvData();
        d_ptr->footValue = InvData();
        d_ptr->handButton->setEnabled(true);
        d_ptr->footButton->setEnabled(true);
        spo2Param.clearCCHDData(true);
        d_ptr->model->updateData();
    }
}

void CCHDWindowPrivate::loadOption()
{
    model->updateData();
}

void CCHDWindowPrivate::update()
{
    CCHDResult result = spo2Param.updateCCHDResult();
    if (result == RepeatCheck)
    {
        handValue = InvData();
        footValue = InvData();
        handButton->setEnabled(true);
        footButton->setEnabled(true);
    }
    model->updateData();
}
