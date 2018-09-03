/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/30
 **/

#include "TopBarWidget.h"
#include <QPainter>
#include <QLayout>
#include "IConfig.h"
#include "SystemModeBarWidget.h"

TopBarWidget *TopBarWidget::_selfObj = NULL;
class TopBarWidgetPrivate
{
public:
    TopBarWidgetPrivate() {}
    QMap<QString , IWidget *> widgetMap;
};

TopBarWidget::TopBarWidget()
    : IWidget()
    , d_ptr(new TopBarWidgetPrivate)
{
}

TopBarWidget::~TopBarWidget()
{
    delete d_ptr;
}

bool TopBarWidget::addWidget(IWidget *win)
{
    if (!win)
    {
        return false;
    }

    if (d_ptr->widgetMap.find(win->name()) != d_ptr->widgetMap.end())
    {
        return false;
    }

    d_ptr->widgetMap.insert(win->name(), win);

    return true;
}

void TopBarWidget::refreshLayout()
{
    _layout();
}

QColor TopBarWidget::getTopBarBlackGroundColor()
{
    return QColor(30, 30, 30);
}

IWidget *TopBarWidget::findWidget(QString str)
{
    QMap<QString , IWidget*>::iterator iter = d_ptr->widgetMap.find(str);
    if (iter != d_ptr->widgetMap.end())
    {
        return iter.value();
    }
    return NULL;
}

void TopBarWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    painter.fillRect(this->rect(), getTopBarBlackGroundColor());
}

void TopBarWidget::_layout()
{
    QMap<QString, IWidget *>::iterator it;
    QString prefix;

    QHBoxLayout *hLayoutTopBarRow = new QHBoxLayout();
    hLayoutTopBarRow->setMargin(0);
    hLayoutTopBarRow->setSpacing(0);

    QStringList topBarRow;
    prefix = "PrimaryCfg|UILayout|WidgetsOrder|topBarRowOrder";
    topBarRow = systemConfig.getChildNodeNameList(prefix);

    if (topBarRow.size() > 0)
    {
        IWidget *w = NULL;
        for (int i = 0; i < topBarRow.size(); i++)
        {
            it = d_ptr->widgetMap.find(topBarRow[i]);
            if (it == d_ptr->widgetMap.end())
            {
                continue;
            }

            w = it.value();
            w->setParent(this);            // 设置父窗体。
            w->setVisible(true);           // 可见。
            QString string = prefix + "|" + topBarRow[i];
            int index = 1;
            systemConfig.getNumValue(string, index);
            hLayoutTopBarRow->addWidget(w, index);
        }
    }
    else
    {
//        debug("topBarRow is null \n");
        return;
    }
    setLayout(hLayoutTopBarRow);
}
