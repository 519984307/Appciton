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
#include "ECGSymbol.h"
#include "WaveWidget.h"

typedef QList<LayoutNode> LayoutRow;

class LayoutManagerPrivate
{
public:
    LayoutManagerPrivate()
        : contentView(new IWidget("ContentView")),
          contentLayout(new QHBoxLayout(contentView)),
          curUserFace(UFACE_MONITOR_STANDARD),
          mainLayout(NULL)
    {
        contentLayout->setSpacing(0);
        contentLayout->setContentsMargins(0, 0, 0, 0);
        this->contentView->setFocusPolicy(Qt::NoFocus);
        this->contentView->setAttribute(Qt::WA_NoSystemBackground);
        this->contentView->setContentsMargins(0, 0, 0, 0);
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
     * @brief parseMargin parse the margin from a string, support string format as follow:
     *        "1": set all the margin to 1
     *        "1,2,3,4": left:1, top: 2, right: 3, bottom: 4
     * @param marginStr
     * @return
     */
    QMargins parseMargin(const QString& marginStr);

    /**
     * @brief doParseMainLayout parse the main layout
     * @note the support attribute as follow:
     * "direction": the box layout direction, the a layout item has this attribute, it's means it's a box layout
     * "margin": the layout's content margin
     * "height": the widget's fix height, if the item is a layout, we will create a container widget to hold the layout and set fix height
     * "width: the widget's fix widget, if the item is a layout, we will create a container widget to hold the layout and set fix height
     * "stretch" the stretch value
     */
    void doParseMainLayout(const QVariantMap &map, QBoxLayout *parentLayout);

    /**
     * @brief doContentLayout do the layout stuff
     */
    void doContentLayout();

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
    IWidget *contentView;
    QHBoxLayout *contentLayout;
    QMap<QString, IWidget *> layoutWidgets;
    UserFaceType curUserFace;

    OrderedMap<int, LayoutRow> layoutInfos;

    QMap<QString, QString> layoutNodeMap;

    QBoxLayout *mainLayout;

    QStringList displayWaveforms;   /* current visiable wave widget's name list */
    QStringList displayParams;      /* current visiable param widget's name list */

    QMap<QString, bool> widgetLayoutable;   /* record whether the widget is layoutable */

private:
    LayoutManagerPrivate(const LayoutManagerPrivate &);
};

QMargins LayoutManagerPrivate::parseMargin(const QString &marginStr)
{
    if (marginStr.isEmpty())
    {
        return QMargins();
    }

    bool ok;
    // parse the margin string
    if (marginStr.contains(','))
    {
        QStringList list = marginStr.split(',');
        if (list.count() == 4)
        {
            int okCount = 0;
            int left = list[0].toInt(&ok);
            okCount += ok;
            int top = list[1].toInt(&ok);
            okCount += ok;
            int right = list[2].toInt(&ok);
            okCount += ok;
            int bottom = list[3].toInt(&ok);
            okCount += ok;
            if (okCount == 4)
            {
                return QMargins(left, top, right, bottom);
            }
        }
    }
    else
    {
        int m = marginStr.toInt(&ok);
        if (ok)
        {
            return QMargins(m, m, m, m);
        }
    }

    qDebug() << Q_FUNC_INFO << "Invalid margin string " << marginStr;
    return QMargins();
}

void LayoutManagerPrivate::doParseMainLayout(const QVariantMap &map, QBoxLayout *parentLayout)
{
    QVariantList layoutItemList = map["LayoutItem"].toList();
    if (layoutItemList.isEmpty())
    {
        // might has only item
        QVariant item = map["LayoutItem"];
        if (item.isValid())
        {
            layoutItemList.append(item);
        }
    }
    QVariantList::ConstIterator iter = layoutItemList.constBegin();

    for (; iter != layoutItemList.constEnd(); ++iter)
    {
        QVariantMap itemMap = iter->toMap();
        if (itemMap.isEmpty())
        {
            continue;
        }

        QString direction = itemMap["@direction"].toString();
        int height = itemMap["@height"].toInt();
        int width = itemMap["@width"].toInt();
        int stretch = itemMap["@stretch"].toInt();

        if (!direction.isEmpty())
        {
            // need to create a layout
            QBoxLayout *subLayout;
            if (direction == "horizontal")
            {
                subLayout = new QHBoxLayout();
            }
            else
            {
                subLayout = new QVBoxLayout();
            }

            QWidget *w = createContainter();
            QString bgColor = itemMap["@bgcolor"].toString();
            if (!bgColor.isEmpty())
            {
                w->setAutoFillBackground(true);
                w->setAttribute(Qt::WA_NoSystemBackground, false);
                QPalette pal = w->palette();
                pal.setBrush(QPalette::Window, QColor(bgColor));
                w->setPalette(pal);
            }
            w->setLayout(subLayout);
            subLayout->setContentsMargins(parseMargin(itemMap["@margin"].toString()));
            subLayout->setSpacing(itemMap["@spacing"].toInt());

            if (height)
            {
                w->setFixedHeight(height);
            }

            if (width)
            {
                w->setFixedWidth(width);
            }

            parentLayout->addWidget(w, stretch);

            doParseMainLayout(itemMap, subLayout);
        }
        else
        {
            // just add widget
            IWidget *w = layoutWidgets.value(itemMap["@text"].toString(), NULL);
            if (w != NULL)
            {
                if (height)
                {
                    w->setFixedHeight(height);
                }

                if (width)
                {
                    w->setFixedWidth(width);
                }

                parentLayout->addWidget(w, stretch);
                w->setVisible(true);
            }
        }
    }
}

void LayoutManagerPrivate::doContentLayout()
{
    // clear the exist layout
    clearLayout(contentLayout);

    // clear the display wave form list first, the displayed waveforms should be updated
    // in each layout funciton
    displayWaveforms.clear();
    displayParams.clear();

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

    contentLayout->activate();
}

void LayoutManagerPrivate::performStandardLayout()
{
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setContentsMargins(0, 0, 0, 0);
    QWidget *waveContainer = createContainter();
    QWidget *leftParamContainer = createContainter();
    leftLayout->addWidget(waveContainer);
    leftLayout->addWidget(leftParamContainer);
    contentLayout->addLayout(leftLayout, 4);
    QWidget *rightParamContainer = createContainter();
    contentLayout->addWidget(rightParamContainer, 2);

    QGridLayout *waveLayout = new QGridLayout(waveContainer);
    waveLayout->setMargin(0);
    QGridLayout *leftParamLayout = new QGridLayout(leftParamContainer);
    leftParamLayout->setMargin(0);
    QGridLayout *rightParamLayout = new QGridLayout(rightParamContainer);
    rightParamLayout->setMargin(0);

    OrderedMap<int, LayoutRow>::ConstIterator iter = layoutInfos.begin();
    for (; iter != layoutInfos.end(); ++iter)
    {
        LayoutRow::ConstIterator nodeIter = iter.value().constBegin();
        for (; nodeIter != iter.value().constEnd(); ++nodeIter)
        {
            int row = iter.key();
            IWidget *w = layoutWidgets.value(layoutNodeMap[nodeIter->name], NULL);
            if (!w || !widgetLayoutable[w->name()])
            {
                continue;
            }
            w->setVisible(true);
            if (nodeIter->pos < LAYOUT_WAVE_END_COLUMN)
            {
                if (row < LAYOUT_MAX_WAVE_ROW_NUM)
                {
                    waveLayout->addWidget(w, row, nodeIter->pos, 1, nodeIter->span);
                    if (qobject_cast<WaveWidget *>(w))
                    {
                        displayWaveforms.append(w->name());
                    }
                }
                else
                {
                    leftParamLayout->addWidget(w, row - LAYOUT_MAX_WAVE_ROW_NUM, nodeIter->pos, 1, nodeIter->span);
                    displayParams.append(w->name());
                }
            }
            else
            {
                rightParamLayout->addWidget(w, row, nodeIter->pos - LAYOUT_WAVE_END_COLUMN, 1, nodeIter->span);
                displayParams.append(w->name());
            }
        }
    }

    // the wave container stretch
    leftLayout->setStretch(0, waveLayout->rowCount());
    // the let param container stretch
    leftLayout->setStretch(1, leftParamLayout->rowCount());
}

void LayoutManagerPrivate::clearLayout(QLayout *layout)
{
    if (layout == NULL)
    {
        return;
    }
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != 0)
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
    QVariantMap layoutMap = systemConfig.getConfig("PrimaryCfg|UILayout|ContentLayout|Normal");
    if (layoutMap == d_ptr->layoutMap)
    {
        return;
    }

