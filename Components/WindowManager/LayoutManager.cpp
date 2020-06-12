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
#include "ConfigManager.h"
#include "QHBoxLayout"
#include <QMap>
#include <Debug.h>
#include "IWidget.h"
#include <QList>
#include "Framework/Utility/OrderedMap.h"
#include "WaveWidget.h"
#include "ShortTrendContainer.h"
#include "TrendWidget.h"
#include "ECGParam.h"
#include "CO2Param.h"
#include "ECGDupParam.h"
#include "SPO2Param.h"
#include "IBPParam.h"
#include "PatientManager.h"

// 第一道波形高度不低于30mm，设置31来计算避免误差造成高度不够
#define FIRST_ECG_WAVE_HEIGHT qRound(31 / systemManager.getScreenPixelHPitch())

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
          paramAreaStretch(2),
          waveRowCount(0),
          leftParamRowCount(0)
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
    QMargins parseMargin(const QString &marginStr);

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
     * @brief perform7LeadLayout perform 7 lead layout
     */
    void perform7LeadLayout();

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
     * @brief performSPO2Layout perform spo2 layout
     */
    void performSPO2Layout();

    /**
     * @brief clearLayout clear the layout item and the layout container
     * @param layout the top layout
     */
    void clearLayout(QLayout *layout);

    /**
     * @brief updateTabOrder update the tab focus order
     */
    void updateTabOrder();

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
    QList<IWidget *> contentWidgets;        /* record the widgets in the content view */
    QList<IWidget *> mainLayoutWidgets;     /* record the widgets in the main layout */

    int waveAreaStretch;
    int paramAreaStretch;

    int waveRowCount;
    int leftParamRowCount;

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
                mainLayoutWidgets.append(w);
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

    // the content widget should also be update in every layout
    contentWidgets.clear();

    switch (curUserFace)
    {
    case UFACE_MONITOR_STANDARD:
        performStandardLayout();
        break;
    case UFACE_MONITOR_ECG_FULLSCREEN:
        if (ecgParam.getLeadMode() == ECG_LEAD_MODE_12)
        {
            perform12LeadLayout();
        }
        else if (ecgParam.getLeadMode() == ECG_LEAD_MODE_5)
        {
            perform7LeadLayout();
        }
        else
        {
            qWarning() << "No ECG Fullscreen Layout for 3 Lead ECG";
            performStandardLayout();
        }
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
    case UFACE_MONITOR_SPO2:
        performSPO2Layout();
        break;
    default:
        qdebug("Unsupport screen layout!");
        break;
    }

    contentLayout->activate();

    // update the focus order
    updateTabOrder();
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
    int curWaveRow = 0;
    int curRightParamRow = 0;
    int curLeftParamRow = 0;
    int rightLayoutColumn = 0;
    bool hasVisableLeftParamWidget = false;
    QVector<QVector<IWidget *> > rightParamsWidget;
    QVector<QVector<int> > rightParamPos;
    for (; iter != layoutInfos.end(); ++iter)
    {
        LayoutRow::ConstIterator nodeIter = iter.value().constBegin();
        int row = iter.key();
        bool hasWave = false;
        bool hasRightParam = false;
        bool hasLeftParam = false;
        for (; nodeIter != iter.value().constEnd(); ++nodeIter)
        {
            if (nodeIter->pos < LAYOUT_WAVE_END_COLUMN)
            {
                if (row < LAYOUT_MAX_WAVE_ROW_NUM)
                {
                    IWidget *w = layoutWidgets.value(layoutNodeMap[nodeIter->name], NULL);
                    if (!w)
                    {
                       continue;
                    }
                    w->setVisible(true);
                    w->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                    waveLayout->addWidget(w, curWaveRow, nodeIter->pos, 1, nodeIter->span);
                    displayWaveforms.append(w->name());
                    contentWidgets.append(w);
                    hasWave = true;
                }
                else
                {
                    IWidget *w = layoutWidgets.value(layoutNodeMap[nodeIter->name], NULL);
                    if (w)
                    {
                        w->setVisible(true);
                        w->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                        leftParamLayout->addWidget(w, curLeftParamRow, nodeIter->pos, 1, nodeIter->span);
                        displayParams.append(w->name());
                        contentWidgets.append(w);
                        hasVisableLeftParamWidget = true;
                    }
                    else
                    {
                        /* add container if the widget isn't exist or empty */
                        QWidget *container = createContainter();
                        container->setVisible(true);
                        container->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                        leftParamLayout->addWidget(container, curLeftParamRow, nodeIter->pos, 1, nodeIter->span);
                    }
                    hasLeftParam = true;
                }
            }
            else
            {
                IWidget *w = layoutWidgets.value(layoutNodeMap[nodeIter->name], NULL);
                if (w)
                {
                    w->setVisible(true);
                    w->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                    contentWidgets.append(w);

                    /* store first, layout later */
                    if (rightParamsWidget.count() < curRightParamRow + 1)
                    {
                        rightParamsWidget.append(QVector<IWidget *>());
                        rightParamPos.append(QVector<int>());
                        rightParamPos.append(QVector<int>());
                    }
                    rightParamsWidget[curRightParamRow].append(w);
                    rightParamPos[curRightParamRow].append(nodeIter->pos - LAYOUT_WAVE_END_COLUMN);
                    displayParams.append(w->name());
                    hasRightParam = true;

                    if (rightLayoutColumn < rightParamsWidget[curRightParamRow].count())
                    {
                        rightLayoutColumn = rightParamsWidget[curRightParamRow].count();
                    }
                }
            }
        }

        if (hasWave)
        {
            curWaveRow++;
        }

        if (hasLeftParam)
        {
            curLeftParamRow++;
        }

        if (hasRightParam)
        {
            curRightParamRow++;
        }
    }

    /* layout the right param */
    for (int i = 0; i < curRightParamRow; i++)
    {
        /* if we has multi widgets at the same row, each widget should has equal space */
        for (int j = 0; j < rightParamsWidget[i].count(); j++)
        {
            rightParamLayout->addWidget(rightParamsWidget[i].at(j), i, rightParamPos[i].at(j),
                                        1, rightLayoutColumn / rightParamsWidget[i].count());
        }
    }

    // 设置波形和右参数区第一项的最小高度。
    waveLayout->setRowMinimumHeight(0, FIRST_ECG_WAVE_HEIGHT);
    rightParamLayout->setRowMinimumHeight(0, FIRST_ECG_WAVE_HEIGHT);

    // 计算波形行数和左参数区行数
    waveRowCount = waveLayout->rowCount();
    leftParamRowCount = hasVisableLeftParamWidget ? leftParamLayout->rowCount() : 0;
    int rowHeightLeft;
    int rowHeightRight;
    int leftRowNum = waveRowCount + leftParamRowCount;
    if (displayWaveforms.count() >= 2 && displayWaveforms.at(1).contains("ECG")
            && (leftRowNum > curRightParamRow && leftRowNum < 7))
    {
        /*
         * The first two wave is ecg and we have the more row at the left region than the right region,
         * set the ECG trend with the height of the first two ecg wave, the left row numbe should be lest
         * than 7 to avoid the height is two small for trend widget
         */
        rowHeightLeft = qRound((contentView->height() - FIRST_ECG_WAVE_HEIGHT) * 1.0 / (leftRowNum - 1));

        waveLayout->setRowStretch(0, FIRST_ECG_WAVE_HEIGHT);
        rightParamLayout->setRowStretch(0, FIRST_ECG_WAVE_HEIGHT + rowHeightLeft);
        if (curRightParamRow > 1)
        {
            rowHeightRight = (contentView->height() - (FIRST_ECG_WAVE_HEIGHT + rowHeightLeft)) / (curRightParamRow - 1);
        }
        else
        {
            rowHeightRight = rowHeightLeft;
        }
    }
    else
    {
        rowHeightLeft = qRound((contentView->height() - FIRST_ECG_WAVE_HEIGHT) * 1.0 / (leftRowNum - 1));
        rowHeightRight = qRound((contentView->height() - FIRST_ECG_WAVE_HEIGHT) * 1.0
                                / (rightParamLayout->rowCount() - 1));

        waveLayout->setRowStretch(0, FIRST_ECG_WAVE_HEIGHT);
        rightParamLayout->setRowStretch(0, FIRST_ECG_WAVE_HEIGHT);
    }

    leftLayout->setStretch(0, (waveRowCount - 1) * rowHeightLeft + FIRST_ECG_WAVE_HEIGHT);
    leftLayout->setStretch(1, leftParamRowCount * rowHeightLeft);
    for (int i = 1; i < waveLayout->rowCount(); i++)
    {
        waveLayout->setRowStretch(i, rowHeightLeft);
    }

    if (hasVisableLeftParamWidget)
    {
        for (int i = 0; i < leftParamLayout->rowCount(); i++)
        {
            leftParamLayout->setRowStretch(i, rowHeightLeft);
        }
    }
    else
    {
        leftParamContainer->setVisible(false);
    }

    for (int i = 1; i < rightParamLayout->rowCount(); i++)
    {
        rightParamLayout->setRowStretch(i, rowHeightRight);
    }
}

