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
#include "Framework/Language/LanguageManager.h"
#include "Button.h"
#include "ComboBox.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "TrendDataSymbol.h"
#include "TrendTableWindow.h"
#include "IConfig.h"
#include "WindowManager.h"
#include "SystemManager.h"

TrendTableSetWindow *TrendTableSetWindow::selfObj = NULL;

class TrendTableSetWindowPrivate
{
public:
    TrendTableSetWindowPrivate()
        : resolutionRatioCbo(NULL), trendGroupCbo(NULL)
    {}

    ComboBox *resolutionRatioCbo;
    ComboBox *trendGroupCbo;
};

TrendTableSetWindow::~TrendTableSetWindow()
{
}

void TrendTableSetWindow::showEvent(QShowEvent *ev)
{
    Dialog::showEvent(ev);

    QString prefix = "TrendTable|";
    int index = 0;

    QString ratioPrefix = prefix + "ResolutionRatio";
    systemConfig.getNumValue(ratioPrefix, index);
    if (index == RESOLUTION_RATIO_NIBP && systemManager.isSupport(CONFIG_NIBP) == false)
    {
        index = 0;
    }
    d_ptr->resolutionRatioCbo->blockSignals(true);
    d_ptr->resolutionRatioCbo->setCurrentIndex(index);
    d_ptr->resolutionRatioCbo->blockSignals(false);

    QString groupPrefix = prefix + "TrendGroup";
    systemConfig.getNumValue(groupPrefix, index);
    int count = d_ptr->trendGroupCbo->count();
    if (index >= count)
    {
        index = count - 1;
    }
    d_ptr->trendGroupCbo->blockSignals(true);
    d_ptr->trendGroupCbo->setCurrentIndex(index);
    d_ptr->trendGroupCbo->blockSignals(false);
}

void TrendTableSetWindow::timeIntervalReleased(int t)
{
    QString prefix = "TrendTable|ResolutionRatio";
    systemConfig.setNumValue(prefix, t);
    TrendTableWindow::getInstance()->setTimeInterval(t);
}

void TrendTableSetWindow::trendGroupReleased(int g)
{
    QString prefix = "TrendTable|TrendGroup";
    systemConfig.setNumValue(prefix, g);
    TrendTableWindow::getInstance()->setTrendGroup(g);
}

TrendTableSetWindow::TrendTableSetWindow()
    : Dialog(), d_ptr(new TrendTableSetWindowPrivate())
{
    setFixedSize(340, 240);

    setWindowTitle(trs("TrendTableSet"));

    QGridLayout *gridLayout = new QGridLayout();
    QLabel *label;

    label = new QLabel(trs("ResolutionRatio"));
    d_ptr->resolutionRatioCbo = new ComboBox();
    for (int i = 0; i < RESOLUTION_RATIO_NR; i ++)
    {
        if (i == RESOLUTION_RATIO_NIBP && systemManager.isSupport(CONFIG_NIBP) == false)
        {
            continue;
        }

        d_ptr->resolutionRatioCbo->addItem(trs(TrendDataSymbol::convert((ResolutionRatio)i)));
    }
    connect(d_ptr->resolutionRatioCbo, SIGNAL(currentIndexChanged(int)), this, SLOT(timeIntervalReleased(int)));
    gridLayout->addWidget(label, 0, 0);
    gridLayout->addWidget(d_ptr->resolutionRatioCbo, 0, 1);

    label = new QLabel(trs("TrendGroup"));
    d_ptr->trendGroupCbo = new ComboBox();
    d_ptr->trendGroupCbo->setEnabled(false);    // 暂时置灰趋势组选择功能(目前不支持IBP,AG参数功能)
    d_ptr->trendGroupCbo->addItem("Resp");
    if (systemManager.isSupport(CONFIG_IBP))
    {
        d_ptr->trendGroupCbo->addItem("IBP");
    }
    if (systemManager.isSupport(CONFIG_AG))
    {
        d_ptr->trendGroupCbo->addItem("AG");
    }
    connect(d_ptr->trendGroupCbo, SIGNAL(currentIndexChanged(int)), this, SLOT(trendGroupReleased(int)));
    gridLayout->addWidget(label, 1, 0);
    gridLayout->addWidget(d_ptr->trendGroupCbo, 1, 1);

    setWindowLayout(gridLayout);
}
