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

class CCHDWindowPrivate
{
public:
    CCHDWindowPrivate()
        : handLabel(NULL),
          footLabel(NULL),
          handButton(NULL),
          footButton(NULL),
          resultLabel(NULL),
          handValue(InvData()),
          footValue(InvData()),
          measureTimes(0)
    {}
    ~CCHDWindowPrivate(){}

    void loadOption();
    void update();

    QLabel *handLabel;
    QLabel *footLabel;
    Button *handButton;
    Button *footButton;
    QLabel *resultLabel;
    short handValue;
    short footValue;
    int measureTimes;
};

CCHDWindow::CCHDWindow()
    : Dialog(),
      d_ptr(new CCHDWindowPrivate())
{
    setWindowTitle(trs("CCHDCheck"));
    setFixedSize(400, 200);
    QGridLayout *layout = new QGridLayout();
    int row = 0;
    // 手部测量
    QLabel *lable = new QLabel;
    lable->setText(trs("handMeasure"));
    layout->addWidget(lable, row, 0);
    d_ptr->handButton = new Button;
    d_ptr->handButton->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->handButton->setText(trs("Start"));
    connect(d_ptr->handButton, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(d_ptr->handButton, row, 1);
    d_ptr->handLabel = new QLabel;
    d_ptr->handLabel->setText(InvStr());
    layout->addWidget(d_ptr->handLabel, row, 2);
    row++;

    // 足部测量
    lable = new QLabel;
    lable->setText(trs("footMeasure"));
    layout->addWidget(lable, row, 0);
    d_ptr->footButton = new Button;
    d_ptr->footButton->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->footButton->setText(trs("Start"));
    connect(d_ptr->footButton, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(d_ptr->footButton, row, 1);
    d_ptr->footLabel = new QLabel;
    d_ptr->footLabel->setText(InvStr());
    layout->addWidget(d_ptr->footLabel, row, 2);
    row++;

    // 结果
    d_ptr->resultLabel = new QLabel;
    d_ptr->resultLabel->setText(QString("%1: %2").arg(trs("Result")).arg(InvStr()));
    d_ptr->resultLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(d_ptr->resultLabel, row, 1);

    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 2);
    layout->setColumnStretch(2, 1);
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

void CCHDWindow::onButtonReleased()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == d_ptr->handButton)
    {
        d_ptr->handValue = spo2Param.getSPO2();
        if (d_ptr->handValue != InvData())
        {
            btn->setEnabled(false);
        }
    }
    else if (btn == d_ptr->footButton)
    {
        d_ptr->footValue = spo2Param.getSPO2();
        if (d_ptr->footValue != InvData())
        {
            btn->setEnabled(false);
        }
    }
    d_ptr->update();
}

void CCHDWindowPrivate::loadOption()
{
    handLabel->setText(InvStr());
    footLabel->setText(InvStr());
    handButton->setEnabled(true);
    footButton->setEnabled(true);
}

void CCHDWindowPrivate::update()
{
    if (handValue == InvData())
    {
        handLabel->setText(InvStr());
    }
    else
    {
        handLabel->setText(QString::number(handValue));
    }
    if (footValue == InvData())
    {
        footLabel->setText(InvStr());
    }
    else
    {
        footLabel->setText(QString::number(footValue));
    }

    if ((handValue >= 95 && (footValue >= 95 || abs(footValue - handValue) <= 3)) ||
            (footValue >= 95 && (handValue >= 95 || abs(footValue - handValue) <= 3)))
    {
        // 阴性
        resultLabel->setText(QString("%1: %2").arg(trs("Result")).arg(trs(SPO2Symbol::convert(Negative))));
        measureTimes = 0;
    }
    else if ((handValue >= 90 && handValue <= 94) &&
             ((footValue >= 90 && footValue <= 94) || (abs(handValue - footValue) > 3))
             )
    {
        // 重复测试判断是否为阳性
        measureTimes++;
        if (measureTimes < 3)
        {
            loadOption();
        }
        else
        {
            measureTimes = 0;
            resultLabel->setText(QString("%1: %2").arg(trs("Result")).arg(trs(SPO2Symbol::convert(Positive))));
        }
    }
    else
    {
        // 阳性
        resultLabel->setText(QString("%1: %2").arg(trs("Result")).arg(trs(SPO2Symbol::convert(Positive))));
    }
}