void LayoutManagerPrivate::perform12LeadLayout()
{
    QVBoxLayout *vLayout = new QVBoxLayout();
    QWidget *waveContainer = createContainter();
    contentLayout->addLayout(vLayout, waveAreaStretch);
    QWidget *rightParamContainer = createContainter();
    contentLayout->addWidget(rightParamContainer, paramAreaStretch);
    QWidget *leftParamContainer = createContainter();

    QGridLayout *waveLayout = new QGridLayout(waveContainer);
    waveLayout->setMargin(0);
    vLayout->addWidget(waveContainer);
    QGridLayout *leftParamLayout = new QGridLayout(leftParamContainer);
    leftParamLayout->setMargin(0);
    vLayout->addWidget(leftParamContainer);
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
    int calLead = ecgParam.getCalcLead();
    ecgWaveList.move(calLead, 0);

    for (int i = 0;  i < ecgWaveList.count(); ++i)
    {
        int row = i;
        int column = 0;
        if (i > 5)
        {
            row = i - 6;
            column = 1;
        }
        IWidget *w = layoutWidgets[ecgWaveList.at(i)];
        if (w)
        {
            w->setVisible(true);
            w->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            waveLayout->addWidget(layoutWidgets[ecgWaveList.at(i)], row, column);
            displayWaveforms.append(w->name());
            contentWidgets.append(w);
        }
    }

    QList<LayoutNode> leftParamNode;
    OrderedMap<int, LayoutRow>::ConstIterator iter = layoutInfos.begin();
    for (; iter != layoutInfos.end(); ++iter)
    {
        LayoutRow::ConstIterator nodeIter = iter.value().constBegin();
        for (; nodeIter != iter.value().constEnd(); ++nodeIter)
        {
            int row = iter.key();
            if (nodeIter->pos < LAYOUT_WAVE_END_COLUMN)
            {
                // add the param on the left in the standard layout
                if (row < LAYOUT_MAX_WAVE_ROW_NUM)
                {
                    // standard layout wave region
                }
                else
                {
                    // standard layout left pararm region
                    leftParamNode.append((*nodeIter));
                }
            }
            else
            {
                // add the param on the right in the standard layout
                IWidget *w = layoutWidgets.value(layoutNodeMap[nodeIter->name], NULL);
                QWidget *qw = w;
                if (!qw)
                {
                    qw = createContainter();
                }
                if (qw)
                {
                    qw->setVisible(true);
                    qw->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                    rightParamLayout->addWidget(qw, row, nodeIter->pos - LAYOUT_WAVE_END_COLUMN, 1, nodeIter->span);
                    rightParamLayout->setRowStretch(row, 1);
                }
                if (w)
                {
                    contentWidgets.append(w);
                    displayParams.append(w->name());
                }
            }
        }
    }

    // 剩下的左参数区的控件依次插入到右参数区下方
    QList<LayoutNode>::iterator nodeIter = leftParamNode.begin();
    for (; nodeIter != leftParamNode.end(); ++nodeIter)
    {
        IWidget *w = layoutWidgets.value(layoutNodeMap[nodeIter->name], NULL);
        QWidget *qw = w;
        if (!qw)
        {
            qw = createContainter();
        }
        if (qw)
        {
            qw->setVisible(true);
            qw->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            rightParamLayout->addWidget(qw, rightParamLayout->rowCount()
                                        , 0, 1, nodeIter->span);
            rightParamLayout->setRowStretch(rightParamLayout->rowCount() - 1, 1);
        }
        if (w)
        {
            contentWidgets.append(w);
            displayParams.append(w->name());
        }
    }

    waveRowCount = waveLayout->rowCount();
    vLayout->setStretch(0, waveRowCount);
    if (leftParamLayout->count() != 0)
    {
        vLayout->setStretch(1, leftParamLayout->rowCount());
        leftParamRowCount = leftParamLayout->rowCount();
    }
    else
    {
        leftParamRowCount = 0;
    }
}

