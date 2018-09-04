/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/30
 **/

#include "ScreenLayoutModel.h"
#include "WindowManager.h"
#include "DemoProvider.h"
#include "ParamManager.h"
#include "ScreenLayoutDefine.h"
#include <QLinkedList>
#include <QVector>
#include <QByteArray>

#define WAVE_SPAN 4
#define PARAM_SPAN 2
#define COLUMN_COUNT 6
#define ROW_COUNT 8

enum ItemType
{
    ITEM_NONE,
    ITEM_PARAM,
    ITEM_WAVE,
};

struct LayoutNode
{
    LayoutNode(): span(1),
        type(ITEM_NONE),
        waveId(WAVE_NONE)
    {}

    LayoutNode(const QString &name, WaveformID waveId, int span)
        : name(name), span(span), waveId(waveId),
          type(ITEM_WAVE)
    {
    }

    LayoutNode(const QString &name, int span)
        : name(name), span(span),
          type(ITEM_NONE),
          waveId(WAVE_NONE)
    {
    }

    QString name;
    int span;
    ItemType type;
    WaveformID waveId;
};

typedef QLinkedList<LayoutNode *> LayoutRow;

class ScreenLayoutModelPrivate
{
public:
    ScreenLayoutModelPrivate()
        : demoProvider(NULL)
    {
        demoProvider = qobject_cast<DemoProvider *>(paramManager.getProvider("DemoProvider"));
    }

    ~ScreenLayoutModelPrivate()
    {
        QVector<LayoutRow *>::iterator iter;
        for (iter = layoutNodes.begin(); iter != layoutNodes.end(); ++iter)
        {
            qDeleteAll(**iter);
        }
        qDeleteAll(layoutNodes);
    }

    /**
     * @brief getWaveData get the wave data from the wave cache, if the wave is not exist in the cache,
     *                    we will try to fetch the data from the demo provider
     * @param waveid
     * @return
     */
    QByteArray getWaveData(WaveformID waveid)
    {
        if (!waveCaches.contains(waveid))
        {
            QByteArray d;
            if (demoProvider)
            {
                d = demoProvider->getDemoWaveData(waveid);
            }
            waveCaches.insert(waveid, d);
        }
        return waveCaches[waveid];
    }

    /**
     * @brief fillWaveData fill the wave info if the @info has a valid waveid
     * @param info the info structure that need to fill wave infos
     */
    void fillWaveData(ScreenLayoutItemInfo &info)
    {
        if (info.waveid == WAVE_NONE || !demoProvider)
        {
            return;
        }

        info.waveContent = getWaveData(info.waveid);
        if (info.waveid >= WAVE_ECG_I && info.waveid <= WAVE_ECG_V6)
        {
            info.baseLine = demoProvider->getBaseLine();
            info.waveMaxValue = 255;
            info.waveMinValue = 0;
            info.sampleRate = demoProvider->getWaveformSample();
        }
        else if (info.waveid == WAVE_RESP)
        {
            info.waveMaxValue = demoProvider->maxRESPWaveValue();
            info.waveMinValue = demoProvider->minRESPWaveValue();
            info.baseLine = demoProvider->getRESPBaseLine();
            info.sampleRate = demoProvider->getRESPWaveformSample();
        }
        else if (info.waveid == WAVE_SPO2)
        {
            info.waveMaxValue = demoProvider->getSPO2MaxValue();
            info.waveMinValue = 0;
            info.baseLine = demoProvider->getSPO2BaseLine();
            info.sampleRate = demoProvider->getSPO2WaveformSample();
        }
        else if (info.waveid == WAVE_CO2)
        {
            info.waveMaxValue = demoProvider->getCO2MaxWaveform();
            info.drawSpeed = 6.25;
            info.waveMinValue = 0;
            info.baseLine = demoProvider->getCO2BaseLine();
            info.sampleRate = demoProvider->getSPO2WaveformSample();
        }
        else if (info.waveid >= WAVE_N2O && info.waveid <= WAVE_O2)
        {
            info.waveMaxValue = demoProvider->getN2OMaxWaveform();
            info.waveMinValue = 0;
            info.baseLine = demoProvider->getN2OBaseLine();
            info.sampleRate = demoProvider->getN2OWaveformSample();
        }
        else if (info.waveid >= WAVE_ART && info.waveid <= WAVE_AUXP2)
        {
            info.waveMaxValue = demoProvider->getIBPMaxWaveform();
            info.waveMinValue = 0;
            info.baseLine = demoProvider->getIBPBaseLine();
            info.sampleRate = demoProvider->getIBPWaveformSample();
        }
    }

