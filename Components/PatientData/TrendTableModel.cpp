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
#include "CO2Param.h"
#include <QTimerEvent>

#define COLUMN_COUNT        7
#define MAX_ROW_COUNT       9
#define DEFAULT_WIDTH       (680)
#define STOP_PRINT_TIMEOUT  (100)

#define ROW_HEIGHT_HINT (themeManger.getAcceptableControlHeight())
#define HEADER_HEIGHT_HINT (themeManger.getAcceptableControlHeight())

#define HIGH_PRIO_ALARM_COLOR (QColor(Qt::red))
#define MED_PRIO_ALARM_COLOR (QColor(Qt::yellow))
#define MANUAL_EVENT_COLOR (QColor(Qt::green))
#define NORMAL_PRIO_ALARM_COLOR (QColor(Qt::white))
#define EVENT_SELECTED_BACKGROUND_COLOR (QColor("#98BFE7"))

struct TrendTableModelIndexInfo
{
    explicit TrendTableModelIndexInfo(int s = 0, int e = 0, int c = -1, int t = 0)
        : start(s)
        , end(e)
        , event(c)
        , total(t)
    {}

    int start;
    int end;
    int event;
    int total;
};

class TrendTableModelPrivate
{
public:
    TrendTableModelPrivate();

    void loadTableTitle();
    void getTrendData();
    void updateIndexInfo();
    void loadTrendData();

    /**
     * @brief getParamName  获取参数名称
     * @param section  参数索引
     * @return  参数名称
     */
    QString getParamName(int section);

    /**
     * @brief addModuleCheck  加入模块支持检查
     * @param trendGroup 趋势组
     * @return 调整过的趋势组
     */
    int addModuleCheck(int trendGroup);

public:
    struct TrendDataContent
    {
        TrendDataContent(QString str = "---",
                         QColor dColor = QColor("#2C405A"),
                         QColor bColor = themeManger.getColor(ThemeManager::ControlTypeNR,
                                         ThemeManager::ElementBackgound,
                                         ThemeManager::StateDisabled)
                        )
            : dataStr(str)
            , dataColor(dColor)
            , backGroundColor(bColor)
        {
        }
        QString dataStr;
        QColor dataColor;
        QColor backGroundColor;
    };
    QList<TrendDataPackage *> trendDataPack;
    QList<int> eventList;                  // 表格数据是否存在事件发生
    int rowCount;

    typedef QMultiMap<ParamID, SubParamID> TrendParamMap;
    TrendParamMap orderMap;    // 参数列表

    typedef QList<SubParamID> TrendParamList;
    TrendParamList curList;                 // 当前列表中包含的子参数
    TrendParamList displayList;             // 显示行列表
    bool isHistory;                         // 历史回顾标志
    QString historyDataPath;                // 历史数据路径
    ResolutionRatio timeInterval;           // 时间间隔

    typedef QList<TrendDataContent> TableDataColList;
    QList<TableDataColList> tableDataList;   // 趋势数据
    QList<TrendDataContent> colHeadList;            // 趋势数据时间列表

    TrendTableModelIndexInfo indexInfo;

    int printTimerId;
    int waitTimerId;
    bool isWait;
    int timeoutNum;
    RecordPageGenerator *generator;
};

TrendTableModel::TrendTableModel(QObject *parent)
    : QAbstractTableModel(parent), d_ptr(new TrendTableModelPrivate())
{
    QString prefix = "TrendTable|";
    int index = 0;
    QString ratioPrefix = prefix + "ResolutionRatio";
    systemConfig.getNumValue(ratioPrefix, index);
    d_ptr->timeInterval = static_cast<ResolutionRatio>(index);
    index = 0;
    QString groupPrefix = prefix + "TrendGroup";
    systemConfig.getNumValue(groupPrefix, index);
    loadCurParam(d_ptr->addModuleCheck(index));
    updateData();
}

TrendTableModel::~TrendTableModel()
{
}

int TrendTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return COLUMN_COUNT;
}

int TrendTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return d_ptr->rowCount;
}

QVariant TrendTableModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() == false || d_ptr->indexInfo.total == 0)
    {
        return QVariant();
    }

    int row = index.row();
    int column = index.column();

    if (d_ptr->tableDataList.count() < column + 1)
    {
        return QVariant();
    }
    if (d_ptr->tableDataList.at(0).count() < row + 1)
    {
        return QVariant();
    }

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
        QColor color = d_ptr->tableDataList.at(column).at(row).backGroundColor;

        QColor colorHead = d_ptr->colHeadList.at(column).backGroundColor;
        int curSecIndex = d_ptr->indexInfo.event % COLUMN_COUNT;
        if (curSecIndex == column &&
                (colorHead != themeManger.getColor(ThemeManager::ControlTypeNR,
                                        ThemeManager::ElementBackgound,
                                        ThemeManager::StateDisabled)))
        {
            return QBrush(EVENT_SELECTED_BACKGROUND_COLOR);
        }

        if (color == HIGH_PRIO_ALARM_COLOR || color == MED_PRIO_ALARM_COLOR)
        {
            return color;
        }

        if (row % 2)
        {
            return themeManger.getColor(ThemeManager::ControlTypeNR,
                                        ThemeManager::ElementBackgound,
                                        ThemeManager::StateDisabled);
        }
        return themeManger.getColor(ThemeManager::ControlTypeNR,
                                    ThemeManager::ElementBackgound,
                                    ThemeManager::StateActive);
    }
    break;
    case Qt::ForegroundRole:
    {
        QColor color = d_ptr->tableDataList.at(column).at(row).backGroundColor;
        QColor colorHead = d_ptr->colHeadList.at(column).backGroundColor;

        int curSecIndex = d_ptr->indexInfo.event % COLUMN_COUNT;
        if (curSecIndex == column && (colorHead == HIGH_PRIO_ALARM_COLOR || colorHead == MED_PRIO_ALARM_COLOR))
        {
            if (color == HIGH_PRIO_ALARM_COLOR || color == MED_PRIO_ALARM_COLOR)
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
        return QSize(w, HEADER_HEIGHT_HINT * 2 / 3);
    }
    break;
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignLeft);
        break;
    case Qt::DisplayRole:
    {
        if (orientation == Qt::Horizontal)
        {
            if (d_ptr->colHeadList.count() < section + 1)
            {
                return QVariant();
            }
            return d_ptr->colHeadList.at(section).dataStr;
        }
        else if (orientation == Qt::Vertical)
        {
            return d_ptr->getParamName(section);
        }
        break;
    }
    case Qt::BackgroundColorRole:
        if (orientation == Qt::Horizontal)
        {
            if (d_ptr->colHeadList.count() < section + 1)
            {
                return QVariant();
            }
            return d_ptr->colHeadList.at(section).backGroundColor;
        }
        else
        {
            if (section % 2)
            {
                return themeManger.getColor(ThemeManager::ControlTypeNR,
                                            ThemeManager::ElementBackgound,
                                            ThemeManager::StateDisabled);
            }
            return themeManger.getColor(ThemeManager::ControlTypeNR,
                                        ThemeManager::ElementBackgound,
                                        ThemeManager::StateActive);
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
    d_ptr->timeInterval = static_cast<ResolutionRatio>(t);
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
    d_ptr->updateIndexInfo();
    d_ptr->loadTrendData();
    endResetModel();
}

void TrendTableModel::leftPage(int &curSecCol)
{
    Q_UNUSED(curSecCol)

    int start = d_ptr->indexInfo.start - COLUMN_COUNT;
    if (start < 0)
    {
        return;
    }
    d_ptr->indexInfo.start = start;
    d_ptr->indexInfo.end = start + COLUMN_COUNT;
    if (d_ptr->indexInfo.end > d_ptr->indexInfo.total)
    {
        d_ptr->indexInfo.end = d_ptr->indexInfo.total;
    }

    beginResetModel();
    d_ptr->loadTrendData();
    endResetModel();
}

