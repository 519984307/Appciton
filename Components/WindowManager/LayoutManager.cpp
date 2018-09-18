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
#include "ScreenLayoutDefine.h"
#include "ScreenLayoutModel.h"
#include "IConfig.h"
#include "QHBoxLayout"
#include <QMap>
#include <Debug.h>
#include "IWidget.h"
#include <QList>
#include "OrderedMap.h"

typedef QList<LayoutNode> LayoutRow;

class LayoutManagerPrivate
{
public:
    LayoutManagerPrivate()
        : contentLayout(new QHBoxLayout()),
          curUserFace(UFACE_MONITOR_STANDARD)
    {
        contentLayout->setSpacing(0);
        contentLayout->setContentsMargins(0, 0, 0, 0);
    }


    /**
     * @brief createContainter create a layout container
     * @return
     */
    QWidget *createContainter()
    {
        QWidget *w = new QWidget();
        w->setAutoFillBackground(false);
        w->setAttribute(Qt::WA_NoSystemBackground);
        w->setProperty("type", "Container");
        return w;
    }

    /**
     * @brief doLayout do the layout stuff
     */
    void doLayout();

    /**
     * @brief performStandardLayout perform standard layout
     */
    void performStandardLayout();

    /**
     * @brief clearLayout clear the layout info
     * @param layout the top layout
     */
    void clearLayout(QLayout *layout);

    QVariantMap layoutMap;
    QHBoxLayout *contentLayout;
    QMap<QString, IWidget *> layoutWidgets;
    UserFaceType curUserFace;

    OrderedMap<int, LayoutRow> layoutInfos;

    QMap<QString, QString> layoutNodeMap;

private:
    LayoutManagerPrivate(const LayoutManagerPrivate &);
};

void LayoutManagerPrivate::doLayout()
{
    clearLayout(contentLayout);

    switch (curUserFace) {
    case UFACE_MONITOR_STANDARD:
        performStandardLayout();
        break;
    case UFACE_MONITOR_12LEAD:
        break;
    case UFACE_MONITOR_OXYCRG:
        break;
    case UFACE_MONITOR_TREND:
        break;
    case UFACE_MONITOR_BIGFONT:
        break;
    default:
        qdebug("Unsupport screen layout!");
        break;
    }
}

void LayoutManagerPrivate::performStandardLayout()
{
    QVBoxLayout *leftLayout = new QVBoxLayout();
    QWidget *waveContainer = createContainter();
    QWidget *leftParamContainer = createContainter();
    leftLayout->addWidget(waveContainer);
    leftLayout->addWidget(leftParamContainer);
    contentLayout->addLayout(leftLayout, 4);
    QWidget *rightParamContainer = createContainter();
    contentLayout->addWidget(rightParamContainer, 2);

    QGridLayout *waveLayout = new QGridLayout(waveContainer);
    QGridLayout *leftParamLayout = new QGridLayout(leftParamContainer);
    QGridLayout *rightParamLayout = new QGridLayout(rightParamContainer);

    OrderedMap<int, LayoutRow>::ConstIterator iter = layoutInfos.begin();
    for (; iter != layoutInfos.end(); ++iter)
    {
        LayoutRow::ConstIterator nodeIter = iter.value().constBegin();
        for (; nodeIter != iter.value().constEnd(); ++nodeIter)
        {
            int row = iter.key();
            IWidget *w = layoutWidgets.value(layoutNodeMap[nodeIter->name], NULL);
            if (!w)
            {
                continue;
            }
            if (nodeIter->pos < LAYOUT_WAVE_END_COLUMN)
            {
                if (row < LAYOUT_MAX_WAVE_ROW_NUM)
                {
                    waveLayout->addWidget(w, row, nodeIter->pos, nodeIter->span, 1);
                }
                else
                {
                    leftParamLayout->addWidget(w, row - LAYOUT_MAX_WAVE_ROW_NUM, nodeIter->pos, nodeIter->span, 1);
                }
            }
            else
            {
                rightParamLayout->addWidget(w, row, nodeIter->pos - LAYOUT_WAVE_END_COLUMN, nodeIter->span, 1);
            }
        }
    }
}

void LayoutManagerPrivate::clearLayout(QLayout *layout)
{
    if (layout == NULL)
    {
        return;
    }
    QLayoutItem *item;
    while ((item = contentLayout->takeAt(0)) != 0)
    {
        QLayout *childLayout = item->layout();
        if (childLayout)
        {
            clearLayout(childLayout);
        }

        QWidget *w = item->widget();
        if (w)
        {
            if (w->property("type").toString() == "Container")
            {
                // this is a container widget
                clearLayout(w->layout());
                delete w;
            }
            else
            {
                w->setParent(NULL);
                w->setVisible(false);
            }
        }

        delete item;
    }
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
    updateLayout();
    d_ptr->doLayout();
}

QLayout *LayoutManager::getContentLayout()
{
    return d_ptr->contentLayout;
}

void LayoutManager::addLayoutWidget(IWidget *w, LayoutNodeType nodeType)
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

    w->setVisible(false);

    if (nodeType != LAYOUT_NODE_NONE)
    {
        d_ptr->layoutNodeMap.insert(layoutNodeName(nodeType), w->name());
    }
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

void LayoutManager::setUFaceType(UserFaceType type)
{
    if (d_ptr->curUserFace == type)
    {
        return;
    }
}

void LayoutManager::updateLayout()
{
    d_ptr->layoutInfos.clear();
    QVariantList layoutRows = d_ptr->layoutMap["LayoutRow"].toList();
    if (layoutRows.isEmpty())
    {
        // might has only one element
        QVariant tmp = d_ptr->layoutMap["LayoutRow"];
        if (tmp.isValid())
        {
            layoutRows.append(tmp);
        }
    }

    QVariantList::ConstIterator iter;
    int curRow = 0;
    for (iter = layoutRows.constBegin(); iter != layoutRows.constEnd(); ++iter)
    {
        LayoutRow row;
        QVariantList nodes = iter->toMap().value("LayoutNode").toList();
        if (nodes.isEmpty())
        {
            // might has only one element
            QVariantMap nm = iter->toMap()["LayoutNode"].toMap();
            if (nm.isEmpty())
            {
                continue;
            }
            nodes.append(nm);
        }

        QVariantList::ConstIterator nodeIter;
        for (nodeIter = nodes.constBegin(); nodeIter != nodes.constEnd(); ++nodeIter)
        {
            QVariantMap nodeMap = nodeIter->toMap();
            int span = nodeMap["@span"].toInt();
            if (span == 0)
            {
                span = 1;
            }
            int pos = nodeMap["@pos"].toInt();
            bool editable = nodeMap["@editable"].toBool();
            LayoutNode node;
            node.name = nodeMap["@text"].toString();
            node.editable = editable;
            node.pos = pos;
            node.span = span;

            if (row.isEmpty())
            {
                row.append(node);
            }
            else
            {
                // the layout should be sorted by the pos
                LayoutRow::Iterator iter = row.begin();
                for (; iter != row.end(); ++iter)
                {
                    if (iter->pos > node.pos)
                    {
                        break;
                    }
                }

                row.insert(iter, node);
            }
        }

        if (!row.isEmpty())
        {
            d_ptr->layoutInfos.insert(curRow, row);
        }

        curRow++;
    }
}

LayoutManager::LayoutManager()
    :d_ptr(new LayoutManagerPrivate())
{
    // TODO: load the store uface info
}
