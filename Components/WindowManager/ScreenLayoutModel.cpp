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

#define COLUMN_COUNT 6
#define ROW_COUNT 8
#define MAX_WAVE_ROW 6
#define WAVE_START_COLUMN 4

enum ItemType
{
    ITEM_PLACE_HOLDER,
    ITEM_PARAM,
    ITEM_WAVE,
};

struct LayoutNode
{
    LayoutNode(int pos, int span)
        : pos(pos), span(span), type(ITEM_PLACE_HOLDER), waveId(WAVE_NONE)
    {}

    LayoutNode(const QString &name, WaveformID waveId, int pos, int span)
        : name(name), pos(pos), span(span), type(ITEM_WAVE), waveId(waveId)
    {
    }

    LayoutNode(const QString &name, int pos, int span)
        : name(name), pos(pos), span(span), type(ITEM_PLACE_HOLDER), waveId(WAVE_NONE)
    {
    }

    QString name;
    int pos;
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
        row->append(new LayoutNode("ECG", WAVE_ECG_II, 0, 4));
        row->append(new LayoutNode("ECG", 4, 2));
        layoutNodes.append(row);

        row = new LayoutRow();
        row->append(new LayoutNode("Pleth", WAVE_SPO2, 0, 4));
        row->append(new LayoutNode("Spo2", 4, 2));
        layoutNodes.append(row);

        row = new LayoutRow();
        row->append(new LayoutNode("Resp", WAVE_RESP, 0, 4));
        row->append(new LayoutNode("Resp", 4, 2));
        layoutNodes.append(row);

        row = new LayoutRow();
        row->append(new LayoutNode("ART", WAVE_ART, 0, 4));
        row->append(new LayoutNode("ART", 4, 2));
        layoutNodes.append(row);

        row = new LayoutRow();
        row->append(new LayoutNode("PA", WAVE_ART, 0, 4));
        row->append(new LayoutNode("PA", 4, 2));
        layoutNodes.append(row);

        row = new LayoutRow();
        row->append(new LayoutNode("Co2", WAVE_CO2, 0, 4));
        row->append(new LayoutNode("Co2", 4, 2));
        layoutNodes.append(row);

        row = new LayoutRow();
        row->append(new LayoutNode("NIBP", 0, 2));
        row->append(new LayoutNode("NIBPList", 2, 2));
        row->append(new LayoutNode("TEMP", 4, 2));
        layoutNodes.append(row);

        row = new LayoutRow();
        row->append(new LayoutNode(trs("Off"), 0, 2));
        row->append(new LayoutNode(trs("Off"), 2, 2));
        row->append(new LayoutNode(trs("Off"), 4, 2));
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
        for (; iter != r->end(); ++iter)
        {
            if ((*iter)->pos == index.column())
            {
                return *iter;
            }
        }

        return NULL;
    }


    DemoProvider *demoProvider;
    QMap<WaveformID, QByteArray> waveCaches;
    QVector<LayoutRow *> layoutNodes;
    QList<QString> supportWaveforms;
    QList<QString> supportParams;
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

bool ScreenLayoutModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        if (value.type() == QVariant::String)
        {
            LayoutNode *node = d_ptr->findNode(index);
            if (node)
            {
                QString text = value.toString();
                node->name = text;
            }
        }

        return true;
    }

    return false;
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
    case Qt::EditRole:
    {
        if (index.column() >= WAVE_START_COLUMN && index.row() < MAX_WAVE_ROW)
        {
            return QVariant(d_ptr->supportWaveforms);
        }

        return QVariant(d_ptr->supportParams);
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

    if (index.row() < MAX_WAVE_ROW && index.column() < WAVE_START_COLUMN)
    {
        // the waveform area is not selectable or editable
        return Qt::ItemIsEnabled;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
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

    return QSize();
}

void ScreenLayoutModel::saveLayoutInfo()
{
}

void ScreenLayoutModel::loadLayoutInfo()
{
}

void ScreenLayoutModel::updateWaveAndParamInfo()
{
    QStringList waveList;
    waveList << "ECG" << "RESP" << "SPO2" << "CO2" << "ART" << "PA" << trs("Off");
    d_ptr->supportWaveforms = waveList;

    QStringList paramList;
    paramList << "SPO2" << "C.O." << "CO2" << "RESP" << "NIBP" << "NIBPList" << trs("Off");
    d_ptr->supportParams = paramList;
}

ScreenLayoutModel::~ScreenLayoutModel()
{
    delete d_ptr;
}
