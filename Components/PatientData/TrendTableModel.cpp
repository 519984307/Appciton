/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/23
 **/

#include "TrendTableModel.h"
#include "TimeDate.h"
#include "ThemeManager.h"
#include "ParamInfo.h"
#include "ParamManager.h"
#include "IBPParam.h"
#include "TrendDataStorageManager.h"
#include "DataStorageDefine.h"
#include "TrendDataSymbol.h"
#include "TrendDataDefine.h"
#include "IConfig.h"
#include "RecorderManager.h"
#include "TrendTablePageGenerator.h"
#include <QDebug>
#include "AlarmConfig.h"

#define COLUMN_COUNT        7
#define MAX_ROW_COUNT       9
#define DEFAULT_WIDTH       (825)

#define ROW_HEIGHT_HINT (themeManger.getAcceptableControlHeight())
#define HEADER_HEIGHT_HINT (themeManger.getAcceptableControlHeight())

#define HIGH_PRIO_ALARM_COLOR (QColor(Qt::red))
#define MED_PRIO_ALARM_COLOR (QColor(Qt::yellow))
#define NORMAL_PRIO_ALARM_COLOR (QColor(Qt::white))
#define EVENT_SELECTED_BACKGROUND_COLOR (QColor("#98BFE7"))
class TrendTableModelPrivate
{
public:
    TrendTableModelPrivate();

    void loadTableTitle();
    void getTrendData();
    void updateDisplayTime();
    void dataIndex();
    void loadTrendData();
    void updateEventIndex();
public:
    struct TrendDataContent
    {
        TrendDataContent()
            : dataStr("---"), dataColor(Qt::white)
        {}
        QString dataStr;
        QColor dataColor;
    };
    QList<TrendDataPackage *> trendDataPack;
    QList<int> eventList;                  // 表格数据是否存在事件发生
    int rowCount;
    int startIndex;
    int endIndex;

    typedef QMultiMap<ParamID, SubParamID> TrendParamMap;
    TrendParamMap orderMap;    // 参数列表

    typedef QList<SubParamID> TrendParamList;
    TrendParamList curList;                 // 当前列表中包含的子参数
    TrendParamList displayList;             // 显示行列表
    bool isHistory;                         // 历史回顾标志
    QString historyDataPath;                // 历史数据路径
    ResolutionRatio timeInterval;           // 时间间隔
    unsigned leftTime;                      // 趋势表左侧时间
    unsigned rightTime;                     // 趋势表右侧时间
    unsigned startTime;                     // 打印的开始时间
    unsigned endTime;                       // 打印的结束时间
    int totalCol;                           // 总列数
    int curSecIndex;                        // 当前选中索引
    int maxDataNum;                         // 趋势表总的数据量

    typedef QList<TrendDataContent> TableDataColList;
    QList<TableDataColList> tableDataList;   // 趋势数据
    QList<TrendDataContent> colHeadList;            // 趋势数据时间列表
};

TrendTableModel::TrendTableModel(QObject *parent)
    : QAbstractTableModel(parent), d_ptr(new TrendTableModelPrivate())
{
    QString prefix = "TrendTable|";
    int index = 0;
    QString ratioPrefix = prefix + "ResolutionRatio";
    systemConfig.getNumValue(ratioPrefix, index);
    d_ptr->timeInterval = (ResolutionRatio)index;
    index = 0;
    QString groupPrefix = prefix + "TrendGroup";
    systemConfig.getNumValue(groupPrefix, index);
    loadCurParam(index);
    updateData();
}

TrendTableModel::~TrendTableModel()
{
}

int TrendTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return d_ptr->totalCol;
}

int TrendTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return d_ptr->rowCount;
}