void LayoutManagerPrivate::perform7LeadLayout()
{
    QVBoxLayout *vLayout = new QVBoxLayout();
    QWidget *waveContainer = createContainter();
    contentLayout->addLayout(vLayout, waveAreaStretch);
    QWidget *rightParamContainer = createContainter();
    contentLayout->addWidget(rightParamContainer, paramAreaStretch);
    QWidget *leftParamContainer = createContainter();

    QGridLayout *waveLayout = new QGridLayout(waveContainer);
    waveLayout->setMargin(0);
    vLayout->addWidget(waveContainer);
    QGridLayout *leftParamLayout = new QGridLayout(leftParamContainer);
    leftParamLayout->setMargin(0);
    vLayout->addWidget(leftParamContainer);
    QGridLayout *rightParamLayout = new QGridLayout(rightParamContainer);
    rightParamLayout->setMargin(0);

    // add the 7 lead waveforms
    QString path("PrimaryCfg|UILayout|ContentLayout|ECG7Lead");
    QString waveOrder;
    systemConfig.getStrValue(path, waveOrder);

    // 获取当前计算导联
    int calLead = ecgParam.getCalcLead();

    QStringList ecgWaveList = waveOrder.split(',');
    ecgWaveList.move(calLead, 0);

    for (int i = 0;  i < ecgWaveList.count(); ++i)
    {
        IWidget *w = layoutWidgets[ecgWaveList.at(i)];
        if (w)
        {
            w->setVisible(true);
            w->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            waveLayout->addWidget(layoutWidgets[ecgWaveList.at(i)], i, 0);
            displayWaveforms.append(w->name());
            contentWidgets.append(w);
        }
    }

    QList<LayoutNode> leftParamNode;
    OrderedMap<int, LayoutRow>::ConstIterator iter = layoutInfos.begin();
    for (; iter != layoutInfos.end(); ++iter)
    {
        LayoutRow::ConstIterator nodeIter = iter.value().constBegin();
        for (; nodeIter != iter.value().constEnd(); ++nodeIter)
        {
            int row = iter.key();
            if (nodeIter->pos < LAYOUT_WAVE_END_COLUMN)
            {
                // add the param on the left in the standard layout
                if (row < LAYOUT_MAX_WAVE_ROW_NUM)
                {
                    // standard layout wave region
                }
                else
                {
                    // standard layout left param region
                    leftParamNode.append((*nodeIter));
                }
            }
            else
            {
                // add the param on the right in the standard layout
                IWidget *w = layoutWidgets.value(layoutNodeMap[nodeIter->name], NULL);
                QWidget *qw = w;
                if (!qw)
                {
                    qw = createContainter();
                }
                if (qw)
                {
                    qw->setVisible(true);
                    qw->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                    rightParamLayout->addWidget(qw, row, nodeIter->pos - LAYOUT_WAVE_END_COLUMN, 1, nodeIter->span);
                }
                if (w)
                {
                    rightParamLayout->setRowStretch(row, 1);
                    contentWidgets.append(w);
                    displayParams.append(w->name());
                }
            }
        }
    }

    // 剩下的左参数区的控件依次插入到右参数区下方
    QList<LayoutNode>::iterator nodeIter = leftParamNode.begin();
    for (; nodeIter != leftParamNode.end(); ++nodeIter)
    {
        IWidget *w = layoutWidgets.value(layoutNodeMap[nodeIter->name], NULL);
        QWidget *qw = w;
        if (!qw)
        {
            qw = createContainter();
        }
        if (qw)
        {
            qw->setVisible(true);
            qw->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            rightParamLayout->addWidget(qw, rightParamLayout->rowCount(), 0, 1, nodeIter->span);
        }
        if (w)
        {
            rightParamLayout->setRowStretch(rightParamLayout->rowCount() - 1, 1);
            contentWidgets.append(w);
            displayParams.append(w->name());
        }
    }

    // 设置第一道ECG波形的高度
    waveLayout->setRowMinimumHeight(0, FIRST_ECG_WAVE_HEIGHT);

    // 设置左参数区和波形区的竖向比例
    vLayout->setStretch(0, waveLayout->rowCount());
    waveRowCount = waveLayout->rowCount();
    if (leftParamLayout->count() != 0)
    {
        vLayout->setStretch(1, leftParamLayout->rowCount());
        leftParamRowCount = leftParamLayout->rowCount();
    }
    else
    {
        leftParamRowCount = 0;
    }
}

