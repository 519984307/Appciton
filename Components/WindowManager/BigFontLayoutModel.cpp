/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/26
 **/

#include "BigFontLayoutModel.h"
#include "ScreenLayoutDefine.h"
#include "OrderedMap.h"
#include "ParamInfo.h"
#include <QStringList>
#include "IConfig.h"
#include "DemoProvider.h"
#include "ParamManager.h"
#include "IBPParam.h"

struct ParamNodeDescription
{
    ParamNodeDescription(){}
    explicit ParamNodeDescription(const QString &displayName)
        : displayName(displayName){}
    QString displayName;
};

typedef QList<LayoutNode *> LayoutRow;

class BigFontLayoutModelPrivate
{
public:
    BigFontLayoutModelPrivate()
        : demoProvider(NULL)
    {
        demoProvider = qobject_cast<DemoProvider *>(paramManager.getProvider("DemoProvider"));
    }
    ~BigFontLayoutModelPrivate()
    {
        clearAllLayoutNodes();
    }

    LayoutNode * findNode(const QModelIndex &index);
    void fillWaveData(ScreenLayoutItemInfo &info);
    bool itemInWaveRegion(const QModelIndex &index);
    /**
     * @brief getUnlayoutedWaveforms  获得没有被布局的波形
     * @return  没有被布局波形的列表
     */
    QStringList getUnlayoutedWaveforms() const;

    /**
     * @brief getUnlayoutedParams  获得没有被布局的参数
     * @return
     */
    QStringList getUnlayoutedParams() const;

    /**
     * @brief getLayoutMap 从当前配置中获取布局map
     * @return
     */
    QVariantMap getLayoutMap() const;

    /**
     * @brief clearAllLayoutNodes 清除全部节点
     */
    void clearAllLayoutNodes();
    /**
     * @brief loadLayoutFromConfig 　从配置中加载布局信息
     * @param config
     */
    void loadLayoutFromConfig(const QVariantMap &config);

    /**
     * @brief loadItemInfos 加载项信息
     */
    void loadItemInfos();

    /**
     * @brief getWaveData 从缓存区获取波形数据，如果缓存区不存在，则从demoProvier中获取
     * @param waveid
     * @return
     */
    QByteArray getWaveData(WaveformID waveid);

    /**
     * @brief getLayoutedWaveforms 获取屏幕中的波形窗口
     * @return the waveform name list
     */
    QStringList getLayoutedWaveforms() const;

    /**
     * @brief getLayoutedParams get the layouted parameters
     * @return
     */
    QStringList getLayoutedParams() const;

    /**
     * @brief findWaveNameFromParam    从参数名称获得对应的波形名称
     * @param paramStr
     * @return
     */
    QString findWaveNameFromParam(QString paramStr);

    DemoProvider *demoProvider;
    OrderedMap<QString , WaveformID> waveIDMaps;
    OrderedMap<QString, ParamNodeDescription> paramNodeDescriptions;     // store param node's description
    QVector<LayoutRow *> layoutNodes;
    QMap<WaveformID, QByteArray> waveCaches;
};
BigFontLayoutModel::BigFontLayoutModel(QObject *parent)
    : QAbstractTableModel(parent),
      d_ptr(new BigFontLayoutModelPrivate)
{
}

BigFontLayoutModel::~BigFontLayoutModel()
{
    delete d_ptr;
}

int BigFontLayoutModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
//    return LAYOUT_ITEM_ROW_COUNT;
    return LAYOUT_COLUMN_COUNT;
}

int BigFontLayoutModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
//    return LAYOUT_WAVE_ROW_POS;
    return LAYOUT_ROW_COUNT;
}

bool BigFontLayoutModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role) {
    case ReplaceRole:
    case InsertRole:
    {
        QString nodeName = value.toString();
        QModelIndex anotherIndex = index;
        if (!nodeName.isEmpty())
        {
            LayoutNode *node = d_ptr->findNode(index);
            if (node)
            {
                node->name = nodeName;
            }
            QModelIndex waveIndex = this->index(index.row() + LAYOUT_PARAM_WAVE_SPACING
                                                , index.column());

            LayoutNode *waveNode = d_ptr->findNode(waveIndex);
            if (waveNode && d_ptr->itemInWaveRegion(waveIndex))
            {
                waveNode->name = d_ptr->findWaveNameFromParam(nodeName);
                anotherIndex = waveIndex;
            }
            emit dataChanged(index, anotherIndex);
        }
        break;
    }
    case RemoveRole:
    {
        LayoutNode *node = d_ptr->findNode(index);
        QModelIndex anotherIndex = index;
        if (node)
        {
            node->name = QString();
            QModelIndex waveIndex = this->index(index.row() + LAYOUT_PARAM_WAVE_SPACING
                                                , index.column());
            LayoutNode *waveNode = d_ptr->findNode(waveIndex);
            if (waveNode && d_ptr->itemInWaveRegion(waveIndex))
            {
                anotherIndex = waveIndex;
                waveNode->name = QString();
            }
            emit dataChanged(index, anotherIndex);
        }
        break;
    }
    default:
        break;
    }
    return false;
}

