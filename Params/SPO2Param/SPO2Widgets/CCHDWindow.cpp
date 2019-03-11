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
#include "Button.h"
#include <QLabel>
#include "LanguageManager.h"
#include <QGridLayout>
#include "BaseDefine.h"
#include "SPO2Param.h"
#include "SPO2Symbol.h"
#include "TableView.h"
#include "CCHDDataModel.h"
#include "TableViewItemDelegate.h"
#include "TableHeaderView.h"

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
    setWindowTitle(trs("CCHDCheck"));
    setFixedSize(400, 300);
    QGridLayout *layout = new QGridLayout();
    int row = 0;

    // 结果
    d_ptr->resultLabel = new QLabel;
    d_ptr->resultLabel->setText(QString("%1: %2").arg(trs("Result")).arg(InvStr()));
    d_ptr->resultLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(d_ptr->resultLabel, row, 0, 1, 0);
    row++;

    // result Data
    d_ptr->tableView = new TableView();
    d_ptr->tableView->setShowGrid(true);
    d_ptr->model = new CCHDDataModel();
    TableHeaderView *hTableHeader = new TableHeaderView(Qt::Horizontal);
    hTableHeader->setResizeMode(QHeaderView::Stretch);
    d_ptr->tableView->setHorizontalHeader(hTableHeader);
    d_ptr->tableView->verticalHeader()->setVisible(false);
    d_ptr->tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    d_ptr->tableView->setFocusPolicy(Qt::NoFocus);
    d_ptr->tableView->setSelectionMode(QAbstractItemView::NoSelection);
    d_ptr->tableView->setShowGrid(true);
    d_ptr->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->tableView->setModel(d_ptr->model);
    d_ptr->tableView->setItemDelegate(new TableViewItemDelegate(this));
    int rowCount = d_ptr->model->rowCount(QModelIndex());
    d_ptr->tableView->setFixedHeight(rowCount * d_ptr->model->getHeightEachRow());
    layout->addWidget(d_ptr->tableView, row, 0, 1, 0);
    row++;

    // 手部测量
    d_ptr->handButton = new Button;
    d_ptr->handButton->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->handButton->setText(trs("handMeasure"));
    connect(d_ptr->handButton, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(d_ptr->handButton, row, 0);

    // 足部测量
    d_ptr->footButton = new Button;
    d_ptr->footButton->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->footButton->setText(trs("footMeasure"));
    connect(d_ptr->footButton, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(d_ptr->footButton, row, 1);
    row++;

    layout->setRowStretch(row, 1);
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
        d_ptr->handValue = spo2Param.getSPO2();
        if (d_ptr->handValue != InvData())
        {
            btn->setEnabled(false);
            d_ptr->model->addData(d_ptr->handValue, true);
        }
    }
    else if (btn == d_ptr->footButton)
    {
        d_ptr->footValue = spo2Param.getSPO2();
        if (d_ptr->footValue != InvData())
        {
            btn->setEnabled(false);
            d_ptr->model->addData(d_ptr->footValue, false);
        }
    }
    d_ptr->update();
}

void CCHDWindowPrivate::loadOption()
{
    model->addData(spo2Param.getCCHDDataList());
    handButton->setEnabled(true);
    footButton->setEnabled(true);
}

void CCHDWindowPrivate::update()
{
    CCHDResult result = spo2Param.getCCHDResult(handValue, footValue);
    switch (result)
    {
    case Negative:
        resultLabel->setText(QString("%1: %2").arg(trs("Result")).arg(trs(SPO2Symbol::convert(Negative))));
        break;
    case Positive:
        resultLabel->setText(QString("%1: %2").arg(trs("Result")).arg(trs(SPO2Symbol::convert(Positive))));
        break;
    case RepeatCheck:
        handValue = InvData();
        footValue = InvData();
        loadOption();
        resultLabel->setText(QString("%1: %2").arg(trs("Result")).arg(trs(SPO2Symbol::convert(RepeatCheck))));
        break;
    case CCHD_NR:
        resultLabel->setText(QString("%1: %2").arg(trs("Result")).arg(InvStr()));
        break;
    default:
        break;
    }
}