// the maximum widget row can be displayed in the wave area while in the oxycrg layout
#define MAX_WIDGET_ROW_IN_OXYCRG_LAYOUT 3
void LayoutManagerPrivate::performOxyCRGLayout()
{
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setContentsMargins(0, 0, 0, 0);
    QWidget *waveContainer = createContainter();
    QWidget *leftParamContainer = createContainter();
    IWidget *oxyCRGWidget = layoutWidgets["OxyCRGWidget"];  // get the oxycrg widget
    leftLayout->addWidget(waveContainer);
    if (oxyCRGWidget)
    {
        oxyCRGWidget->setVisible(true);
        oxyCRGWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        leftLayout->addWidget(oxyCRGWidget);
    }
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

    int insertRow = 0;
    int lastWaveRow = 0;
    int waveRemainRow = MAX_WIDGET_ROW_IN_OXYCRG_LAYOUT;    // 波形剩下可插入的行数
    OrderedMap<int, LayoutRow>::ConstIterator iter = layoutInfos.end() - 1;
    for (; iter != layoutInfos.begin(); --iter)
    {
        int row = iter.key();
        if (row >= LAYOUT_MAX_WAVE_ROW_NUM && !(*iter).isEmpty())
        {
            waveRemainRow--;
        }
        else
        {
            break;
        }
    }

    for (iter = layoutInfos.begin(); iter != layoutInfos.end(); ++iter)
    {
        int row = iter.key();
        if (row >= LAYOUT_MAX_WAVE_ROW_NUM && (contentWidgets.indexOf(oxyCRGWidget) == -1))
        {
            // find a suitable row to insert oxyCRGWidget
            contentWidgets.append(oxyCRGWidget);
        }
        LayoutRow::ConstIterator nodeIter = iter.value().constBegin();
        for (; nodeIter != iter.value().constEnd(); ++nodeIter)
        {
            IWidget *w = layoutWidgets.value(layoutNodeMap[nodeIter->name], NULL);
            QWidget *qw = w;
            if (nodeIter->pos < LAYOUT_WAVE_END_COLUMN)  // in the left part, contain wave or param
            {
                if (row < LAYOUT_MAX_WAVE_ROW_NUM)  // wave widgets
                {
                    if (waveRemainRow <= MAX_WIDGET_ROW_IN_OXYCRG_LAYOUT && waveRemainRow > 0)
                    {
                        if (!qw)
                        {
                            qw = createContainter();
                        }
                        qw->setVisible(true);
                        qw->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                        waveLayout->addWidget(qw, insertRow, nodeIter->pos, 1, nodeIter->span);
                        waveLayout->setRowStretch(insertRow, 1);
                        if (w)
                        {
                            displayWaveforms.append(w->name());
                        }
                        waveRemainRow--;
                    }
                    lastWaveRow = insertRow;
                }
                else    // param widgets
                {
                    // add the params in the left part
                    // standard layout left pararm region
                    if (!qw)
                    {
                        qw = createContainter();
                    }
                    qw->setVisible(true);
                    qw->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                    leftParamLayout->addWidget(qw, insertRow - lastWaveRow - 1, nodeIter->pos, 1, nodeIter->span);
                    leftParamLayout->setRowStretch(insertRow - lastWaveRow - 1, 1);
                    if (w)
                    {
                        displayParams.append(w->name());
                    }
                }
            }
            else  // the right part are all param
            {
                if (!qw)
                {
                    qw = createContainter();
                }
                qw->setVisible(true);
                qw->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                rightParamLayout->addWidget(qw, row, nodeIter->pos - LAYOUT_WAVE_END_COLUMN, 1, nodeIter->span);
                rightParamLayout->setRowStretch(row, 1);
                if (w)
                {
                    displayParams.append(w->name());
                }
            }

            if (w)
            {
                contentWidgets.append(w);
            }
        }
        if (!(*iter).isEmpty())
        {
            insertRow++;
        }
    }
    leftLayout->setStretch(0, waveLayout->rowCount());
    int rowCount = 0;
    if (leftParamLayout->count() != 0)
    {
        rowCount = leftParamLayout->rowCount();
    }
    leftLayout->setStretch(1, insertRow - rowCount - waveLayout->rowCount());
    waveRowCount = insertRow - rowCount;
    leftLayout->setStretch(2, rowCount);
    leftParamRowCount = rowCount;
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
            gridLayout->setColumnStretch(column, 1);

            QVBoxLayout *vLayout = new QVBoxLayout(nodeContainer);
            vLayout->setMargin(0);

            IWidget *w = layoutWidgets.value(layoutNodeMap[paramName], NULL);
            if (w && widgetLayoutable[w->name()])
            {
                w->setVisible(true);
                w->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                vLayout->addWidget(w, 2);
                displayParams.append(w->name());
                contentWidgets.append(w);
            }
            else
            {
                vLayout->addWidget(createContainter(), 2);
            }

            w = layoutWidgets.value(layoutNodeMap[waveName], NULL);
            if (w && widgetLayoutable[w->name()])
            {
                w->setVisible(true);
                w->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
                vLayout->addWidget(w, 1);
                displayWaveforms.append(w->name());
                contentWidgets.append(w);
            }
            column++;
        }
        row++;
    }
    waveRowCount = 0;
    leftParamRowCount = 0;
}