QVariant BigFontLayoutModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
        break;
    case Qt::DisplayRole:
    {
        LayoutNode *node = d_ptr->findNode(index);
        if (node)
        {
            ScreenLayoutItemInfo info;
            info.waveid = d_ptr->waveIDMaps.value(node->name, WAVE_NONE);
            info.name = node->name;
            if (info.waveid != WAVE_NONE)
            {
                info.displayName = paramInfo.getParamWaveformName(info.waveid);
            }
            else
            {
                ParamNodeDescription paramNode = d_ptr->paramNodeDescriptions.value(node->name);
                if (!paramNode.displayName.isEmpty())
                {
                    info.displayName = paramNode.displayName;
                }
                else
                {
                    info.displayName = info.name;
                }
            }
            d_ptr->fillWaveData(info);
            return qVariantFromValue(info);
        }
        break;
    }
    case ReplaceRole:
    {
        LayoutNode *node = d_ptr->findNode(index);
        if (node)
        {
            if (node->name.isEmpty())
            {
                break;
            }
        }
        else
        {
            break;
        }
        if (d_ptr->itemInWaveRegion(index))     // 波形
        {
            QStringList replaceWaveforms = d_ptr->getUnlayoutedWaveforms();
            if (replaceWaveforms.isEmpty())
            {
                break;
            }
            replaceWaveforms.append(node->name);
            QVariantList list;
            QStringList::ConstIterator iter = replaceWaveforms.constBegin();
            for (; iter != replaceWaveforms.end() ; iter++)
            {
                QVariantMap m;
                m["@name"] = *iter;
                m["@displayName"] = paramInfo.getParamWaveformName(d_ptr->waveIDMaps.value(*iter, WAVE_NONE));
                list.append(m);
            }
            return list;
        }
        else    // 参数
        {
            QStringList replaceParams = d_ptr->getUnlayoutedParams();
            if (replaceParams.isEmpty())
            {
                break;
            }
            replaceParams.append(node->name);
            QVariantList list;
            QStringList::ConstIterator iter = replaceParams.begin();
            for (; iter != replaceParams.end() ; iter++)
            {
                QVariantMap m;
                m["name"] = *iter;
                m["displayName"] = d_ptr->paramNodeDescriptions[*iter].displayName;
                list.append(m);
            }
            return list;
        }

        break;
    }
    case InsertRole:
    {
        LayoutNode *node = d_ptr->findNode(index);
        if (node)
        {
            if (!node->name.isEmpty())
            {
                break;
            }
        }

        if (d_ptr->itemInWaveRegion(index))
        {
            QVariantList list;
            QStringList unlayoutWaves = d_ptr->getUnlayoutedWaveforms();
            QStringList::ConstIterator iter = unlayoutWaves.constBegin();
            for (; iter != unlayoutWaves.constEnd(); ++iter)
            {
                QVariantMap m;
                m["name"] = *iter;
                m["displayName"] = paramInfo.getParamWaveformName(d_ptr->waveIDMaps.value(*iter, WAVE_NONE));
                list.append(m);
            }
            return list;
        }
        else
        {
            QVariantList list;
            QStringList unlayoutParams = d_ptr->getUnlayoutedParams();
            QStringList::ConstIterator iter = unlayoutParams.constBegin();
            for (; iter != unlayoutParams.constEnd(); ++iter)
            {
                QVariantMap m;
                m["name"] = *iter;
                m["displayName"] = d_ptr->paramNodeDescriptions[*iter].displayName;
                list.append(m);
            }
            return list;
        }
        break;
    }
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags BigFontLayoutModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QAbstractTableModel::flags(index);
    }
    LayoutNode *node = d_ptr->findNode(index);
    bool editable = false;
    if (node)
    {
        editable = node->editable;
    }
    if (editable)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    else
    {
        return Qt::NoItemFlags;
    }
}