QVariant TrendTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || d_ptr->trendDataPack.count() == 0)
    {
        return QVariant();
    }

    int row = index.row();
    int column = index.column();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        return d_ptr->tableDataList.at(column).at(row).dataStr;
    }
    case Qt::SizeHintRole:
    {
        int w = DEFAULT_WIDTH / (COLUMN_COUNT + 1);
        return QSize(w, HEADER_HEIGHT_HINT * 3 / 2);
    }
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
    case Qt::BackgroundColorRole:
    {
        QColor color = d_ptr->tableDataList.at(column).at(row).dataColor;

        QColor colorHead = d_ptr->colHeadList.at(column).dataColor;
        int curSecIndex = d_ptr->curSecIndex % COLUMN_COUNT;
        if (curSecIndex == column &&
            (colorHead == HIGH_PRIO_ALARM_COLOR ||
             colorHead == MED_PRIO_ALARM_COLOR))
        {
            return QBrush(EVENT_SELECTED_BACKGROUND_COLOR);
        }

        if (color == HIGH_PRIO_ALARM_COLOR ||
            color == MED_PRIO_ALARM_COLOR)
        {
            return color;
        }

        if (row % 2)
        {
            return themeManger.getColor(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                        ThemeManager::StateDisabled);
        }
        else
        {
            return themeManger.getColor(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                        ThemeManager::StateActive);
        }
    }
        break;
    case Qt::ForegroundRole:
    {
        QColor color = d_ptr->tableDataList.at(column).at(row).dataColor;
        QColor colorHead = d_ptr->colHeadList.at(column).dataColor;

        int curSecIndex = d_ptr->curSecIndex % COLUMN_COUNT;
        if (curSecIndex == column &&
            (colorHead == HIGH_PRIO_ALARM_COLOR ||
             colorHead == MED_PRIO_ALARM_COLOR))
        {
            if (color == HIGH_PRIO_ALARM_COLOR ||
                color == MED_PRIO_ALARM_COLOR)
            {
                return color;
            }
        }
       return QBrush(QColor("#2C405A"));
    }
        break;
    default:
        break;
    }
    return QVariant();
}

QVariant TrendTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::SizeHintRole:
    {
        int w = DEFAULT_WIDTH / (COLUMN_COUNT + 1);

        if (orientation == Qt::Vertical)
        {
            return QSize(w, HEADER_HEIGHT_HINT * 3 / 2);
        }
        else
        {
            return QSize(w, HEADER_HEIGHT_HINT * 2 / 3);
        }
        break;
    }
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignLeft);
        break;
    case Qt::DisplayRole:
    {
        if (orientation == Qt::Horizontal)
        {
            return d_ptr->colHeadList.at(section).dataStr;
        }
        else if (orientation == Qt::Vertical)
        {
            SubParamID id = d_ptr->displayList.at(section);
            QString str;
            switch (id)
            {
            case SUB_PARAM_NIBP_SYS:
                str = trs(paramInfo.getParamName(PARAM_NIBP));
                break;
            case SUB_PARAM_ART_SYS:
            case SUB_PARAM_PA_SYS:
            case SUB_PARAM_CVP_MAP:
            case SUB_PARAM_LAP_MAP:
            case SUB_PARAM_RAP_MAP:
            case SUB_PARAM_ICP_MAP:
            case SUB_PARAM_AUXP1_SYS:
            case SUB_PARAM_AUXP2_SYS:
                str = paramInfo.getSubParamName(id);
                str = str.left(str.length() - 4);
                break;
            case SUB_PARAM_T1:
                str = paramInfo.getSubParamName(id);
                str += '/';
                str += paramInfo.getSubParamName(SUB_PARAM_T2);
                break;
            case SUB_PARAM_ETCO2:
            {
                QString name;
                name = paramInfo.getSubParamName(id);
                str = name.right(3);
                str += '(';
                str += trs(name.left(name.length() - 3));
                str += '/';
                name = paramInfo.getSubParamName(SUB_PARAM_FICO2);
                str += trs(name.left(name.length() - 3));
                str += ')';
            }
                break;
            default:
                str = trs(paramInfo.getSubParamName(id));
                break;
            }
            str += '\n';
            str += "(";
            str += Unit::localeSymbol(paramManager.getSubParamUnit(paramInfo.getParamID(id), id));
            str += ")";
            return str;
        }
        break;
    }
    case Qt::BackgroundColorRole:
        if (orientation == Qt::Horizontal)
        {
            QColor color = d_ptr->colHeadList.at(section).dataColor;
            if (color != HIGH_PRIO_ALARM_COLOR &&
                color != MED_PRIO_ALARM_COLOR)
            {
                color = themeManger.getColor(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                             ThemeManager::StateDisabled);
            }
            return color;
        }
        else
        {
            if (section % 2)
            {
                return themeManger.getColor(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                            ThemeManager::StateDisabled);
            }
            else
            {
                return themeManger.getColor(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                            ThemeManager::StateActive);
            }
        }
        break;
    case Qt::ForegroundRole:
        return QBrush(QColor("#2C405A"));
        break;
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags TrendTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QAbstractTableModel::flags(index);
    }

    Qt::ItemFlags flags;
    flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return flags;
}

bool TrendTableModel::eventFilter(QObject *obj, QEvent *ev)
{
    return QObject::eventFilter(obj, ev);
}