    void loadLayoutNodes()
    {
        LayoutRow *row = new LayoutRow();
        row->append(new LayoutNode("ECG", WAVE_ECG_II, 4));
        row->append(new LayoutNode("ECG", 2));
        layoutNodes.append(row);

        row = new LayoutRow();
        row->append(new LayoutNode("Pleth", WAVE_SPO2, 4));
        row->append(new LayoutNode("Spo2", 2));
        layoutNodes.append(row);

        row = new LayoutRow();
        row->append(new LayoutNode("Resp", WAVE_RESP, 4));
        row->append(new LayoutNode("Resp", 2));
        layoutNodes.append(row);

        row = new LayoutRow();
        row->append(new LayoutNode("ART", WAVE_ART, 4));
        row->append(new LayoutNode("ART", 2));
        layoutNodes.append(row);

        row = new LayoutRow();
        row->append(new LayoutNode("Co2", WAVE_CO2, 4));
        row->append(new LayoutNode("Co2", 2));
        layoutNodes.append(row);

        row = new LayoutRow();
        row->append(new LayoutNode("NIBP", 2));
        row->append(new LayoutNode("NIBPList", 2));
        row->append(new LayoutNode("TEMP", 2));
        layoutNodes.append(row);
    }

    /**
     * @brief findNode find a layout node at the modelindex
     * @param index the index of the model
     * @return the layout node if exists, otherwise return NULL
     */
    LayoutNode *findNode(const QModelIndex &index)
    {
        if (!index.isValid() || index.row() >= layoutNodes.count() || index.column() >= COLUMN_COUNT)
        {
            return NULL;
        }

        LayoutRow *r = layoutNodes[index.row()];
        LayoutRow::ConstIterator iter = r->constBegin();
        int nodeStart = 0;
        for (; iter != r->end(); ++iter)
        {
            if (nodeStart == index.column())
            {
                return *iter;
            }
            nodeStart += (*iter)->span;
        }

        return NULL;
    }

    DemoProvider *demoProvider;
    QMap<WaveformID, QByteArray> waveCaches;
    QVector<LayoutRow *> layoutNodes;
};

ScreenLayoutModel::ScreenLayoutModel(QObject *parent)
    : QAbstractTableModel(parent), d_ptr(new ScreenLayoutModelPrivate)
{
    d_ptr->loadLayoutNodes();
}

int ScreenLayoutModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return COLUMN_COUNT;
}

int ScreenLayoutModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ROW_COUNT;
}

QVariant ScreenLayoutModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
    case Qt::DisplayRole:
    {
        LayoutNode *node = d_ptr->findNode(index);
        if (node)
        {
            ScreenLayoutItemInfo info;
            info.waveid = node->waveId;
            info.name = node->name;
            d_ptr->fillWaveData(info);
            return qVariantFromValue(info);
        }
    }
    break;
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags ScreenLayoutModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QAbstractTableModel::flags(index);
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QSize ScreenLayoutModel::span(const QModelIndex &index) const
{
    if (index.isValid())
    {
        LayoutNode *node = d_ptr->findNode(index);
        // if the node exist, use the node's span value, otherwise, use the default value
        if (node)
        {
            return QSize(node->span, 1);
        }
    }
    return QAbstractTableModel::span(index);
}

ScreenLayoutModel::~ScreenLayoutModel()
{
    delete d_ptr;
}
