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
#include "Framework/Utility/OrderedMap.h"
#include "ParamInfo.h"
#include <QStringList>
#include "IConfig.h"
#include "DemoProvider.h"
#include "ParamManager.h"
#include "IBPParam.h"
#include "Framework/Language/LanguageManager.h"
#include "SystemManager.h"
#include "ColorManager.h"

struct ParamNodeDescription
{
    ParamNodeDescription() : waveNode(LAYOUT_NODE_NONE), paramID(PARAM_NONE) {}
    explicit ParamNodeDescription(const QString &displayName, LayoutNodeType waveNode,
                                  ParamID paramID)
        : displayName(displayName), waveNode(waveNode), paramID(paramID){}
    QString displayName;
    LayoutNodeType waveNode;
    ParamID paramID;
};

#define NODE_DESC(dispName, waveNode, paramID) ParamNodeDescription(dispName, waveNode, paramID)

typedef QList<LayoutNode *> LayoutRow;

class BigFontLayoutModelPrivate
{
public:
    BigFontLayoutModelPrivate()
        : demoProvider(NULL),
          isChangeData(false)
    {
        demoProvider = qobject_cast<DemoProvider *>(paramManager.getProvider("DemoProvider"));
    }
    ~BigFontLayoutModelPrivate()
    {
        clearAllLayoutNodes();
    }

    LayoutNode * findNode(const QModelIndex &index);
    void fillWaveData(ScreenLayoutItemInfo *info);

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

    DemoProvider *demoProvider;
    OrderedMap<QString , WaveformID> waveIDMaps;
    OrderedMap<QString, ParamNodeDescription> paramNodeDescriptions;     // store param node's description
    QVector<LayoutRow *> layoutNodes;
    QMap<WaveformID, QByteArray> waveCaches;

    bool isChangeData;
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
    return LAYOUT_COLUMN_COUNT;
}

int BigFontLayoutModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
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

            // find the node's wave name, there's 1 row between param and wave, need to skip
            QModelIndex waveIndex = this->index(index.row() + 2
                                                , index.column());

            LayoutNode *waveNode = d_ptr->findNode(waveIndex);
            if (waveNode && waveNode->span > 1)
            {
                waveNode->name = layoutNodeName(d_ptr->paramNodeDescriptions[nodeName].waveNode);
                anotherIndex = waveIndex;
            }
            emit dataChanged(index, anotherIndex);
            d_ptr->isChangeData = true;
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
            QModelIndex waveIndex = this->index(index.row() + 2
                                                , index.column());
            LayoutNode *waveNode = d_ptr->findNode(waveIndex);
            if (waveNode && waveNode->span > 1)
            {
                anotherIndex = waveIndex;
                waveNode->name = QString();
            }
            emit dataChanged(index, anotherIndex);
            d_ptr->isChangeData = true;
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
                info.displayName = trs(paramInfo.getParamWaveformName(info.waveid));
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
                    info.displayName = trs(info.name);
                }
            }
            d_ptr->fillWaveData(&info);
            return qVariantFromValue(info);
        }
        break;
    }
    case Qt::ForegroundRole:
    {
        LayoutNode *node = d_ptr->findNode(index);
        if (node)
        {
            ParamID paramID = PARAM_NONE;
            WaveformID waveID = d_ptr->waveIDMaps.value(node->name, WAVE_NONE);
            if (waveID != WAVE_NONE)
            {
                paramID = paramInfo.getParamID(waveID);
            }
            else
            {
                ParamNodeDescription desc = d_ptr->paramNodeDescriptions.value(node->name);
                paramID = desc.paramID;
            }
            QColor color = colorManager.getColor(paramInfo.getParamName(paramID));
            return QBrush(color);
        }
    }
        break;
    case Qt::BackgroundRole:
    {
        return QBrush(Qt::black);
    }
        break;
    case ReplaceRole:
    {
        LayoutNode *node = d_ptr->findNode(index);
        if (node == NULL || node->name.isEmpty())
        {
            break;
        }

        if (node->span == 1)    // param item
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
        if (node == NULL || !node->name.isEmpty())
        {
            break;
        }

        if (node->span == 1) {      // param item
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
    case BorderRole:
    {
        int border = 0;
        LayoutNode *node = d_ptr->findNode(index);
        if (index.row() == 0)
        {
            border |= BORDER_TOP;
        }

        if (index.row() == rowCount() - 1)
        {
            border |= BORDER_BOTTOM;
        }

        if (index.column() == 0 || index.column() == columnCount() / 2)
        {
            border |= BORDER_LEFT;
        }

        if (index.column() == columnCount() - 1)
        {
            border |= BORDER_RIGHT;
        }

        if (node)
        {
            border |= BORDER_LEFT | BORDER_TOP | BORDER_RIGHT | BORDER_BOTTOM;
            QModelIndex idx = this->index(index.row(),  node->span + index.column());
            if (index.isValid() && d_ptr->findNode(idx) != NULL)
            {
                border &= ~BORDER_RIGHT;
            }

            if (index.row() == rowCount() / 2)
            {
                border &= ~BORDER_TOP;
            }
        }

        return border;
    }
    break;
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
            if (node->span > 1)
            {
                // wave item's span is always larger than 1
                return QSize(node->span, 2);
            }
            else
            {
                return QSize(node->span, 1);
            }
        }
    }

    return QAbstractTableModel::span(index);
}