int TrendTableModel::getHeaderHeightHint()
{
    return HEADER_HEIGHT_HINT * 2 / 3;
}

int TrendTableModel::getRowHeightHint()
{
    return ROW_HEIGHT_HINT * 3 / 2;
}

void TrendTableModel::setTimeInterval(int t)
{
    d_ptr->timeInterval = (ResolutionRatio)t;
}

void TrendTableModel::loadCurParam(int trendGroup)
{
    d_ptr->curList.clear();

    TrendTableModelPrivate::TrendParamList list;

    if (d_ptr->orderMap.contains(PARAM_DUP_ECG))
    {
        list = d_ptr->orderMap.values(PARAM_DUP_ECG);
        qSort(list);
        d_ptr->curList.append(list);
    }

    if (d_ptr->orderMap.contains(PARAM_SPO2))
    {
        list = d_ptr->orderMap.values(PARAM_SPO2);
        qSort(list);
        d_ptr->curList.append(list);
    }

    if (d_ptr->orderMap.contains(PARAM_NIBP))
    {
        list = d_ptr->orderMap.values(PARAM_NIBP);
        qSort(list);
        d_ptr->curList.append(list);
    }

    if (d_ptr->orderMap.contains(PARAM_TEMP))
    {
        list = d_ptr->orderMap.values(PARAM_TEMP);
        qSort(list);
        d_ptr->curList.append(list);
    }

    if (trendGroup == TREND_GROUP_RESP)
    {
        if (d_ptr->orderMap.contains(PARAM_DUP_RESP))
        {
            list = d_ptr->orderMap.values(PARAM_DUP_RESP);
            qSort(list);
            d_ptr->curList.append(list);
        }

        if (d_ptr->orderMap.contains(PARAM_CO2))
        {
            list = d_ptr->orderMap.values(PARAM_CO2);
            qSort(list);
            d_ptr->curList.append(list);
        }
    }
    else if (trendGroup == TREND_GROUP_IBP)
    {
        if (d_ptr->orderMap.contains(PARAM_IBP))
        {
            list = d_ptr->orderMap.values(PARAM_IBP);
            qSort(list);
            d_ptr->curList.append(list);
        }

        if (d_ptr->orderMap.contains(PARAM_CO))
        {
            list = d_ptr->orderMap.values(PARAM_CO);
            qSort(list);
            d_ptr->curList.append(list);
        }
    }
    else if (trendGroup == TREND_GROUP_AG)
    {
        if (d_ptr->orderMap.contains(PARAM_AG))
        {
            list = d_ptr->orderMap.values(PARAM_AG);
            qSort(list);
            d_ptr->curList.append(list);
        }

        if (d_ptr->orderMap.contains(PARAM_CO2))
        {
            list = d_ptr->orderMap.values(PARAM_CO2);
            qSort(list);
            d_ptr->curList.append(list);
        }
    }
}

void TrendTableModel::setHistoryDataPath(QString path)
{
    d_ptr->historyDataPath = path;
}

void TrendTableModel::setHistoryData(bool flag)
{
    d_ptr->isHistory = flag;
}

void TrendTableModel::updateData()
{
    beginResetModel();
    d_ptr->loadTableTitle();
    d_ptr->getTrendData();
    d_ptr->updateDisplayTime();
    d_ptr->loadTrendData();
    d_ptr->updateEventIndex();
    endResetModel();
}

void TrendTableModel::leftPage(int &curSecCol)
{
    Q_UNUSED(curSecCol)
    if (!d_ptr->trendDataPack.count())
    {
        return;
    }

    unsigned timeInterval = TrendDataSymbol::convertValue(d_ptr->timeInterval);
    // 将趋势表左侧时间往左移动一页的时间
    d_ptr->leftTime = d_ptr->leftTime - timeInterval * COLUMN_COUNT;

    unsigned startTime = d_ptr->trendDataPack.first()->time;
    if (startTime % timeInterval != 0)
    {
        startTime = startTime + (timeInterval - startTime % timeInterval);
    }
    unsigned endTime = d_ptr->trendDataPack.last()->time;
    endTime = endTime - endTime % timeInterval;

    if (d_ptr->leftTime < startTime)
    {
        d_ptr->leftTime = startTime;
    }

    // 更新趋势表右侧时间
    d_ptr->rightTime = d_ptr->leftTime + timeInterval * (COLUMN_COUNT - 1);

    if (d_ptr->rightTime > endTime)
    {
        d_ptr->rightTime = endTime;
    }

    beginResetModel();
    d_ptr->loadTrendData();
    endResetModel();
}