QSize BigFontLayoutModel::span(const QModelIndex &index) const
{
    if (index.isValid())
    {
        LayoutNode *node = d_ptr->findNode(index);
        // if the node exist, use the node's span value, otherwise, use the default value
        if (node)
        {
            if (d_ptr->itemInWaveRegion(index))
            {
                return QSize(node->span, 1);
            }
            else
            {
                return QSize(node->span, node->span);
            }
        }
    }

    return QSize();
}

void BigFontLayoutModel::saveLayoutInfo()
{
    systemConfig.setConfig("PrimaryCfg|UILayout|ContentLayout|BigFont", d_ptr->getLayoutMap());
}

void BigFontLayoutModel::loadLayoutInfo()
{
    const QVariantMap config = systemConfig.getConfig("PrimaryCfg|UILayout|ContentLayout|BigFont");
    beginResetModel();
    d_ptr->loadLayoutFromConfig(config);
    endResetModel();

    int row = rowCount();
    int column = columnCount();
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            QModelIndex idx = index(i, j);
            if (d_ptr->findNode(idx))
            {
                emit spanChanged(idx);
            }
        }
    }
}

void BigFontLayoutModel::updateWaveAndParamInfo()
{
    d_ptr->loadItemInfos();
}

LayoutNode *BigFontLayoutModelPrivate::findNode(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= layoutNodes.count() || index.column() >= LAYOUT_ROW_COUNT)
    {
        return NULL;
    }
    LayoutRow *r = layoutNodes[index.row()];
    LayoutRow::ConstIterator iter = r->constBegin();
    for (; iter != r->constEnd(); iter++)
    {
        if ((*iter)->pos == index.column())
        {
            return *iter;
        }
    }
    return NULL;
}

void BigFontLayoutModelPrivate::fillWaveData(ScreenLayoutItemInfo &info)
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

bool BigFontLayoutModelPrivate::itemInWaveRegion(const QModelIndex &index)
{
    if (index.isValid() && ((index.row() + 1) % LAYOUT_PARAM_ROW_SPACING == 0))
    {
        return true;
    }
    return false;
}

QStringList BigFontLayoutModelPrivate::getUnlayoutedWaveforms() const
{
    QStringList existWaveforms = getLayoutedWaveforms();
    QStringList avaliablesWaveforms = waveIDMaps.keys();
    QStringList unlayoutedWaveforms;
    QStringList::ConstIterator iter;
    for (iter = avaliablesWaveforms.constBegin(); iter != avaliablesWaveforms.constEnd(); ++iter)
    {
        if (existWaveforms.contains(*iter))
        {
            continue;
        }

        unlayoutedWaveforms.append(*iter);
    }
    return unlayoutedWaveforms;
}

QStringList BigFontLayoutModelPrivate::getUnlayoutedParams() const
{
    QStringList existParams = getLayoutedParams();
    QStringList avaliableParams = paramNodeDescriptions.keys();
    QStringList unlayoutedParams;
    QStringList::ConstIterator iter;
    for (iter = avaliableParams.constBegin(); iter != avaliableParams.constEnd(); ++iter)
    {
        if (existParams.contains(*iter))
        {
            continue;
        }
        unlayoutedParams.append(*iter);
    }
    return unlayoutedParams;
}

QVariantMap BigFontLayoutModelPrivate::getLayoutMap() const
{
    QVariantList layoutRows;
    QVector<LayoutRow *>::ConstIterator iter = layoutNodes.constBegin();
    QVector<LayoutRow *>::ConstIterator endIter = layoutNodes.constEnd();
    for (; iter != endIter; iter += LAYOUT_PARAM_ROW_SPACING)
    {
        QVariantList nodes;

        LayoutRow::ConstIterator paraIter = (*iter)->constBegin();
        LayoutRow::ConstIterator waveIter = (*(iter+LAYOUT_PARAM_WAVE_SPACING))->constBegin();
        for (; paraIter != (*iter)->constEnd(); paraIter++, waveIter++)
        {
            LayoutNode *paraNode = *paraIter;
            LayoutNode *waveNode = *waveIter;
            QVariantMap pn, wn;
            QVariantMap node;

            pn["@span"] = paraNode->span;
            pn["@editable"] = paraNode->editable;
            pn["@pos"] = paraNode->pos;
            pn["@text"] = paraNode->name;
            wn["@span"] = waveNode->span;
            wn["@editable"] = waveNode->editable;
            wn["@pos"] = waveNode->pos;
            wn["@text"] = waveNode->name;
            node["Param"] = pn;
            node["Wave"] = wn;

            nodes.append(node);
        }

        QVariantMap nm;
        nm["LayoutNode"] = nodes;
        layoutRows.append(nm);
    }

    QVariantMap rm;
    rm["LayoutRow"] = layoutRows;

    return rm;
}
void BigFontLayoutModelPrivate::clearAllLayoutNodes()
{
    QVector<LayoutRow *>::iterator iter;
    for (iter = layoutNodes.begin(); iter != layoutNodes.end(); ++iter)
    {
        qDeleteAll(**iter);
        (*iter)->clear();
    }
    qDeleteAll(layoutNodes);
    layoutNodes.clear();
}