    d_ptr->layoutMap = layoutMap;

    // reparse the layout info and perform layout
    updateLayout();
}

QLayout *LayoutManager::mainLayout()
{
    if (!d_ptr->mainLayout)
    {
        // start parse the main layout
        QVariantMap mainLayout = systemConfig.getConfig("PrimaryCfg|UILayout|MainLayout");
        if (mainLayout["@direction"] == "horizontal")
        {
            d_ptr->mainLayout = new QHBoxLayout();
        }
        else
        {
            d_ptr->mainLayout = new QVBoxLayout();
        }

        QString marginStr = mainLayout["@margin"].toString();
        d_ptr->mainLayout->setContentsMargins(d_ptr->parseMargin(marginStr));
        d_ptr->mainLayout->setSpacing(mainLayout["@spacing"].toInt());

        d_ptr->doParseMainLayout(mainLayout, d_ptr->mainLayout);
    }

    return d_ptr->mainLayout;
}

IWidget *LayoutManager::getContentView() const
{
    return d_ptr->contentView;
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
    d_ptr->widgetLayoutable.insert(w->name(), true);

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
    if (!d_ptr->contentLayout->isEmpty() && d_ptr->curUserFace == type)
    {
        return;
    }

    d_ptr->curUserFace = type;

    updateLayout();
}