void TrendTableModel::rightPage(int &curSecCol)
{
    Q_UNUSED(curSecCol)
    if (!d_ptr->trendDataPack.count())
    {
        return;
    }

    unsigned timeInterval = TrendDataSymbol::convertValue(d_ptr->timeInterval);
    unsigned lastleftTime = d_ptr->leftTime;
    d_ptr->leftTime = d_ptr->leftTime + timeInterval * COLUMN_COUNT;
    unsigned endTime = d_ptr->trendDataPack.last()->time;
    endTime = endTime - endTime % timeInterval;

    if (d_ptr->leftTime > endTime)
    {
        d_ptr->leftTime = lastleftTime;
    }
    else
    {
        d_ptr->rightTime = d_ptr->leftTime + timeInterval * (COLUMN_COUNT - 1);
        if (d_ptr->rightTime > endTime)
        {
            d_ptr->rightTime = endTime;
        }
    }

    beginResetModel();
    d_ptr->loadTrendData();
    endResetModel();
}

void TrendTableModel::leftMoveEvent(int &curSecCol)
{
    Q_UNUSED(curSecCol)
    for (int i = d_ptr->eventList.count() - 1; i >= 0; i --)
    {
        unsigned timeInterval = TrendDataSymbol::convertValue(d_ptr->timeInterval);
        int eventIndex = d_ptr->eventList.at(i);
        if (eventIndex < d_ptr->curSecIndex)
        {
           unsigned startTime = d_ptr->trendDataPack.first()->time;
            if (startTime % timeInterval != 0)
            {
                startTime = startTime + (timeInterval - startTime % timeInterval);
            }
            // 将趋势表的时间数据移到有事件发生的那页
            d_ptr->leftTime = startTime + eventIndex /
                    COLUMN_COUNT * COLUMN_COUNT * timeInterval;
            d_ptr->rightTime = d_ptr->leftTime + (COLUMN_COUNT - 1) * timeInterval;

            unsigned endTime = d_ptr->trendDataPack.last()->time;
            endTime = endTime - endTime % timeInterval;

            if (d_ptr->rightTime > endTime)
            {
                d_ptr->rightTime = endTime;
            }
            // 更新当前事件选中号，用于给该事件所在列标记颜色
            d_ptr->curSecIndex = eventIndex;

            // 装载数据
            beginResetModel();
            d_ptr->loadTrendData();
            endResetModel();
            return;
        }
    }
}

void TrendTableModel::rightMoveEvent(int &curSecCol)
{
    Q_UNUSED(curSecCol)
    for (int i = 0; i < d_ptr->eventList.count(); i ++)
    {
        unsigned timeInterval = TrendDataSymbol::convertValue(d_ptr->timeInterval);
        int eventIndex = d_ptr->eventList.at(i);
        if (eventIndex > d_ptr->curSecIndex)
        {
            unsigned startTime = d_ptr->trendDataPack.first()->time;
            if (startTime % timeInterval != 0)
            {
                startTime = startTime + (timeInterval - startTime % timeInterval);
            }
            d_ptr->leftTime = startTime + eventIndex /
                    COLUMN_COUNT * COLUMN_COUNT * timeInterval;
            d_ptr->rightTime = d_ptr->leftTime + (COLUMN_COUNT - 1) * timeInterval;

            unsigned endTime = d_ptr->trendDataPack.last()->time;
            endTime = endTime - endTime % timeInterval;

            if (d_ptr->rightTime > endTime)
            {
                d_ptr->rightTime = endTime;
            }

            d_ptr->curSecIndex = eventIndex;

            beginResetModel();
            d_ptr->loadTrendData();
            endResetModel();
            return;
        }
    }
}

bool TrendTableModel::getDataTimeRange(unsigned &start, unsigned &end)
{
    if (d_ptr->trendDataPack.count() != 0)
    {
        start = d_ptr->trendDataPack.first()->time;
        end = d_ptr->trendDataPack.last()->time;
        return true;
    }
    return false;
}

void TrendTableModel::displayDataTimeRange(unsigned &start, unsigned &end)
{
    start = d_ptr->startTime;
    end = d_ptr->endTime;
}