void BigFontLayoutModelPrivate::loadLayoutFromConfig(const QVariantMap &config)
{
    clearAllLayoutNodes();

    QVariantList layoutRows = config["LayoutRow"].toList();
    if (layoutRows.isEmpty())
    {
        QVariant tmp = config["LayoutRow"];
        if (tmp.isValid())
        {
            layoutRows.append(tmp.toMap());
        }
        return;
    }

    QVariantList::ConstIterator iter = layoutRows.constBegin();
    for (; iter != layoutRows.constEnd(); iter++)
    {
        QVariantList nodes = iter->toMap().value("LayoutNode").toList();
        if (nodes.isEmpty())
        {
            QVariantMap tmp = iter->toMap().value("LayoutNode").toMap();
            if (tmp.isEmpty())
            {
                continue;
            }
            nodes.append(tmp);
        }

        LayoutRow *paramRow = new LayoutRow();
        LayoutRow *waveRow = new LayoutRow();
        LayoutRow *blankRow = new LayoutRow();

        QVariantList::ConstIterator nodeIter = nodes.constBegin();
        for (; nodeIter != nodes.constEnd(); nodeIter += LAYOUT_NODE_ITEM_COUNT)
        {
            QVariantMap node1Map = nodeIter->toMap();
            QVariantMap node2Map = (nodeIter+1)->toMap();

            QVariantList nodeParam;
            nodeParam.append(node1Map["Param"]);
            nodeParam.append(node2Map["Param"]);

            QVariantList nodeWave;
            nodeWave.append(node1Map["Wave"]);
            nodeWave.append(node2Map["Wave"]);

            QVariantList::ConstIterator pIter = nodeParam.constBegin();
            for (; pIter!= nodeParam.constEnd(); pIter++)
            {
                QVariantMap _nodePara = pIter->toMap();
                LayoutNode *node = new LayoutNode();
                node->pos = _nodePara["@pos"].toInt();
                node->span = _nodePara["@span"].toInt();
                node->editable = _nodePara["@editable"].toBool();
                node->name = _nodePara["@text"].toString();
                if (paramRow->isEmpty())
                {
                    paramRow->append(node);
                }
                else
                {
                    // the layout should be sorted by the pos
                    LayoutRow::Iterator iter = paramRow->begin();
                    for (; iter != paramRow->end(); ++iter)
                    {
                        if ((*iter)->pos > node->pos)
                        {
                            break;
                        }
                    }
                    paramRow->insert(iter, node);
                }
            }

            QVariantList::ConstIterator wIter = nodeWave.constBegin();
            for (; wIter != nodeWave.constEnd(); wIter++)
            {
                QVariantMap _nodeWave = wIter->toMap();
                LayoutNode *node = new LayoutNode();
                node->pos = _nodeWave["@pos"].toInt();
                node->span = _nodeWave["@span"].toInt();
                node->editable = _nodeWave["@editable"].toBool();
                node->name = _nodeWave["@text"].toString();

                if (waveRow->isEmpty())
                {
                    waveRow->append(node);
                }
                else
                {
                    // the layout should be sorted by the pos
                    LayoutRow::Iterator iter = waveRow->begin();
                    for (; iter != waveRow->end(); ++iter)
                    {
                        if ((*iter)->pos > node->pos)
                        {
                            break;
                        }
                    }
                    waveRow->insert(iter, node);
                }
            }
        }
        layoutNodes.append(paramRow);
        layoutNodes.append(blankRow);
        layoutNodes.append(blankRow);
        layoutNodes.append(waveRow);
    }
}