void LayoutManagerPrivate::performTrendLayout()
{
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setContentsMargins(0, 0, 0, 0);
    QWidget *waveContainer = createContainter();
    QWidget *leftParamContainer = createContainter();

    QHBoxLayout *hLayout = new QHBoxLayout();
    ShortTrendContainer *trendContainer = qobject_cast<ShortTrendContainer *> (layoutWidgets["ShortTrendContainer"]);
    if (trendContainer)
    {
        hLayout->addWidget(trendContainer, 3);
        hLayout->addWidget(waveContainer, 5);
        leftLayout->addLayout(hLayout);
        leftLayout->addWidget(leftParamContainer);
        trendContainer->setVisible(true);
        trendContainer->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        contentWidgets.append(trendContainer);
    }

    contentLayout->addLayout(leftLayout, 8);

    QWidget *rightParamContainer = createContainter();
    contentLayout->addWidget(rightParamContainer, 4);

    QGridLayout *waveLayout = new QGridLayout(waveContainer);
    waveLayout->setMargin(0);
    QGridLayout *leftParamLayout = new QGridLayout(leftParamContainer);
    leftParamLayout->setMargin(0);
    QGridLayout *rightParamLayout = new QGridLayout(rightParamContainer);
    rightParamLayout->setMargin(0);

    QList<TrendWidget *> rightTrendWidgets;

    OrderedMap<int, LayoutRow>::ConstIterator iter = layoutInfos.begin();
    int insetRow = 0;
    int lastWaveRow = 0;    // 记录最后波形区最后一行行数
    for (; iter != layoutInfos.end(); ++iter)
    {
        LayoutRow::ConstIterator nodeIter = iter.value().constBegin();
        for (; nodeIter != iter.value().constEnd(); ++nodeIter)
        {
            int row = iter.key();
            IWidget *w = layoutWidgets.value(layoutNodeMap[nodeIter->name], NULL);
            QWidget *qw = w;
            if (!qw)
            {
                qw = createContainter();
            }

            if (w)
            {
                contentWidgets.append(w);
            }
            qw->setVisible(true);
            qw->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            if (nodeIter->pos < LAYOUT_WAVE_END_COLUMN)
            {
                if (row < LAYOUT_MAX_WAVE_ROW_NUM)
                {
                    waveLayout->addWidget(qw, insetRow, nodeIter->pos, 1, nodeIter->span);
                    waveLayout->setRowStretch(insetRow, 1);
                    lastWaveRow = insetRow;
                    if (w)
                    {
                        displayWaveforms.append(w->name());
                    }
                }
                else
                {
                    leftParamLayout->addWidget(qw, insetRow - lastWaveRow - 1, nodeIter->pos, 1, nodeIter->span);
                    leftParamLayout->setRowStretch(insetRow - lastWaveRow - 1 , 1);
                    if (w)
                    {
                        displayParams.append(w->name());
                    }
                }
            }
            else
            {
                rightParamLayout->addWidget(qw, row, nodeIter->pos - LAYOUT_WAVE_END_COLUMN, 1, nodeIter->span);
                rightParamLayout->setRowStretch(row, 1);
                if (w)
                {
                    displayParams.append(w->name());
                }
                if (nodeIter->pos == LAYOUT_WAVE_END_COLUMN)  // the first trend node on each row
                {
                    TrendWidget *trendWidget = qobject_cast<TrendWidget *>(w);
                    if (trendWidget)
                    {
                        rightTrendWidgets.append(trendWidget);
                    }
                }
            }
        }
        if (!(*iter).isEmpty())
        {
            insetRow++;
        }
    }

    // the wave container stretch
    leftLayout->setStretch(0, waveLayout->rowCount());
    waveRowCount = waveLayout->rowCount();

    // the let param container stretch
    if (leftParamLayout->count() != 0)
    {
        leftLayout->setStretch(1, leftParamLayout->rowCount());
        leftParamRowCount = leftParamLayout->rowCount();
    }
    else
    {
        leftParamRowCount = 0;
    }

    typedef QList<SubParamID> SubParamIDListType;

    QList<SubParamIDListType> list;

    // update the trend item
    foreach(const TrendWidget *trend, rightTrendWidgets)
    {
        QList<SubParamID> subParams = trend->getShortTrendSubParams();
        if (subParams.count() && list.count() <= lastWaveRow)
        {
            list.append(subParams);
        }
    }

    // create trend items
    trendContainer->setTrendItemNum(list.count());

    for (int i = 0; i < list.count(); ++i)
    {
        trendContainer->addSubParamToTrendItem(i, list.at(i));
    }
    trendContainer->updateDefautlTrendList();
}