void TrendTableModel::printTrendData(unsigned startTime, unsigned endTime)
{
    int startIndex;
    int endIndex;

    // 二分查找时间索引
    int lowPos = 0;
    int highPos = d_ptr->trendDataPack.count() - 1;
    while (lowPos <= highPos)
    {
        int midPos = (lowPos + highPos) / 2;
        int timeDiff = qAbs(startTime - d_ptr->trendDataPack.at(midPos)->time);

        if (startTime < d_ptr->trendDataPack.at(midPos)->time)
        {
            highPos = midPos - 1;
        }
        else if (startTime > d_ptr->trendDataPack.at(midPos)->time)
        {
            lowPos = midPos + 1;
        }

        if (timeDiff == 0 || lowPos > highPos)
        {
            startIndex = midPos;
            break;
        }
    }

    lowPos = 0;
    highPos = d_ptr->trendDataPack.count() - 1;
    while (lowPos <= highPos)
    {
        int midPos = (lowPos + highPos) / 2;
        int timeDiff = qAbs(endTime - d_ptr->trendDataPack.at(midPos)->time);

        if (endTime < d_ptr->trendDataPack.at(midPos)->time)
        {
            highPos = midPos - 1;
        }
        else if (endTime > d_ptr->trendDataPack.at(midPos)->time)
        {
            lowPos = midPos + 1;
        }

        if (timeDiff == 0 || lowPos > highPos)
        {
            endIndex = midPos;
            break;
        }
    }

    // 打印
    IStorageBackend *backend = trendDataStorageManager.backend();
    if (backend->getBlockNR() <= 0)
    {
        return;
    }
    TrendTablePrintInfo printInfo;
    printInfo.startIndex = startIndex;
    printInfo.stopIndex = endIndex;
    printInfo.interval = TrendDataSymbol::convertValue(d_ptr->timeInterval);
    printInfo.list = d_ptr->displayList;
    for (int i = 0; i < d_ptr->colHeadList.count(); i ++)
    {
        if (d_ptr->colHeadList.at(i).dataColor == Qt::yellow)
        {
            printInfo.eventList.append(true);
        }
        else
        {
            printInfo.eventList.append(false);
        }
    }
    RecordPageGenerator *gen = new TrendTablePageGenerator(backend, printInfo);
    recorderManager.addPageGenerator(gen);
}

unsigned TrendTableModel::getColumnCount() const
{
    return COLUMN_COUNT;
}

unsigned TrendTableModel::getRightTime() const
{
   return d_ptr->rightTime;
}

TrendTableModelPrivate::TrendTableModelPrivate()
    : rowCount(0), startIndex(0), endIndex(0),
      isHistory(false), historyDataPath(""),
      timeInterval(RESOLUTION_RATIO_5_SECOND), leftTime(0),
      rightTime(0), startTime(0), endTime(0), totalCol(0),
      curSecIndex(0), maxDataNum(0)
{
    orderMap.clear();

    QList<ParamID> paramIDList;
    paramManager.getParams(paramIDList);
    qSort(paramIDList);

    for (int i = 0; i < SUB_PARAM_NR; i ++)
    {
        ParamID paramID = paramInfo.getParamID((SubParamID)i);
        if (-1 == paramIDList.indexOf(paramID))
        {
            continue;
        }

        switch (i)
        {
        case SUB_PARAM_ECG_PVCS:
        case SUB_PARAM_ST_I:
        case SUB_PARAM_ST_II:
        case SUB_PARAM_ST_III:
        case SUB_PARAM_ST_aVR:
        case SUB_PARAM_ST_aVL:
        case SUB_PARAM_ST_aVF:
        case SUB_PARAM_ST_V1:
        case SUB_PARAM_ST_V2:
        case SUB_PARAM_ST_V3:
        case SUB_PARAM_ST_V4:
        case SUB_PARAM_ST_V5:
        case SUB_PARAM_ST_V6:
        case SUB_PARAM_NIBP_MAP:
        case SUB_PARAM_NIBP_DIA:
        case SUB_PARAM_ART_DIA:
        case SUB_PARAM_ART_MAP:
        case SUB_PARAM_ART_PR:
        case SUB_PARAM_PA_DIA:
        case SUB_PARAM_PA_MAP:
        case SUB_PARAM_PA_PR:
        case SUB_PARAM_CVP_PR:
        case SUB_PARAM_LAP_PR:
        case SUB_PARAM_RAP_PR:
        case SUB_PARAM_ICP_PR:
        case SUB_PARAM_AUXP1_DIA:
        case SUB_PARAM_AUXP1_MAP:
        case SUB_PARAM_AUXP1_PR:
        case SUB_PARAM_AUXP2_DIA:
        case SUB_PARAM_AUXP2_MAP:
        case SUB_PARAM_AUXP2_PR:
        case SUB_PARAM_FICO2:
        case SUB_PARAM_FIN2O:
        case SUB_PARAM_FIAA1:
        case SUB_PARAM_FIAA2:
        case SUB_PARAM_FIO2:
        case SUB_PARAM_T2:
        case SUB_PARAM_TD:
            break;
        default:
        {
            orderMap.insert(paramID, (SubParamID)i);
            break;
        }
        }
    }
}