void TrendTableModel::rightPage(int &curSecCol)
{
    Q_UNUSED(curSecCol)

    int start = d_ptr->indexInfo.start + COLUMN_COUNT;
    if (start >= d_ptr->indexInfo.total)
    {
        return;
    }
    int end = d_ptr->indexInfo.end + COLUMN_COUNT;
    if (end > d_ptr->indexInfo.total)
    {
        end = d_ptr->indexInfo.total;
    }
    d_ptr->indexInfo.start = start;
    d_ptr->indexInfo.end = end;

    beginResetModel();
    d_ptr->loadTrendData();
    endResetModel();
}

void TrendTableModel::leftMoveEvent(int &curSecCol)
{
    Q_UNUSED(curSecCol)

    if (d_ptr->eventList.count() == 0)
    {
        return;
    }
    // 初始化事件索引
    int index = d_ptr->eventList.count() - 1;
    if (d_ptr->indexInfo.event < 0)
    {
        d_ptr->indexInfo.event = d_ptr->eventList.last();
    }
    else
    {
        index = d_ptr->eventList.indexOf(d_ptr->indexInfo.event);
        if (index <= 0)
        {
            return;
        }
        d_ptr->indexInfo.event = d_ptr->eventList.at(index - 1);
    }


    int coef = d_ptr->indexInfo.event / COLUMN_COUNT;
    d_ptr->indexInfo.start = coef * COLUMN_COUNT;
    int end = d_ptr->indexInfo.start + COLUMN_COUNT;
    if (end > d_ptr->indexInfo.total)
    {
        end = d_ptr->indexInfo.total;
    }
    d_ptr->indexInfo.end = end;

    beginResetModel();
    d_ptr->loadTrendData();
    endResetModel();
}

void TrendTableModel::rightMoveEvent(int &curSecCol)
{
    Q_UNUSED(curSecCol)

    if (d_ptr->eventList.count() == 0)
    {
        return;
    }
    // 初始化事件索引
    int index = 0;
    if (d_ptr->indexInfo.event < 0)
    {
        d_ptr->indexInfo.event = d_ptr->eventList.at(index);
    }
    else
    {
        index = d_ptr->eventList.indexOf(d_ptr->indexInfo.event);
        if (index < 0 || index == d_ptr->eventList.count() - 1)
        {
            return;
        }
        d_ptr->indexInfo.event = d_ptr->eventList.at(index + 1);
    }


    int coef = d_ptr->indexInfo.event / COLUMN_COUNT;
    d_ptr->indexInfo.start = coef * COLUMN_COUNT;
    int end = d_ptr->indexInfo.start + COLUMN_COUNT;
    if (end > d_ptr->indexInfo.total)
    {
        end = d_ptr->indexInfo.total;
    }
    d_ptr->indexInfo.end = end;

    beginResetModel();
    d_ptr->loadTrendData();
    endResetModel();
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
    if (d_ptr->indexInfo.end <= d_ptr->indexInfo.start
            || d_ptr->indexInfo.end < 1
            || d_ptr->trendDataPack.count() < d_ptr->indexInfo.end)
    {
        start = 0;
        end = 0;
        qDebug() << Q_FUNC_INFO << "Trend table print time wrong";
        return;
    }
    start = d_ptr->trendDataPack.at(d_ptr->indexInfo.start)->time;
    end = d_ptr->trendDataPack.at(d_ptr->indexInfo.end - 1)->time;
}