void LayoutManagerPrivate::performSPO2Layout()
{
    QVBoxLayout *layout = new QVBoxLayout();
    QWidget *spo2WaveContainter = createContainter();
    layout->addWidget(spo2WaveContainter);
    QHBoxLayout *hLayout = new QHBoxLayout();
    layout->addLayout(hLayout);
    QWidget *trendWaveContainter = createContainter();
    QWidget *paramContainter = createContainter();
    hLayout->addWidget(trendWaveContainter);
    hLayout->addWidget(paramContainter);
    contentLayout->addLayout(layout);

    QVBoxLayout *spo2WaveLayout = new QVBoxLayout(spo2WaveContainter);
    QGridLayout *trendWaveLayout = new QGridLayout(trendWaveContainter);
    QGridLayout *paramLayout = new QGridLayout(paramContainter);

    QVariantMap spo2LayoutInfos;
    if (!spo2Param.isConnected(true))
    {
        // 单血氧界面
        spo2LayoutInfos = systemConfig.getConfig("PrimaryCfg|UILayout|ContentLayout|SPO2Single");
    }
    else
    {
        // 双血氧界面
        spo2LayoutInfos = systemConfig.getConfig("PrimaryCfg|UILayout|ContentLayout|SPO2Double");
    }
    QVariantList layoutRows = spo2LayoutInfos["LayoutRow"].toList();
    int index = 0;
    currentConfig.getNumValue("SPO2|Sensor", index);
    SPO2RainbowSensor sensor = static_cast<SPO2RainbowSensor>(index);

    if (layoutRows.isEmpty())
    {
        // maybe has only one
        QVariant row = spo2LayoutInfos["LayoutRow"];
        if (row.isValid())
        {
            layoutRows.append(row);
        }
    }

    int insetRow = 0;
    QVariantList::ConstIterator rowIter = layoutRows.constBegin();
    for (; rowIter != layoutRows.constEnd(); ++rowIter)
    {
        QVariantMap rowMap = rowIter->toMap();
        QVariantList nodes = rowMap["LayoutNode"].toList();
        if (nodes.isEmpty())
        {
            // maybe has only one
            QVariant node = rowMap["LayoutNode"];
            if (node.isValid())
            {
                nodes.append(node);
            }
        }
        bool flag = false;
        QVariantList::ConstIterator nodeIter = nodes.constBegin();
        for (; nodeIter != nodes.constEnd(); ++nodeIter)
        {
            QVariantMap node = nodeIter->toMap();
            QString nodeName = node["@text"].toString();
            int nodeSpan = node["@span"].toInt();
            int nodePos = node["@pos"].toInt();

            // change layout accord current sensor
            if (sensor == SPO2_RAINBOW_SENSOR_R25)
            {
                if (nodeName == QString(layoutNodeName(LAYOUT_NODE_PARAM_SPHB)))
                {
                    nodeName = QString(layoutNodeName(LAYOUT_NODE_PARAM_SPCO));
                }
                else if (nodeName == QString(layoutNodeName(LAYOUT_NODE_PARAM_SPOC)))
                {
                    continue;
                }
                else if (nodeName == QString(layoutNodeName(LAYOUT_NODE_TREND_WAVE_SPHB)))
                {
                    nodeName = QString(layoutNodeName(LAYOUT_NODE_TREND_WAVE_SPCO));
                }
            }

            IWidget *w = layoutWidgets.value(layoutNodeMap[nodeName], NULL);
            QWidget *qw = w;
            if (!qw)
            {
                qw = createContainter();
            }
            if (w)
            {
                contentWidgets.append(w);
            }

            qw->setVisible(true);
            qw->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            if (nodePos == 0 && nodeSpan == LAYOUT_COLUMN_COUNT)
            {
                // add spo2 wave
                spo2WaveLayout->addWidget(qw);
                displayWaveforms.append(w->name());
            }
            else if (nodePos == 0 && nodeSpan == LAYOUT_WAVE_END_COLUMN)
            {
                // add trend wave
                trendWaveLayout->addWidget(qw, insetRow, nodePos, 1, nodeSpan);
                flag = true;
            }
            else if (nodePos >= LAYOUT_WAVE_END_COLUMN)
            {
                // add param
                paramLayout->addWidget(qw, insetRow, nodePos - LAYOUT_WAVE_END_COLUMN, 1, nodeSpan);
                displayParams.append(w->name());
                flag = true;
            }
        }
        if (flag)
        {
            insetRow++;
        }
    }

    layout->setStretch(0, spo2WaveLayout->count());
    layout->setStretch(1, insetRow);
    hLayout->setStretch(0, LAYOUT_WAVE_END_COLUMN);
    hLayout->setStretch(1, LAYOUT_COLUMN_COUNT - LAYOUT_WAVE_END_COLUMN);
    waveRowCount = insetRow + spo2WaveLayout->count();
    leftParamRowCount = 0;
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
    waveRowCount = 0;
    leftParamRowCount = 0;
}