void TrendTableModelPrivate::getTrendData()
{
    // 趋势数据
    IStorageBackend *backend;
    if (isHistory)
    {
        backend = StorageManager::open(historyDataPath + TREND_DATA_FILE_NAME, QIODevice::ReadWrite);
    }
    else
    {
        backend = trendDataStorageManager.backend();
    }
    int blockNum = backend->getBlockNR();
    QByteArray data;
    TrendDataSegment *dataSeg;
    qDeleteAll(trendDataPack);
    trendDataPack.clear();
    for (int i = 0; i < blockNum; i ++)
    {
        TrendDataPackage *pack;
        pack = new TrendDataPackage;
        data = backend->getBlockData((quint32)i);
        dataSeg = reinterpret_cast<TrendDataSegment *>(data.data());
        pack->time = dataSeg->timestamp;
        pack->co2Baro = dataSeg->co2Baro;
        for (int j = 0; j < dataSeg->trendValueNum; j ++)
        {
            pack->subparamValue[(SubParamID)dataSeg->values[j].subParamId] = dataSeg->values[j].value;
            pack->subparamAlarm[(SubParamID)dataSeg->values[j].subParamId] = dataSeg->values[j].alarmFlag;
            pack->alarmFlag = dataSeg->eventFlag;
        }
        trendDataPack.append(pack);
    }

    // 动态内存释放
    if (isHistory)
    {
        delete backend;
    }
}

void TrendTableModelPrivate::updateDisplayTime()
{
    if (trendDataPack.count() != 0)
    {
        unsigned interval = TrendDataSymbol::convertValue(timeInterval);
        unsigned startTime = trendDataPack.first()->time;

        if (startTime % interval != 0)
        {
            startTime = startTime + (interval - startTime % interval);
        }
        unsigned lastTime = trendDataPack.last()->time;
        lastTime = lastTime - lastTime % interval;

        leftTime = (lastTime - startTime) * 1.0 / interval / COLUMN_COUNT;
        leftTime = leftTime * interval * COLUMN_COUNT;
        leftTime += startTime;

        rightTime = lastTime;
    }
}

void TrendTableModelPrivate::dataIndex()
{
    // 开始和结尾的索引查找
    startIndex = InvData();
    endIndex = InvData();

    // 二分查找时间索引
    int lowPos = 0;
    int highPos = trendDataPack.count() - 1;
    while (lowPos <= highPos)
    {
        int midPos = (lowPos + highPos) / 2;
        int timeDiff = qAbs(leftTime - trendDataPack.at(midPos)->time);

        if (leftTime < trendDataPack.at(midPos)->time)
        {
            highPos = midPos - 1;
        }
        else if (leftTime > trendDataPack.at(midPos)->time)
        {
            lowPos = midPos + 1;
        }

        if (timeDiff == 0 || lowPos > highPos)
        {
            startIndex = midPos;
            break;
        }
    }

    lowPos = 0;
    highPos = trendDataPack.count() - 1;
    while (lowPos <= highPos)
    {
        int midPos = (lowPos + highPos) / 2;
        int timeDiff = qAbs(rightTime - trendDataPack.at(midPos)->time);

        if (rightTime < trendDataPack.at(midPos)->time)
        {
            highPos = midPos - 1;
        }
        else if (rightTime > trendDataPack.at(midPos)->time)
        {
            lowPos = midPos + 1;
        }

        if (timeDiff == 0 || lowPos > highPos)
        {
            endIndex = midPos;
            break;
        }
    }
}