void TrendTableModel::printTrendData(unsigned startTime, unsigned endTime)
{
    if (startTime >= endTime)
    {
        return;
    }
    IStorageBackend *backend;
    if (d_ptr->isHistory)
    {
        backend = StorageManager::open(d_ptr->historyDataPath + TREND_DATA_FILE_NAME, QIODevice::ReadOnly);
    }
    else
    {
        backend = trendDataStorageManager.backend();
    }
    if (backend->getBlockNR() <= 0)
    {
        return;
    }

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
            startIndex = d_ptr->trendDataPack.at(midPos)->index;
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
            endIndex = d_ptr->trendDataPack.at(midPos)->index;
            break;
        }
    }

    TrendTablePrintInfo printInfo;
    printInfo.startIndex = startIndex;
    printInfo.stopIndex = endIndex;
    printInfo.interval = TrendDataSymbol::convertValue(d_ptr->timeInterval);
    printInfo.list = d_ptr->displayList;
    for (int i = startIndex; i < endIndex; i++)
    {
        printInfo.timestampEventMap[d_ptr->trendDataPack.at(i)->time] = d_ptr->trendDataPack.at(i)->status;
    }
    RecordPageGenerator *gen = new TrendTablePageGenerator(backend, printInfo);
    if (recorderManager.isPrinting() && !d_ptr->isWait)
    {
        if (gen->getPriority() <= recorderManager.getCurPrintPriority())
        {
            gen->deleteLater();
        }
        else
        {
            recorderManager.stopPrint();
            d_ptr->generator = gen;
            d_ptr->waitTimerId = startTimer(2000); // 等待2000ms
            d_ptr->isWait = true;
        }
    }
    else if (!recorderManager.getPrintStatus())
    {
        recorderManager.addPageGenerator(gen);
    }
    else
    {
        gen->deleteLater();
    }
}

unsigned TrendTableModel::getColumnCount() const
{
    return COLUMN_COUNT;
}

void TrendTableModel::getCurIndexInfo(unsigned &curIndex, unsigned &totalIndex) const
{
    curIndex = d_ptr->indexInfo.start;
    totalIndex = d_ptr->indexInfo.total;
}

void TrendTableModel::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->printTimerId == ev->timerId())
    {
        if (!recorderManager.isPrinting() || d_ptr->timeoutNum == 10) // 1000ms超时处理
        {
            if (!recorderManager.isPrinting())
            {
                recorderManager.addPageGenerator(d_ptr->generator);
            }
            else
            {
                d_ptr->generator->deleteLater();
                d_ptr->generator = NULL;
            }
            killTimer(d_ptr->printTimerId);
            d_ptr->printTimerId = -1;
            d_ptr->timeoutNum = 0;
        }
        d_ptr->timeoutNum++;
    }
    else if (d_ptr->waitTimerId == ev->timerId())
    {
        d_ptr->printTimerId = startTimer(STOP_PRINT_TIMEOUT);
        killTimer(d_ptr->waitTimerId);
        d_ptr->waitTimerId = -1;
        d_ptr->isWait = false;
    }
}

TrendTableModelPrivate::TrendTableModelPrivate()
    : rowCount(0),
      isHistory(false), historyDataPath(""),
      timeInterval(RESOLUTION_RATIO_5_SECOND),
      printTimerId(-1),
      waitTimerId(-1),
      isWait(false),
      timeoutNum(0),
      generator(NULL)
{
    orderMap.clear();

    QList<ParamID> paramIDList;
    paramManager.getParams(paramIDList);
    qSort(paramIDList);

    for (int i = 0; i < SUB_PARAM_NR; i ++)
    {
        ParamID paramID = paramInfo.getParamID(static_cast<SubParamID>(i));
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
            orderMap.insert(paramID, static_cast<SubParamID>(i));
            break;
        }
        }
    }
}