void LayoutManagerPrivate::updateTabOrder()
{
    QList<QWidget *> widgets;
    QList<QWidget *> subWidgets;
    foreach(IWidget *w, mainLayoutWidgets)
    {
        if (w->name() == "ContentView")
        {
            foreach(IWidget *cw, contentWidgets)
            {
                cw->getSubFocusWidget(subWidgets);
                if (subWidgets.isEmpty())
                {
                    widgets.append(cw);
                }
                else
                {
                    widgets.append(subWidgets);
                    subWidgets.clear();
                }
            }
        }
        else
        {
            w->getSubFocusWidget(subWidgets);
            if (subWidgets.isEmpty())
            {
                widgets.append(w);
            }
            else
            {
                widgets.append(subWidgets);
                subWidgets.clear();
            }
        }
    }

    QWidget *lastFocusableWidget = NULL;
    for (int i = 0; i < widgets.count(); ++i)
    {
        if (widgets.at(i)->focusPolicy() == Qt::NoFocus)
        {
            continue;
        }
        if (lastFocusableWidget == NULL)
        {
            lastFocusableWidget = widgets.at(i);
            continue;
        }

        QWidget::setTabOrder(lastFocusableWidget,  widgets.at(i));
        lastFocusableWidget = widgets.at(i);
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
    static HRSourceType lastHrSource = ecgDupParam.getCurHRSource();
    if (!d_ptr->contentLayout->isEmpty() && d_ptr->curUserFace == type)
    {
        return;
    }
    if (type == UFACE_MONITOR_SPO2 && !systemManager.isSupport(PARAM_SPO2))
    {
        // 切换血氧界面且系统不支持血氧时，强制切换为标准界面
        type = UFACE_MONITOR_STANDARD;
    }

    if (d_ptr->curUserFace != UFACE_MONITOR_SPO2 && type == UFACE_MONITOR_SPO2)
    {
        lastHrSource = ecgDupParam.getCurHRSource();
        ecgDupParam.setHrSource(HR_SOURCE_SPO2);
        ecgDupParam.updatePR(0, ecgDupParam.getCurPRSource(), false);   // 切换类型时手动更新pr值，避免上次pr值为无效值
    }
    else if (d_ptr->curUserFace == UFACE_MONITOR_SPO2 && type != UFACE_MONITOR_SPO2)
    {
        ecgDupParam.setHrSource(lastHrSource);
        ecgDupParam.updateHR(ecgDupParam.getHR(true));   // 切换类型时手动更新hr值，避免上次hr值为无效值
    }

    d_ptr->curUserFace = type;

    updateLayout();

    systemConfig.setNumValue("UserFaceType", static_cast<int>(type));

    emit userInterfaceChange(type);
}

UserFaceType LayoutManager::getUFaceType() const
{
    return d_ptr->curUserFace;
}

void LayoutManager::updateLayout()
{
    // find the ECG correspond wave
    int leadMode = ecgParam.getLeadMode();
    int lead = ECG_LEAD_II;
    if (currentConfig.getNumValue("ECG|Ecg1Wave", lead))
    {
        d_ptr->layoutNodeMap[layoutNodeName(LAYOUT_NODE_WAVE_ECG1)]
            = ecgParam.getWaveWidgetName(static_cast<ECGLead>(lead));
    }

    if (leadMode > ECG_LEAD_MODE_3)
    {
        if (currentConfig.getNumValue("ECG|Ecg2Wave", lead))
        {
            d_ptr->layoutNodeMap[layoutNodeName(LAYOUT_NODE_WAVE_ECG2)]
                = ecgParam.getWaveWidgetName(static_cast<ECGLead>(lead));
        }
    }
    else
    {
        d_ptr->layoutNodeMap[layoutNodeName(LAYOUT_NODE_WAVE_ECG2)] = QString();
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

            if (node.name == layoutNodeName(LAYOUT_NODE_PARAM_CO2)
                    || node.name == layoutNodeName(LAYOUT_NODE_WAVE_CO2))
            {
                if (!co2Param.isConnected())
                {
                    continue;
                }
            }
            if (node.name == layoutNodeName(LAYOUT_NODE_PARAM_IBP1)
                    || node.name == layoutNodeName(LAYOUT_NODE_WAVE_IBP1)
                    || node.name == layoutNodeName(LAYOUT_NODE_PARAM_IBP2)
                    || node.name == layoutNodeName(LAYOUT_NODE_WAVE_IBP2))
            {
                if (!ibpParam.isConnected())
                {
                    continue;
                }
            }

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

        bool isRowEmpty = true;
        QList<LayoutNode>::ConstIterator iter = row.begin();
        for (; iter != row.end() ; iter++)
        {
            if ((*iter).name.isEmpty())
            {
                continue;
            }
            isRowEmpty = false;
            break;
        }

        if (!isRowEmpty)
        {
            d_ptr->layoutInfos.insert(curRow, row);
        }
        curRow++;
    }

    // perform layout
    d_ptr->doContentLayout();

    emit layoutChanged();
}

void LayoutManager::patientTypeChangeSlot()
{
    updateLayoutWidgetsConfig();
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
    if (getUFaceType() == UFACE_MONITOR_BIGFONT)
    {
        r.moveTo(0, 0);
        QRect gr(d_ptr->contentView->mapToGlobal(r.topLeft()),
                 d_ptr->contentView->mapToGlobal(r.bottomRight()));
        return gr;
    }
    else
    {
        /*
         * The purpose is to solve the problem of incomplete display of the menu interface
         * when the interface is less than 3 waveforms.
         */
        int displayWaveCount = d_ptr->waveRowCount;
        if (displayWaveCount < 3)
        {
            displayWaveCount = 3;
        }
        // 获得不遮挡第一道波形的菜单显示区域
        int startYPos = r.height() / (displayWaveCount + d_ptr->leftParamRowCount);

        r.moveTo(0, startYPos);
        QRect gr(d_ptr->contentView->mapToGlobal(r.topLeft()),
                 d_ptr->contentView->mapToGlobal(r.bottomRight()));
        gr.setWidth(gr.width() * d_ptr->waveAreaStretch / (d_ptr->waveAreaStretch + d_ptr->paramAreaStretch));
        gr.setHeight(r.height() * (displayWaveCount - 1 + d_ptr->leftParamRowCount)
                     / (displayWaveCount + d_ptr->leftParamRowCount));
        return gr;
    }
}

void LayoutManager::updateTabOrder()
{
    d_ptr->updateTabOrder();
}

void LayoutManager::updateLayoutWidgetsConfig()
{
    QList<IWidget*> wList = d_ptr->layoutWidgets.values();

    foreach(IWidget *w, wList)
    {
        w->updateWidgetConfig();
    }
    updateLayout();
}

LayoutManager::LayoutManager()
    : d_ptr(new LayoutManagerPrivate())
{
    d_ptr->layoutMap = systemConfig.getConfig("PrimaryCfg|UILayout|ContentLayout|Normal");

    addLayoutWidget(d_ptr->contentView);

    connect(&patientManager, SIGNAL(signalPatientType(PatientType)), this, SLOT(patientTypeChangeSlot()));
}
