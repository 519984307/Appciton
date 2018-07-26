/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/25
 **/

#include "TrendTableSetWindow.h"
#include "LanguageManager.h"
#include "Button.h"
#include "ComboBox.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "TrendDataSymbol.h"
#include "TrendTableWindow.h"
#include "IConfig.h"

TrendTableSetWindow *TrendTableSetWindow::selfObj = NULL;

class TrendTableSetWindowPrivate
{
public:
    TrendTableSetWindowPrivate()
        : resolutionRatioCbo(NULL), trendGroupCbo(NULL), yesBtn(NULL)
    {}

    ComboBox *resolutionRatioCbo;
    ComboBox *trendGroupCbo;
    Button *yesBtn;
};

TrendTableSetWindow::~TrendTableSetWindow()
{
}

void TrendTableSetWindow::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);

    QString prefix = "TrendTable|";
    int index = 0;

    QString ratioPrefix = prefix + "ResolutionRatio";
    systemConfig.getNumValue(ratioPrefix, index);
    d_ptr->resolutionRatioCbo->setCurrentIndex(index);

    QString groupPrefix = prefix + "TrendGroup";
    systemConfig.getNumValue(groupPrefix, index);
    d_ptr->trendGroupCbo->setCurrentIndex(index);
}

void TrendTableSetWindow::timeIntervalReleased(int t)
{
    QString prefix = "TrendTable|ResolutionRatio";
    systemConfig.setNumValue(prefix, t);
    trendTableWindow.setTimeInterval(t);
}

void TrendTableSetWindow::trendGroupReleased(int g)
{
    QString prefix = "TrendTable|TrendGroup";
    systemConfig.setNumValue(prefix, g);
    trendTableWindow.setTrendGroup(g);
}

void TrendTableSetWindow::yesReleased()
{
    hide();
    trendTableWindow.show();
}

TrendTableSetWindow::TrendTableSetWindow()
    : Window(), d_ptr(new TrendTableSetWindowPrivate())
{
    setWindowTitle(trs("TrendTableSet"));

    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *hLayout;
    QLabel *label;

    label = new QLabel(trs("ResolutionRatio"));
    d_ptr->resolutionRatioCbo = new ComboBox();
    for (int i = 0; i < RESOLUTION_RATIO_NR; i ++)
    {
        d_ptr->resolutionRatioCbo->addItem(trs(TrendDataSymbol::convert((ResolutionRatio)i)));
    }
    connect(d_ptr->resolutionRatioCbo, SIGNAL(currentIndexChanged(int)), this, SLOT(timeIntervalReleased(int)));
    hLayout = new QHBoxLayout();
    hLayout->addWidget(label);
    hLayout->addWidget(d_ptr->resolutionRatioCbo);
    layout->addLayout(hLayout);

    label = new QLabel(trs("TrendGroup"));
    d_ptr->trendGroupCbo = new ComboBox();
    d_ptr->trendGroupCbo->addItems(QStringList()
                                   << "Resp"
                                   << "IBP"
                                   << "AG"
                                  );
    connect(d_ptr->trendGroupCbo, SIGNAL(currentIndexChanged(int)), this, SLOT(trendGroupReleased(int)));
    hLayout = new QHBoxLayout();
    hLayout->addWidget(label);
    hLayout->addWidget(d_ptr->trendGroupCbo);
    layout->addLayout(hLayout);

    d_ptr->yesBtn = new Button(trs("EnglishYESChineseSURE"));
    d_ptr->yesBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->yesBtn, SIGNAL(released()), this, SLOT(yesReleased()));
    layout->addWidget(d_ptr->yesBtn);

    setWindowLayout(layout);
}