void TrendTableModelPrivate::getTrendData()
{
    // 获取趋势数据
    IStorageBackend *backend;
    if (isHistory)
    {
        backend = StorageManager::open(historyDataPath + TREND_DATA_FILE_NAME, QIODevice::ReadOnly);
    }
    else
    {
        backend = trendDataStorageManager.backend();
    }
    QByteArray data;
    TrendDataSegment *dataSeg;
    int blockNum = backend->getBlockNR();
    unsigned interval = TrendDataSymbol::convertValue(timeInterval);

    // 清空趋势数据缓存
    qDeleteAll(trendDataPack);
    trendDataPack.clear();

    // 开始装载趋势数据
    int count = 0;
    eventList.clear();
    for (quint32 i = 0; i < static_cast<quint32>(blockNum); i++)
    {
        // 把每一块存储的趋势数据读取出来放入data缓存中,并以TrendDataSegment格式读取
        data = backend->getBlockData(i);
        dataSeg = reinterpret_cast<TrendDataSegment *>(data.data());

        // 筛选数据：剔除不能整除时间间隔的且没有报警状态的数据包
        unsigned timeStamp = dataSeg->timestamp;
        unsigned status = dataSeg->status;
        if (timeInterval != RESOLUTION_RATIO_NIBP && timeStamp % interval != 0)
        {
            if (!(status & (TrendDataStorageManager::CollectStatusAlarm
                       |TrendDataStorageManager::CollectStatusNIBP
                       |TrendDataStorageManager::CollectStatusFreeze
                       |TrendDataStorageManager::CollectStatusPrint
                       |TrendDataStorageManager::CollectStatusCOResult)))
            {
                continue;
            }
        }

        // 选择nibp选项时只筛选触发nibp测量的数据
        if (timeInterval == RESOLUTION_RATIO_NIBP && !(status & TrendDataStorageManager::CollectStatusNIBP))
        {
            continue;
        }

        // 把dataSeg中的数据加载到pack中
        TrendDataPackage *pack = new TrendDataPackage;
        pack->time = timeStamp;
        pack->co2Baro = dataSeg->co2Baro;
        for (int j = 0; j < dataSeg->trendValueNum; j ++)
        {
            // 非nibp事件的nibp参数强制显示为无效数据
            SubParamID id = static_cast<SubParamID>(dataSeg->values[j].subParamId);
            TrendDataType value = dataSeg->values[j].value;
            if (id == SUB_PARAM_NIBP_SYS
                    || id == SUB_PARAM_NIBP_DIA
                    || id == SUB_PARAM_NIBP_MAP)
            {
                if (!(status & TrendDataStorageManager::CollectStatusNIBP))
                {
                    value = InvData();
                }
            }

            pack->subparamValue[id] = value;
            pack->subparamAlarm[id] = dataSeg->values[j].alarmFlag;
        }
        pack->status = status;
        pack->alarmFlag = dataSeg->eventFlag;
        pack->index = i;
        trendDataPack.append(pack);

        // 更新生理报警事件列表
        if ((pack->status & TrendDataStorageManager::CollectStatusAlarm)
                || (pack->status & TrendDataStorageManager::CollectStatusPrint)
                || (pack->status & TrendDataStorageManager::CollectStatusFreeze)
                || (pack->status & TrendDataStorageManager::CollectStatusNIBP) )
        {
            eventList.append(count);
        }
        count++;
    }

    // 如果是历史数据，删除备份区数据
    if (isHistory)
    {
        delete backend;
        backend = NULL;
    }

    indexInfo.total = count;
}

void TrendTableModelPrivate::updateIndexInfo()
{
    int count = trendDataPack.count() - 1;
    int coef = count / COLUMN_COUNT;
    int remainder = count % COLUMN_COUNT;
    indexInfo.start = coef * COLUMN_COUNT;
    indexInfo.end = indexInfo.start + remainder + 1;
    indexInfo.event = -1;
    indexInfo.total = count + 1;
}