void BigFontLayoutModel::saveLayoutInfo()
{
    systemConfig.setConfig("PrimaryCfg|UILayout|ContentLayout|BigFont", d_ptr->getLayoutMap());
    d_ptr->isChangeData = false;
}

void BigFontLayoutModel::loadLayoutInfo(bool isDefaultConfig)
{
    QVariantMap config;
    if (isDefaultConfig)
    {
        Config defalutConfig(ORGINAL_SYSTEM_CFG_FILE);
        config = defalutConfig.getConfig("PrimaryCfg|UILayout|ContentLayout|BigFont");
    }
    else
    {
        config = systemConfig.getConfig("PrimaryCfg|UILayout|ContentLayout|BigFont");
    }

    QVariantMap oldMap = d_ptr->getLayoutMap();
    beginResetModel();
    d_ptr->loadLayoutFromConfig(config);
    endResetModel();
    QVariantMap newMap = d_ptr->getLayoutMap();

    if (oldMap != newMap && isDefaultConfig)
    {
        d_ptr->isChangeData = true;
    }

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

bool BigFontLayoutModel::isChangeLayoutInfo()
{
    return d_ptr->isChangeData;
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

void BigFontLayoutModelPrivate::fillWaveData(ScreenLayoutItemInfo *info)
{
    if (info->waveid == WAVE_NONE || !demoProvider)
    {
        return;
    }

    info->waveContent = getWaveData(info->waveid);
    if (info->waveid >= WAVE_ECG_I && info->waveid <= WAVE_ECG_V6)
    {
        info->baseLine = demoProvider->getBaseLine();
        info->waveMaxValue = 255;
        info->waveMinValue = 0;
        info->sampleRate = demoProvider->getWaveformSample();
    }
    else if (info->waveid == WAVE_RESP)
    {
        info->waveMaxValue = demoProvider->maxRESPWaveValue();
        info->waveMinValue = demoProvider->minRESPWaveValue();
        info->baseLine = demoProvider->getRESPBaseLine();
        info->sampleRate = demoProvider->getRESPWaveformSample();
    }
    else if (info->waveid == WAVE_SPO2)
    {
        info->waveMaxValue = demoProvider->getSPO2MaxValue();
        info->waveMinValue = 0;
        info->baseLine = demoProvider->getSPO2BaseLine();
        info->sampleRate = demoProvider->getSPO2WaveformSample();
    }
    else if (info->waveid == WAVE_CO2)
    {
        /* divided by 2 to make the wave higher */
        info->waveMaxValue = demoProvider->getCO2MaxWaveform() / 2;
        info->drawSpeed = 6.25;
        info->waveMinValue = 0;
        info->baseLine = demoProvider->getCO2BaseLine();
        info->sampleRate = demoProvider->getCO2WaveformSample();
    }
    else if (info->waveid >= WAVE_N2O && info->waveid <= WAVE_O2)
    {
        info->waveMaxValue = demoProvider->getN2OMaxWaveform();
        info->waveMinValue = 0;
        info->baseLine = demoProvider->getN2OBaseLine();
        info->sampleRate = demoProvider->getN2OWaveformSample();
    }
    else if (info->waveid >= WAVE_ART && info->waveid <= WAVE_AUXP2)
    {
        info->waveMaxValue = demoProvider->getIBPMaxWaveform();
        info->waveMinValue = 0;
        info->baseLine = demoProvider->getIBPBaseLine();
        info->sampleRate = demoProvider->getIBPWaveformSample();
    }
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

    QVector<LayoutRow *>::ConstIterator riter = layoutNodes.constBegin();

    QVariantList paramList;
    QVariantList waveList;

    int validRow = 0;
    for (; riter != layoutNodes.constEnd(); ++riter)
    {
        if ((*riter)->isEmpty())
        {
            continue;
        }


        LayoutRow::ConstIterator iter = (*riter)->constBegin();
        for (; iter != (*riter)->constEnd(); ++iter)
        {
            LayoutNode *n = *iter;

            QVariantMap nm;
            nm["@span"] = n->span;
            nm["@editable"] = n->editable ? 1 : 0;
            nm["@pos"] = n->pos;
            nm["@text"] = n->name;

            if (validRow % 2 == 0)
            {
                // param row
                paramList.append(nm);
            }
            else
            {
                // wave row
                waveList.append(nm);
            }
        }
        validRow += 1;
    }

    Q_ASSERT(paramList.length() == waveList.length());
    Q_ASSERT(paramList.length() % 2 == 0);

    for (int i = 0; i< paramList.length(); i += 2)
    {
        QVariantMap node1;
        QVariantList nodeList;
        node1["Param"] = paramList.at(i);
        node1["Wave"] = waveList.at(i);
        nodeList.append(node1);
        QVariantMap node2;
        node2["Param"] = paramList.at(i + 1);
        node2["Wave"] = waveList.at(i + 1);
        nodeList.append(node2);
        QVariantMap row;
        row["LayoutNode"] = nodeList;
        layoutRows.append(row);
    }

    QVariantMap m;
    m["LayoutRow"] = layoutRows;
    return m;
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
    isChangeData = false;
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

        QVariantList::ConstIterator nodeIter = nodes.constBegin();
        for (; nodeIter != nodes.constEnd(); ++nodeIter)
        {
            QVariantMap nodeMap = nodeIter->toMap();
            QVariantMap paramMap = nodeMap["Param"].toMap();
            QVariantMap waveMap = nodeMap["Wave"].toMap();

            LayoutNode *paramNode = new LayoutNode();
            if (!paramMap.isEmpty())
            {
                paramNode->pos = paramMap["@pos"].toInt();
                paramNode->span = paramMap["@span"].toInt();
                paramNode->editable = paramMap["@editable"].toInt();
                paramNode->name = paramMap["@text"].toString();
#ifdef BIG_FONT_LAYOUT_CO2_REPLACE_RESP
                if (systemManager.isSupport(PARAM_CO2) && paramManager.getParam(PARAM_CO2)->isConnected())
                {
                    // co2 replace resp when co2 connecting
                    if (paramNode->name == QString(layoutNodeName(LAYOUT_NODE_PARAM_RESP)))
                    {
                        paramNode->name = QString(layoutNodeName(LAYOUT_NODE_PARAM_CO2));
                    }
                }
                else
                {
                    if (paramNode->name == QString(layoutNodeName(LAYOUT_NODE_PARAM_CO2)))
                    {
                        paramNode->name = QString(layoutNodeName(LAYOUT_NODE_PARAM_RESP));
                    }
                }
#endif

                if (paramRow->isEmpty())
                {
                    paramRow->append(paramNode);
                }
                else
                {
                    // the layout should be sorted by the pos
                    LayoutRow::Iterator iter = paramRow->begin();
                    for (; iter != paramRow->end(); ++iter)
                    {
                        if ((*iter)->pos > paramNode->pos)
                        {
                            break;
                        }
                    }
                    paramRow->insert(iter, paramNode);
                }
            }

            LayoutNode *waveNode = new LayoutNode();
            if (!waveMap.isEmpty())
            {
                waveNode->pos = waveMap["@pos"].toInt();
                waveNode->span = waveMap["@span"].toInt();
                waveNode->editable = waveMap["@editable"].toInt();
                waveNode->name = waveMap["@text"].toString();

#ifdef BIG_FONT_LAYOUT_CO2_REPLACE_RESP
                if (systemManager.isSupport(PARAM_CO2) && paramManager.getParam(PARAM_CO2)->isConnected())
                {
                    // co2 replace resp when co2 connecting
                    if (waveNode->name == QString(layoutNodeName(LAYOUT_NODE_WAVE_RESP)))
                    {
                        waveNode->name = QString(layoutNodeName(LAYOUT_NODE_WAVE_CO2));
                    }
                }
                else
                {
                    if (waveNode->name == QString(layoutNodeName(LAYOUT_NODE_WAVE_CO2)))
                    {
                        waveNode->name = QString(layoutNodeName(LAYOUT_NODE_WAVE_RESP));
                    }
                }
#endif

                if (waveRow->isEmpty())
                {
                    waveRow->append(waveNode);
                }
                else
                {
                    // the layout should be sorted by the pos
                    LayoutRow::Iterator iter = waveRow->begin();
                    for (; iter != waveRow->end(); ++iter)
                    {
                        if ((*iter)->pos > waveNode->pos)
                        {
                            break;
                        }
                    }
                    waveRow->insert(iter, waveNode);
                }
            }
        }

        layoutNodes.append(paramRow);
        layoutNodes.append(new LayoutRow());
        layoutNodes.append(waveRow);
        layoutNodes.append(new LayoutRow());
    }
}

void BigFontLayoutModelPrivate::loadItemInfos()
{
    // two ecg wave at most
    /* TODO: find the proper ECG Wave */
    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_ECG1), WAVE_ECG_II);
    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_ECG2), WAVE_ECG_I);

    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_RESP), WAVE_RESP);
    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_SPO2), WAVE_SPO2);
    waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_CO2), WAVE_CO2);

    // find proper IBP Wave base of the wave name
    QString nodeName;
    if (systemManager.isSupport(CONFIG_IBP))
    {
        nodeName = layoutNodeName(LAYOUT_NODE_WAVE_IBP1);
        waveIDMaps.insert(nodeName, ibpParam.getWaveformID(ibpParam.getEntitle(IBP_CHN_1)));
        nodeName = layoutNodeName(LAYOUT_NODE_WAVE_IBP2);
        waveIDMaps.insert(nodeName, ibpParam.getWaveformID(ibpParam.getEntitle(IBP_CHN_2)));

        nodeName = layoutNodeName(LAYOUT_NODE_PARAM_IBP1);
        // IBP's pressure name is identical to it's wave name
        QString waveformName;
        waveformName = paramInfo.getParamWaveformName(waveIDMaps[layoutNodeName(LAYOUT_NODE_WAVE_IBP1)]);
        paramNodeDescriptions[nodeName] = NODE_DESC(waveformName, LAYOUT_NODE_WAVE_IBP1, PARAM_IBP);

        nodeName = layoutNodeName(LAYOUT_NODE_PARAM_IBP2);
        waveformName = paramInfo.getParamWaveformName(waveIDMaps[layoutNodeName(LAYOUT_NODE_WAVE_IBP2)]);
        paramNodeDescriptions[nodeName] = NODE_DESC(waveformName, LAYOUT_NODE_WAVE_IBP2, PARAM_IBP);
    }
    if (systemManager.isSupport((CONFIG_AG)))
    {
        waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_N2O), WAVE_N2O);
        waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_AA1), WAVE_AA1);
        waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_AA2), WAVE_AA2);
        waveIDMaps.insert(layoutNodeName(LAYOUT_NODE_WAVE_O2), WAVE_O2);

        nodeName = layoutNodeName(LAYOUT_NODE_PARAM_AA1);
        paramNodeDescriptions[nodeName] = NODE_DESC("AA1", LAYOUT_NODE_WAVE_AA1, PARAM_AG);
        nodeName = layoutNodeName(LAYOUT_NODE_PARAM_AA2);
        paramNodeDescriptions[nodeName] = NODE_DESC("AA2", LAYOUT_NODE_WAVE_AA2, PARAM_AG);
        nodeName = layoutNodeName(LAYOUT_NODE_PARAM_N2O);
        paramNodeDescriptions[nodeName] = NODE_DESC("N2O", LAYOUT_NODE_WAVE_N2O, PARAM_AG);
        nodeName = layoutNodeName(LAYOUT_NODE_PARAM_O2);
        paramNodeDescriptions[nodeName] = NODE_DESC("AG_O2", LAYOUT_NODE_WAVE_O2, PARAM_AG);
    }

    nodeName = layoutNodeName(LAYOUT_NODE_PARAM_ECG);
    paramNodeDescriptions[nodeName] = NODE_DESC(paramInfo.getParamName(PARAM_ECG), LAYOUT_NODE_WAVE_ECG1, PARAM_ECG);
    nodeName = layoutNodeName(LAYOUT_NODE_PARAM_SPO2);
    paramNodeDescriptions[nodeName] = NODE_DESC(paramInfo.getParamName(PARAM_SPO2), LAYOUT_NODE_WAVE_SPO2, PARAM_SPO2);
    nodeName = layoutNodeName(LAYOUT_NODE_PARAM_RESP);
    paramNodeDescriptions[nodeName] = NODE_DESC(paramInfo.getParamName(PARAM_RESP), LAYOUT_NODE_WAVE_RESP, PARAM_RESP);

    nodeName = layoutNodeName(LAYOUT_NODE_PARAM_CO2);
    paramNodeDescriptions[nodeName] = NODE_DESC(paramInfo.getParamName(PARAM_CO2), LAYOUT_NODE_WAVE_CO2, PARAM_CO2);
    nodeName = layoutNodeName(LAYOUT_NODE_PARAM_NIBP);
    paramNodeDescriptions[nodeName] = NODE_DESC(paramInfo.getParamName(PARAM_NIBP), LAYOUT_NODE_NONE, PARAM_NIBP);
    nodeName = layoutNodeName(LAYOUT_NODE_PARAM_NIBPLIST);
    paramNodeDescriptions[nodeName] = NODE_DESC(trs("NIBPList"), LAYOUT_NODE_NONE, PARAM_NIBP);
    nodeName = layoutNodeName(LAYOUT_NODE_PARAM_TEMP);
    paramNodeDescriptions[nodeName] = NODE_DESC(paramInfo.getParamName(PARAM_TEMP), LAYOUT_NODE_NONE, PARAM_TEMP);

#ifndef HIDE_ECG_ST_PVCS_SUBPARAM
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_ST)] = NODE_DESC("ST", LAYOUT_NODE_NONE, PARAM_ECG);
    paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_PVCS)] = NODE_DESC("PVCs", LAYOUT_NODE_NONE, PARAM_ECG);
#endif
    if (systemManager.isSupport(CONFIG_O2))
    {
        paramNodeDescriptions[layoutNodeName(LAYOUT_NODE_PARAM_OXYGEN)] = NODE_DESC("O2", LAYOUT_NODE_NONE, PARAM_O2);
    }
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
            if (waveIDMaps.contains((*iter)->name))
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
    for (riter = layoutNodes.constBegin(); riter != layoutNodes.constEnd(); ++riter)
    {
        LayoutRow::ConstIterator iter = (*riter)->constBegin();
        for (; iter != (*riter)->constEnd(); ++iter)
        {
            if ((!waveIDMaps.contains((*iter)->name)) && (!(*iter)->name.isEmpty()))
            {
                list.append((*iter)->name);
            }
        }
    }
    return list;
}