UserFaceType LayoutManager::getUFaceType() const
{
    return d_ptr->curUserFace;
}

void LayoutManager::updateLayout()
{
    // TODO: 1. check co2 is connect or not to decide whether show co2 wave or trend

    // find the ECG correspond wave
    int leadMode = ECG_LEAD_MODE_3;
    systemConfig.getNumValue("PrimaryCfg|ECG|LeadMode", leadMode);
    QString ecg1Wave;
    QString ecg2Wave;
    systemConfig.getStrValue("PrimaryCfg|ECG|Ecg1WaveWidget", ecg1Wave);
    systemConfig.getStrValue("PrimaryCfg|ECG|Ecg2WaveWidget", ecg2Wave);
    if (leadMode == ECG_LEAD_MODE_3)
    {
        d_ptr->layoutNodeMap[layoutNodeName(LAYOUT_NODE_WAVE_ECG1)] = ecg1Wave;
        d_ptr->layoutNodeMap[layoutNodeName(LAYOUT_NODE_WAVE_ECG2)] = QString();
    }
    else
    {
        d_ptr->layoutNodeMap[layoutNodeName(LAYOUT_NODE_WAVE_ECG1)] = ecg1Wave;
        d_ptr->layoutNodeMap[layoutNodeName(LAYOUT_NODE_WAVE_ECG2)] = ecg2Wave;
    }

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

    // perform layout
    d_ptr->doContentLayout();
}

QStringList LayoutManager::getDisplayedWaveforms()
{
    return d_ptr->displayWaveforms;
}

QList<int> LayoutManager::getDisplayedWaveformIDs()
{
    QList<int> waveIDs;
    QStringList::ConstIterator iter;
    for (iter = d_ptr->displayWaveforms.constBegin(); iter != d_ptr->displayWaveforms.constEnd(); ++iter)
    {
        WaveWidget *w = qobject_cast<WaveWidget *>(d_ptr->layoutWidgets[*iter]);
        if (w)
        {
            waveIDs.append(w->getID());
        }
    }

    return waveIDs;
}

void LayoutManager::resetWave()
{
    QStringList::ConstIterator iter;
    for (iter = d_ptr->displayWaveforms.constBegin(); iter != d_ptr->displayWaveforms.constEnd(); ++iter)
    {
        WaveWidget *w = qobject_cast<WaveWidget *>(d_ptr->layoutWidgets[*iter]);
        if (w)
        {
            w->resetWave();
        }
    }
}

bool LayoutManager::setWidgetLayoutable(const QString &name, bool enable)
{
    d_ptr->widgetLayoutable[name] = enable;

    if (enable)
    {
        return !d_ptr->displayWaveforms.contains(name) && !d_ptr->displayParams.contains(name);
    }
    else
    {
        return d_ptr->displayWaveforms.contains(name) || d_ptr->displayParams.contains(name);
    }
}

LayoutManager::LayoutManager()
    :d_ptr(new LayoutManagerPrivate())
{
    d_ptr->layoutMap = systemConfig.getConfig("PrimaryCfg|UILayout|ContentLayout|Normal");

    addLayoutWidget(d_ptr->contentView);
}