void TrendTableModelPrivate::loadTrendData()
{
    tableDataList.clear();
    colHeadList.clear();
    QString time;
    int  interval = TrendDataSymbol::convertValue(timeInterval);
    int col = 0;
    QMap<int, AlarmPriority> indexMap;

    dataIndex();
    if (startIndex != InvData() && endIndex != InvData())
    {
        bool isStart = true;
        unsigned lastTime = trendDataPack.at(startIndex)->time;
        if (lastTime % interval != 0)
        {
            lastTime = lastTime + (interval - lastTime % interval);
        }

        TrendDataPackage *pack;
        TrendDataContent colHeadContent;
        for (int i = startIndex; i <= endIndex; i ++)
        {
            pack = trendDataPack.at(i);
            unsigned t = pack->time;

            // 判断每个固定的时间间隔点是否有报警事件发生
            // 并获取报警事件的优先级
            if (pack->alarmFlag)
            {
                AlarmPriority alarmPriority = ALARM_PRIO_PROMPT;
                int count = displayList.count();
                for (int i = count - 1; i >= 0; i--)
                {
                    bool isAlarm = pack->subparamAlarm.value(displayList.at(i), false);
                    alarmPriority = alarmConfig.getLimitAlarmPriority(displayList.at(i));
                    if (isAlarm)
                    {
                        break;
                    }
                }
                indexMap.insert(col, alarmPriority);
            }

            if (t != lastTime)
            {
                continue;
            }

            timeDate.getTime(t, time, true);
            colHeadContent.dataStr = time;
            colHeadList.append(colHeadContent);

            // 取出一屏的开始到结束时间
            endTime = t;
            if (isStart)
            {
                startTime = t;
                isStart = false;
            }

            TableDataColList colList;
            TrendDataContent dataContent;
            for (int j = 0; j < rowCount; j ++)
            {

                if (displayList.at(j) == SUB_PARAM_NIBP_SYS)
                {
                    qint16 nibpSys = pack->subparamValue.value((SubParamID)(displayList.at(j) - 2), InvData());
                    qint16 nibpDia = pack->subparamValue.value((SubParamID)(displayList.at(j) - 1), InvData());
                    qint16 nibpMap = pack->subparamValue.value(displayList.at(j), InvData());
                    QString sysStr = nibpSys == InvData() ? "---" : QString::number(nibpSys);
                    QString diaStr = nibpDia == InvData() ? "---" : QString::number(nibpDia);
                    QString mapStr = nibpMap == InvData() ? "---" : QString::number(nibpMap);
                    dataContent.dataStr = sysStr + "/" + diaStr + "\n(" + mapStr + ")";
                }
                else if (displayList.at(j) == SUB_PARAM_ART_SYS || displayList.at(j) == SUB_PARAM_PA_SYS ||
                         displayList.at(j) == SUB_PARAM_AUXP1_SYS || displayList.at(j) == SUB_PARAM_AUXP2_SYS)
                {
                    qint16 ibpSys = pack->subparamValue.value((SubParamID)(displayList.at(j) - 2), InvData());
                    qint16 ibpDia = pack->subparamValue.value((SubParamID)(displayList.at(j) - 1), InvData());
                    qint16 ibpMap = pack->subparamValue.value(displayList.at(j), InvData());
                    QString sysStr = ibpSys == InvData() ? "---" : QString::number(ibpSys);
                    QString diaStr = ibpDia == InvData() ? "---" : QString::number(ibpDia);
                    QString mapStr = ibpMap == InvData() ? "---" : QString::number(ibpMap);
                    dataContent.dataStr = sysStr + "/" + diaStr + "\n(" + mapStr + ")";
                }
                else if (displayList.at(j) == SUB_PARAM_ETCO2 || displayList.at(j) == SUB_PARAM_ETN2O ||
                         displayList.at(j) == SUB_PARAM_ETAA1 || displayList.at(j) == SUB_PARAM_ETAA2 ||
                         displayList.at(j) == SUB_PARAM_ETO2 || displayList.at(j) == SUB_PARAM_T1)
                {
                    qint16 data1 = pack->subparamValue.value(displayList.at(j), InvData());
                    qint16 data2 = pack->subparamValue.value((SubParamID)(displayList.at(j) + 1), InvData());

                    QString dataStr1;
                    QString dataStr2;

                    if (displayList.at(j) == SUB_PARAM_T1
                            || displayList.at(j) == SUB_PARAM_ETCO2)
                    {
                        double dubData1 = data1 * 1.0 / 10;
                        double dubData2 = data2 * 1.0 / 10;
                        dataStr1 = data1 == InvData() ? "---" : QString::number(dubData1, 'f', 1);
                        dataStr2 = data2 == InvData() ? "---" : QString::number(dubData2, 'f', 1);
                    }
                    else
                    {
                        dataStr1 = data1 == InvData() ? "---" : QString::number(data1);
                        dataStr2 = data2 == InvData() ? "---" : QString::number(data2);
                    }
                    dataContent.dataStr = dataStr1 + "/" + dataStr2;
                }
                else
                {
                    qint16 data = pack->subparamValue.value(displayList.at(j), InvData());
                    dataContent.dataStr = data == InvData() ? "---" : QString::number(data);
                }

                bool isAlarm = pack->subparamAlarm.value(displayList.at(j), false);
                AlarmPriority alarmPriority = alarmConfig.getLimitAlarmPriority(displayList.at(j));
                QColor color;

                if (isAlarm)
                {
                    if (alarmPriority == ALARM_PRIO_HIGH)
                    {
                        color = HIGH_PRIO_ALARM_COLOR;
                    }
                    else
                    {
                        color = MED_PRIO_ALARM_COLOR;
                    }
                }
                else
                {
                    color = NORMAL_PRIO_ALARM_COLOR;
                }
                dataContent.dataColor = color;
                colList.append(dataContent);
            }
            tableDataList.append(colList);
            col++;
            lastTime = lastTime + interval;
        }
    }

    totalCol = col;

    // 当数据不够一屏时,后面补空
//    for (; col < COLUMN_COUNT; col ++)
//    {
//        TrendDataContent defaultTimeContent;
//        defaultTimeContent.dataStr = "";
//        colHeadList.append(defaultTimeContent);
//        for (int j = 0; j < rowCount; j ++)
//        {
//            TrendDataContent defalutContent;
//            tableDataList[col][j] = defalutContent;
//        }
//    }

    // 趋势表头添加背景色
    for (QMap<int, AlarmPriority>::const_iterator it = indexMap.begin();
         it != indexMap.end(); ++it)
    {
        int col = it.key();
        if (colHeadList.count() < col + 1)
        {
            continue;
        }
        AlarmPriority alarmPrio = it.value();
        QColor color;
        switch (alarmPrio)
        {
            case ALARM_PRIO_HIGH:
            color = HIGH_PRIO_ALARM_COLOR;
            break;
            case ALARM_PRIO_MED:
            case ALARM_PRIO_LOW:
            color = MED_PRIO_ALARM_COLOR;
            break;
            case ALARM_PRIO_PROMPT:
            color = NORMAL_PRIO_ALARM_COLOR;
            break;
        }

        if (colHeadList.at(col).dataStr != "")
        {
            colHeadList[col].dataColor = color;
        }
    }
}