void BigFontLayoutModelPrivate::loadItemInfos()
{
    // two ecg wave at most
    // TODO: find the proper ECG Wave
    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_ECG1), WAVE_ECG_I);
    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_ECG2), WAVE_ECG_II);

    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_RESP), WAVE_RESP);
    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_SPO2), WAVE_SPO2);
    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_CO2), WAVE_CO2);

    // find proper IBP Wave base of the wave name
    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_IBP1),
                      ibpParam.getWaveformID(ibpParam.getEntitle(IBP_INPUT_1)));
    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_IBP2),
                      ibpParam.getWaveformID(ibpParam.getEntitle(IBP_INPUT_2)));

    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_N2O), WAVE_N2O);
    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_AA1), WAVE_AA1);
    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_AA2), WAVE_AA2);
    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_O2), WAVE_O2);


    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_ECG)] = ParamNodeDescription(paramInfo.getParamName(PARAM_ECG));
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_SPO2)] = ParamNodeDescription(paramInfo.getParamName(PARAM_SPO2));
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_RESP)] = ParamNodeDescription(paramInfo.getParamName(PARAM_RESP));
    // IBP's pressure name is identical to it's wave name
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_IBP1)] = ParamNodeDescription(
                paramInfo.getParamWaveformName(waveIDMaps[layoutNodeName(LAYOUT_NODE_WAVE_IBP1)]));
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_IBP2)] = ParamNodeDescription(
                paramInfo.getParamWaveformName(waveIDMaps[layoutNodeName(LAYOUT_NODE_WAVE_IBP2)]));
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_CO2)] = ParamNodeDescription(paramInfo.getParamName(PARAM_CO2));
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_NIBP)] = ParamNodeDescription(paramInfo.getParamName(PARAM_NIBP));
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_NIBPLIST)] = ParamNodeDescription(trs("NIBPList"));
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_TEMP)] = ParamNodeDescription(paramInfo.getParamName(PARAM_TEMP));

    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_AA1)] = ParamNodeDescription("AA1");
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_AA2)] = ParamNodeDescription("AA2");
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_N2O)] = ParamNodeDescription("N2O");
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_O2)] = ParamNodeDescription("O2");

    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_ST)] = ParamNodeDescription("ST");
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_PVCS)] = ParamNodeDescription("PVCs");
}

QByteArray BigFontLayoutModelPrivate::getWaveData(WaveformID waveid)
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

QStringList BigFontLayoutModelPrivate::getLayoutedWaveforms() const
{
    QStringList list;
    QVector<LayoutRow *>::ConstIterator riter;
    for (riter = layoutNodes.constBegin(); riter != layoutNodes.end(); ++riter)
    {
        LayoutRow::ConstIterator iter = (*riter)->constBegin();
        for (; iter != (*riter)->end(); ++iter)
        {
            WaveformID waveId = waveIDMaps.value((*iter)->name, WAVE_NONE);
            if (waveId != WAVE_NONE)
            {
                list.append((*iter)->name);
            }
        }
    }
    return list;
}

QStringList BigFontLayoutModelPrivate::getLayoutedParams() const
{
    QStringList list;
    QVector<LayoutRow *>::ConstIterator riter;
    int row = 0;
    for (riter = layoutNodes.constBegin(); riter != layoutNodes.constEnd(); ++riter)
    {
        int column = 0;
        LayoutRow::ConstIterator iter = (*riter)->constBegin();
        if ((*iter) == NULL)
        {
            continue;
        }
        for (; iter != (*riter)->constEnd(); ++iter)
        {
            WaveformID waveId = waveIDMaps.value((*iter)->name, WAVE_NONE);
            if (waveId == WAVE_NONE && ( (row+1)% LAYOUT_PARAM_ROW_SPACING == 1))
            {
                list.append((*iter)->name);
            }
            column += (*iter)->span;
        }
        row += LAYOUT_PARAM_ROW_SPACING;
    }
    return list;
}

QString BigFontLayoutModelPrivate::findWaveNameFromParam(QString paramStr)
{
    if (paramStr.isEmpty())
    {
        return QString();
    }
    LayoutNodeType namePos = LAYOUT_NODE_NONE;
    for (int i = 0; i < LAYOUT_NODE_NR; i++)
    {
        namePos = static_cast<LayoutNodeType>(i);
        QString str = layoutNodeName(namePos);
        if (str == paramStr)
        {
            break;
        }
    }
    int pos = static_cast<int>(namePos);
    pos += static_cast<int>(LAYOUT_NODE_WAVE_ECG1) -1;
    namePos = static_cast<LayoutNodeType>(pos);
    if (namePos > LAYOUT_NODE_NR)
    {
        return layoutNodeName(LAYOUT_NODE_NONE);
    }
    return layoutNodeName(namePos);
}
