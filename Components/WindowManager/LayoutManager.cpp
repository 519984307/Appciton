/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/17
 **/

#include "LayoutManager.h"
#include "ScreenLayoutModel.h"
#include "IConfig.h"
#include <QGridLayout>
#include <QMap>
#include <Debug.h>
#include "IWidget.h"

class LayoutManagerPrivate
{
public:
    LayoutManagerPrivate()
        : gridLayout(new QGridLayout())
    {}

    void doLayout();

    QVariantMap layoutMap;
    QGridLayout *gridLayout;
    QMap<QString, IWidget *> layoutWidgets;

private:
    LayoutManagerPrivate(const LayoutManagerPrivate &);
};

void LayoutManagerPrivate::doLayout()
{
}

LayoutManager &LayoutManager::LayoutManager::getInstance()
{
    static LayoutManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new LayoutManager();
    }
    return *instance;
}

LayoutManager::~LayoutManager()
{
    delete d_ptr;
}

void LayoutManager::reloadLayoutConfig()
{
    d_ptr->layoutMap = systemConfig.getConfig("PrimaryCfg|UILayout|ScreenLayout|Normal");
    d_ptr->doLayout();
}

QLayout *LayoutManager::getContentLayout()
{
    return d_ptr->gridLayout;
}

void LayoutManager::addLayoutWidget(IWidget *w)
{
    if (!w)
    {
        return;
    }

    if (d_ptr->layoutWidgets.contains(w->name()))
    {
        qdebug("Layout widget is already added.");
    }

    d_ptr->layoutWidgets.insert(w->name(), w);
}

IWidget *LayoutManager::getLayoutWidget(const QString &name)
{
    QMap<QString, IWidget *>::Iterator iter = d_ptr->layoutWidgets.find(name);
    if (iter != d_ptr->layoutWidgets.end())
    {
        return iter.value();
    }
    return NULL;
}

LayoutManager::LayoutManager()
    :d_ptr(new LayoutManagerPrivate())
{
}