void TrendTableModelPrivate::loadTrendData()
{
    // 索引超出范围，不再装载数据，直接退出
    if (indexInfo.total < indexInfo.end)
    {
        return;
    }

    // 装载趋势表头数据
    colHeadList.clear();
    for (int i = indexInfo.start; i < indexInfo.end; i++)
    {
        TrendDataPackage *pack = trendDataPack.at(i);

        // 填充TrendDataContent结构体dataStr成员
        QString time;
        timeDate.getTime(pack->time, time, true);
        TrendDataContent colHeadContent(time);

        // 填充TrendDataContent结构体backGroundColor成员
        if (pack->alarmFlag == true)
        {
            AlarmPriority prio = ALARM_PRIO_PROMPT;
            int cnt = displayList.count();
            for (int j = cnt - 1; j >= 0; j--)
            {
                SubParamID id = displayList.at(j);
                bool alarmed = pack->subparamAlarm.value(id, false);
                prio = alarmConfig.getLimitAlarmPriority(id);
                if (alarmed == true)
                {
                    break;
                }
            }
            if (prio == ALARM_PRIO_HIGH)
            {
                colHeadContent.backGroundColor = HIGH_PRIO_ALARM_COLOR;
            }
            else if (prio == ALARM_PRIO_MED)
            {
                colHeadContent.backGroundColor = MED_PRIO_ALARM_COLOR;
            }
        }

        if ((pack->status & TrendDataStorageManager::CollectStatusPrint)
                || (pack->status & TrendDataStorageManager::CollectStatusFreeze)
                || (pack->status & TrendDataStorageManager::CollectStatusNIBP))
        {
            colHeadContent.backGroundColor = MANUAL_EVENT_COLOR;
        }

        // 填充colHeadList链表
        colHeadList.append(colHeadContent);
    }

    // 装载趋势表体数据
    tableDataList.clear();
    for (int i = indexInfo.start; i < indexInfo.end; i++)
    {
        TrendDataPackage *pack = trendDataPack.at(i);

        // 装载趋势表体中每个pack的数据
        TableDataColList colList;
        for (int j = 0; j < rowCount; j++)
        {
            TrendDataContent dContent;
            SubParamID id = displayList.at(j);

            // 装载参数数据
            switch (id)
            {
            case SUB_PARAM_NIBP_SYS:
            {
                ParamID paramId = paramInfo.getParamID(static_cast<SubParamID>(id));
                UnitType type = paramManager.getSubParamUnit(paramId, static_cast<SubParamID>(id));
                int sysData = pack->subparamValue.value(static_cast<SubParamID>(id), InvData());;
                int diaData = pack->subparamValue.value(static_cast<SubParamID>(id + 1), InvData());
                int mapData = pack->subparamValue.value(static_cast<SubParamID>(id + 2), InvData());
                QString sysStr = QString::number(sysData);
                QString diaStr = QString::number(diaData);
                QString mapStr = QString::number(mapData);
                if (type != UNIT_MMHG && sysData != InvData() &&
                        diaData != InvData() && mapData != InvData())
                {
                    sysStr = Unit::convert(type, UNIT_MMHG, sysData, co2Param.getBaro());
                    diaStr = Unit::convert(type, UNIT_MMHG, diaData, co2Param.getBaro());
                    mapStr = Unit::convert(type, UNIT_MMHG, mapData, co2Param.getBaro());
                }
                sysStr = sysData == InvData() ? "---" : sysStr;
                diaStr = diaData == InvData() ? "---" : diaStr;
                mapStr = mapData == InvData() ? "---" : mapStr;
                dContent.dataStr = sysStr + "/" + diaStr + "\n(" + mapStr + ")";
            }
            break;
            case SUB_PARAM_ART_SYS:
            case SUB_PARAM_PA_SYS:
            case SUB_PARAM_AUXP1_SYS:
            case SUB_PARAM_AUXP2_SYS:
            {
                qint16 ibpSys = pack->subparamValue.value(static_cast<SubParamID>(id - 2), InvData());
                qint16 ibpDia = pack->subparamValue.value(static_cast<SubParamID>(id - 1), InvData());
                qint16 ibpMap = pack->subparamValue.value(id, InvData());
                QString sysStr = ibpSys == InvData() ? "---" : QString::number(ibpSys);
                QString diaStr = ibpDia == InvData() ? "---" : QString::number(ibpDia);
                QString mapStr = ibpMap == InvData() ? "---" : QString::number(ibpMap);
                dContent.dataStr = sysStr + "/" + diaStr + "\n(" + mapStr + ")";
            }
            break;
            case SUB_PARAM_ETCO2:
            case SUB_PARAM_ETN2O:
            case SUB_PARAM_ETAA1:
            case SUB_PARAM_ETAA2:
            case SUB_PARAM_ETO2:
            case SUB_PARAM_T1:
            {
                qint16 data1 = pack->subparamValue.value(id, InvData());
                qint16 data2 = pack->subparamValue.value(static_cast<SubParamID>(id + 1), InvData());

                QString dataStr1;
                QString dataStr2;

                if (id == SUB_PARAM_T1 || id == SUB_PARAM_ETCO2)
                {
                    double dubData1 = data1 * 1.0 / 10;
                    double dubData2 = data2 * 1.0 / 10;
                    QString v1Str;
                    QString v2Str;
                    UnitType type = paramManager.getSubParamUnit(paramInfo.getParamID(id), id);
                    if (id == SUB_PARAM_T1)
                    {
                        v1Str = Unit::convert(type, UNIT_TC, dubData1);
                        v2Str = Unit::convert(type, UNIT_TC, dubData2);
                    }
                    else
                    {
                        v1Str = Unit::convert(type, UNIT_PERCENT, dubData1, co2Param.getBaro());
                        v2Str = Unit::convert(type, UNIT_PERCENT, dubData2, co2Param.getBaro());
                    }
                    dataStr1 = data1 == InvData() ? "---" : v1Str;
                    dataStr2 = data2 == InvData() ? "---" : v2Str;
                }
                else
                {
                    dataStr1 = data1 == InvData() ? "---" : QString::number(data1);
                    dataStr2 = data2 == InvData() ? "---" : QString::number(data2);
                }
                dContent.dataStr = dataStr1 + "/" + dataStr2;
            }
            break;
            default:
            {
                qint16 data = pack->subparamValue.value(id, InvData());
                dContent.dataStr = data == InvData() ? "---" : QString::number(data);
            }
            break;
            }

            // 装载参数背景颜色
            bool alarmed = pack->subparamAlarm.value(id, false);
            AlarmPriority prio = alarmConfig.getLimitAlarmPriority(id);
            if (alarmed == true)
            {
                if (prio == ALARM_PRIO_HIGH)
                {
                    dContent.backGroundColor = HIGH_PRIO_ALARM_COLOR;
                }
                else
                {
                    dContent.backGroundColor = MED_PRIO_ALARM_COLOR;
                }
            }

            // 装载每个包的解析数据
            colList.append(dContent);
        }

        // 装载所有包的解析数据
        tableDataList.append(colList);
    }
}

QString TrendTableModelPrivate::getParamName(int section)
{
    SubParamID id = displayList.at(section);
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

int TrendTableModelPrivate::addModuleCheck(int trendGroup)
{
    int index = trendGroup;
    // 加入模块支持条件判断
    while (index >= 0)
    {
        switch (index)
        {
            case TREND_GROUP_RESP:
            {
                if (!systemManager.isSupport(CONFIG_RESP))
                {
                    index = -1;
                }
                return index;
            }
            break;
            case TREND_GROUP_IBP:
            {
                if (!systemManager.isSupport(CONFIG_IBP))
                {
                    index = TREND_GROUP_RESP;
                    break;
                }
                return index;
            }
            break;
            case TREND_GROUP_AG:
            {
                if (!systemManager.isSupport(CONFIG_AG))
                {
                    index = TREND_GROUP_IBP;
                    break;
                }
                return index;
            }
            break;
        }
    }
    return index;
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
    // nibp选项时，调整趋势表垂直参数顺序
    int index = displayList.indexOf(SUB_PARAM_NIBP_SYS);
    if (index < 0)
    {
        return;
    }
    if (timeInterval == RESOLUTION_RATIO_NIBP)
    {
        displayList.move(index, 0);
    }
}