void TrendTableModelPrivate::updateEventIndex()
{
    if (trendDataPack.count() != 0)
    {
        eventList.clear();
        int  interval = TrendDataSymbol::convertValue(timeInterval);
        unsigned lastTime = trendDataPack.first()->time;
        if (lastTime % interval != 0)
        {
            lastTime = lastTime + (interval - lastTime % interval);
        }

        TrendDataPackage *pack;
        bool isEvent = false;
        int index = 0;
        for (int i = 0; i < trendDataPack.count(); i ++)
        {
            pack = trendDataPack.at(i);
            unsigned t = pack->time;

            // 判断是否有事件发生
            if (pack->alarmFlag)
            {
                isEvent = true;
            }

            // 是否满足时间间隔的时间
            if (t != lastTime)
            {
                continue;
            }

            // 该时间间隔内是否发生事件
            if (isEvent)
            {
                eventList.append(index);
                isEvent = false;
            }

            index++;
            lastTime = lastTime + interval;
        }
        maxDataNum = index;
        curSecIndex = index - 1;
    }
}

void TrendTableModelPrivate::loadTableTitle()
{
    displayList.clear();
    rowCount = 0;
    for (int i = 0; i < curList.length(); i ++)
    {
        SubParamID id = curList.at(i);
        switch (id)
        {
        case SUB_PARAM_ART_SYS:
        case SUB_PARAM_PA_SYS:
        case SUB_PARAM_CVP_MAP:
        case SUB_PARAM_LAP_MAP:
        case SUB_PARAM_RAP_MAP:
        case SUB_PARAM_ICP_MAP:
        case SUB_PARAM_AUXP1_SYS:
        case SUB_PARAM_AUXP2_SYS:
        {
            SubParamID ibp1 = ibpParam.getSubParamID(ibpParam.getEntitle(IBP_INPUT_1));
            SubParamID ibp2 = ibpParam.getSubParamID(ibpParam.getEntitle(IBP_INPUT_2));
            if (id != ibp1 && id != ibp2)
            {
                continue;
            }
            break;
        }
        default:
            break;
        }
        rowCount++;
        displayList.append(id);
    }
}
