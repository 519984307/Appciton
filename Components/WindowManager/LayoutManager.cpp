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
          mainLayout(NULL),
          waveAreaStretch(4),
          paramAreaStretch(2)
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
     * @brief perform12LeadLayout perform 12 lead layout
     */
    void perform12LeadLayout();

    /**
     * @brief performOxyCRGLayout perform OxyCRG interface layout
     */
    void performOxyCRGLayout();

    /**
     * @brief performBigFontLayout perform Bigfont layout
     */
    void performBigFontLayout();

    /**
     * @brief performTrendLayout perform trend layout
     */
    void performTrendLayout();

    /**
     * @brief clearLayout clear the layout item and the layout container
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

    int waveAreaStretch;
    int paramAreaStretch;

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

    // clear the display wave form list and param first, the displayed waveforms and params should be updated
    // in each layout funciton
    displayWaveforms.clear();
    displayParams.clear();

    switch (curUserFace) {
    case UFACE_MONITOR_STANDARD:
        performStandardLayout();
        break;
    case UFACE_MONITOR_12LEAD:
        perform12LeadLayout();
        break;
    case UFACE_MONITOR_OXYCRG:
        performOxyCRGLayout();
        break;
    case UFACE_MONITOR_TREND:
        performTrendLayout();
        break;
    case UFACE_MONITOR_BIGFONT:
        performBigFontLayout();
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
    contentLayout->addLayout(leftLayout, waveAreaStretch);
    QWidget *rightParamContainer = createContainter();
    contentLayout->addWidget(rightParamContainer, paramAreaStretch);

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

void LayoutManagerPrivate::perform12LeadLayout()
{
    QWidget *waveContainer = createContainter();
    contentLayout->addWidget(waveContainer, waveAreaStretch);
    QWidget *rightParamContainer = createContainter();
    contentLayout->addWidget(rightParamContainer, paramAreaStretch);

    QGridLayout *waveLayout = new QGridLayout(waveContainer);
    waveLayout->setMargin(0);
    QGridLayout *rightParamLayout = new QGridLayout(rightParamContainer);
    rightParamLayout->setMargin(0);

    // add the 12 lead waveforms
    QString path("PrimaryCfg|UILayout|ContentLayout|ECG12Lead|");
    // get the display format
    int mode = DISPLAY_12LEAD_STAND;
    currentConfig.getNumValue("PrimaryCfg|ECG12L|DisplayFormat", mode);
    path += ECGSymbol::convert(static_cast<Display12LeadFormat>(mode));

    QString waveOrder;
    systemConfig.getStrValue(path, waveOrder);

    QStringList ecgWaveList = waveOrder.split(',');

    for (int i = 0;  i < ecgWaveList.count(); ++i)
    {
        int row = i / 2;
        int column = i % 2;
        IWidget *w = layoutWidgets[ecgWaveList.at(i)];
        if (w)
        {
            w->setVisible(true);
            waveLayout->addWidget(layoutWidgets[ecgWaveList.at(i)], row, column);
            displayWaveforms.append(w->name());
        }
    }

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

            // only add the param on the right in the standard layout
            if (nodeIter->pos >= LAYOUT_WAVE_END_COLUMN)
            {
                w->setVisible(true);
                rightParamLayout->addWidget(w, row, nodeIter->pos - LAYOUT_WAVE_END_COLUMN, 1, nodeIter->span);
                displayParams.append(w->name());
            }
        }
    }
}

#define MAX_WAVEWIDGET_ROW_IN_OXYCRG_LAYOUT 3       // the maximum wavewidget row can be displayed in the wave area while in the oxycrg layout
void LayoutManagerPrivate::performOxyCRGLayout()
{
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setContentsMargins(0, 0, 0, 0);
    QWidget *waveContainer = createContainter();
    IWidget *oxyCRGWidget = layoutWidgets["OxyCRGWidget"];  // get the oxycrg widget
    leftLayout->addWidget(waveContainer, 1);
    if (oxyCRGWidget)
    {
        oxyCRGWidget->setVisible(true);
        leftLayout->addWidget(oxyCRGWidget, 1);
    }
    contentLayout->addLayout(leftLayout, waveAreaStretch);
    QWidget *rightParamContainer = createContainter();
    contentLayout->addWidget(rightParamContainer, paramAreaStretch);

    QGridLayout *waveLayout = new QGridLayout(waveContainer);
    waveLayout->setMargin(0);
    QGridLayout *rightParamLayout = new QGridLayout(rightParamContainer);
    rightParamLayout->setMargin(0);

    int waveWidgetCounter = 0;
    int currentRow = -1;
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
            if (nodeIter->pos < LAYOUT_WAVE_END_COLUMN) // in the left part, contain wave or param
            {
                if (row < LAYOUT_MAX_WAVE_ROW_NUM) // wave widgets
                {
                    if (waveWidgetCounter < MAX_WAVEWIDGET_ROW_IN_OXYCRG_LAYOUT || currentRow == row)
                    {
                        w->setVisible(true);
                        waveLayout->addWidget(w, row, nodeIter->pos, 1, nodeIter->span);
                        if (qobject_cast<WaveWidget *>(w))
                        {
                            displayWaveforms.append(w->name());
                        }

                        if (currentRow != row)
                        {
                            waveWidgetCounter++;
                            currentRow = row;
                        }
                    }
                }
                else    // param widgets
                {
                    // don't add the params in the left part
                }
            }
            else  // the right part are all param
            {
                w->setVisible(true);
                rightParamLayout->addWidget(w, row, nodeIter->pos - LAYOUT_WAVE_END_COLUMN, 1, nodeIter->span);
                displayParams.append(w->name());
            }
        }
    }
}

void LayoutManagerPrivate::performBigFontLayout()
{
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setMargin(0);
    contentLayout->addLayout(gridLayout);

    QVariantMap bigFontLayout = systemConfig.getConfig("PrimaryCfg|UILayout|ContentLayout|BigFont");
    QVariantList  layoutRows = bigFontLayout["LayoutRow"].toList();
    if (layoutRows.isEmpty())
    {
        // might has only one item
        QVariant row = bigFontLayout["LayoutRow"];
        if (row.isValid())
        {
            layoutRows.append(row);
        }
    }

    QVariantList::ConstIterator rowIter;
    int row = 0;
    for (rowIter = layoutRows.constBegin(); rowIter != layoutRows.constEnd(); ++rowIter)
    {
        QVariantMap rowMap = rowIter->toMap();
        QVariantList nodes = rowMap["LayoutNode"].toList();
        if (nodes.isEmpty())
        {
            // might has only one item
            QVariant n = rowMap["LayoutNode"];
            if (n.isValid())
            {
                nodes.append(n);
            }
        }

        QVariantList::ConstIterator nodeIter;
        int column = 0;;
        for (nodeIter = nodes.constBegin(); nodeIter != nodes.constEnd(); ++nodeIter)
        {
            QVariantMap node = nodeIter->toMap();
            QString paramName = node["Param"].toMap()["@text"].toString();
            QString waveName = node["Wave"].toMap()["@text"].toString();

            QWidget *nodeContainer = createContainter();
            gridLayout->addWidget(nodeContainer, row, column);

            QVBoxLayout *vLayout = new QVBoxLayout(nodeContainer);
            vLayout->setMargin(0);

            IWidget *w = layoutWidgets.value(layoutNodeMap[paramName], NULL);
            if (w && widgetLayoutable[w->name()])
            {
                w->setVisible(true);
                vLayout->addWidget(w, 2);
                displayParams.append(w->name());
            }
            else
            {
                vLayout->addWidget(createContainter(), 2);
            }

            w = layoutWidgets.value(layoutNodeMap[waveName], NULL);
            if (w && widgetLayoutable[w->name()])
            {
                w->setVisible(true);
                vLayout->addWidget(w, 1);
                displayWaveforms.append(w->name());
            }
            else
            {
                vLayout->addWidget(createContainter(), 1);
            }
            column++;
        }

        row++;
    }
}

void LayoutManagerPrivate::performTrendLayout()
{
    // TODO
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

    emit layoutChanged();
}

QStringList LayoutManager::getDisplayedWaveforms() const
{
    return d_ptr->displayWaveforms;
}

QList<int> LayoutManager::getDisplayedWaveformIDs() const
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

QStringList LayoutManager::getDisplayedWaveformLabels() const
{
    QStringList waveLabels;
    QStringList::ConstIterator iter;
    for (iter = d_ptr->displayWaveforms.constBegin(); iter != d_ptr->displayWaveforms.constEnd(); ++iter)
    {
        WaveWidget *w = qobject_cast<WaveWidget *>(d_ptr->layoutWidgets[*iter]);
        if (w)
        {
            waveLabels.append(w->waveLabel());
        }
    }

    return waveLabels;
}

WaveWidget *LayoutManager::getDisplayedWaveWidget(WaveformID id)
{
    QStringList::ConstIterator iter;
    for (iter = d_ptr->displayWaveforms.constBegin(); iter != d_ptr->displayWaveforms.constEnd(); ++iter)
    {
        WaveWidget *w = qobject_cast<WaveWidget *>(d_ptr->layoutWidgets[*iter]);
        if (w && w->getID() == id)
        {
            return w;
        }
    }

    return NULL;
}

bool LayoutManager::isLastWaveWidget(const WaveWidget *w) const
{
    if (d_ptr->displayWaveforms.isEmpty() || !w)
    {
        return false;
    }

    QString name = d_ptr->displayWaveforms.last();
    WaveWidget *lastW = qobject_cast<WaveWidget *>(d_ptr->layoutWidgets[name]);
    if (lastW == w)
    {
        return true;
    }

    return false;
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

QRect LayoutManager::getMenuArea() const
{
    QRect r = d_ptr->contentView->geometry();
    QRect gr(d_ptr->contentView->mapToGlobal(r.topLeft()),
             d_ptr->contentView->mapToGlobal(r.bottomRight()));

    gr.setWidth(gr.width() * d_ptr->waveAreaStretch / (d_ptr->waveAreaStretch + d_ptr->paramAreaStretch));
    return gr;
}

LayoutManager::LayoutManager()
    :d_ptr(new LayoutManagerPrivate())
{
    d_ptr->layoutMap = systemConfig.getConfig("PrimaryCfg|UILayout|ContentLayout|Normal");

    addLayoutWidget(d_ptr->contentView);
}