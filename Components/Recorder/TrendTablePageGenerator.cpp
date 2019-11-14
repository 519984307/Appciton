/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/30
 **/

#include "TrendTablePageGenerator.h"
#include "PatientManager.h"
#include <QList>
#include <QByteArray>
#include "ParamManager.h"
#include "ParamInfo.h"
#include <QDateTime>
#include "TrendDataStorageManager.h"
#include "Framework/Language/LanguageManager.h"
#include "Framework/TimeDate/TimeDate.h"

// 打印纸每页最多打印10行数据（1 head title + 9 data）
#define RECORD_PER_PAGE 9
class TrendTablePageGeneratorPrivate
{
public:
    TrendTablePageGeneratorPrivate()
        : curPageType(RecordPageGenerator::TitlePage),
          backend(NULL),
          curIndex(-1),
          stopIndex(-1),
          interval(30)
    {}

    // load data from the backend, if don't have any more data, return false
    bool loadStringList();

    // add sub param value to the string list
    void addSubParamValueToStringList(const TrendDataPackage &datapack, const QList<SubParamID> &subParamIDs,
                                      const unsigned eventType);

    RecordPageGenerator::PageType curPageType;
    IStorageBackend *backend;
    QList<QStringList> stringLists;
    int curIndex;
    int stopIndex;
    int interval;
    QList<SubParamID> subParamList;
    QMap<unsigned int, unsigned int> timestampEventMap;
    PatientInfo patientInfo;        // 病人信息
};

bool  dataPacketLessThan(const TrendDataPackage &d1, const TrendDataPackage &d2)
{
    return d1.time > d2.time;
}

bool TrendTablePageGeneratorPrivate::loadStringList()
{
    if (backend == NULL || curIndex < 0 || stopIndex < 0)
    {
        return false;
    }

    if (curIndex < stopIndex)
    {
        // reach the stop index
        return false;
    }

    stringLists.clear();

    int count = 0;
    /* 1. get $RECORD_PER_PAGE trend data at most
     * 2. the trend data timestamp can divide by the interval
     * 3. the trend data must have  save sub param id.
     * 4. when the sub param id is different, we need to print it in next round
     */
    for (; curIndex >= stopIndex && count < RECORD_PER_PAGE; curIndex--)
    {
        quint32 dataLength = backend->getBlockDataLen(curIndex);

        QByteArray data(dataLength, 0);

        if (backend->readBlockData(curIndex, data.data(), dataLength) != dataLength)
        {
            continue;
        }

        TrendDataSegment *dataSeg = reinterpret_cast<TrendDataSegment *>(data.data());

        // 使得趋势表显示内容与打印内容保持一致
        if (timestampEventMap.contains(dataSeg->timestamp) == false)
        {
            continue;
        }

        TrendDataPackage dataPackage = parseTrendSegment(dataSeg);
        unsigned eventType = timestampEventMap[dataSeg->timestamp];
        addSubParamValueToStringList(dataPackage, subParamList, eventType);

        count++;
    }

    return count > 0;
}

static void preparePressSubParamInfos(SubParamID subParamID, const TrendDataPackage &datapack, TrendDataType data[],
                                      bool alarms[], unsigned eventType)
{
    switch (subParamID)
    {
    case SUB_PARAM_NIBP_SYS:
        data[0] = datapack.subparamValue.value(SUB_PARAM_NIBP_SYS, InvData());
        data[1] = datapack.subparamValue.value(SUB_PARAM_NIBP_DIA, InvData());
        data[2] = datapack.subparamValue.value(SUB_PARAM_NIBP_MAP, InvData());
        if (eventType & TrendDataStorageManager::CollectStatusNIBP)
        {
            alarms[0] = datapack.subparamAlarm.value(SUB_PARAM_NIBP_SYS, false);
            alarms[1] = datapack.subparamAlarm.value(SUB_PARAM_NIBP_DIA, false);
            alarms[2] = datapack.subparamAlarm.value(SUB_PARAM_NIBP_MAP, false);
        }
        else
        {
            alarms[0] = false;
            alarms[1] = false;
            alarms[2] = false;
        }
        break;
    case SUB_PARAM_ART_SYS:
        data[0] = datapack.subparamValue.value(SUB_PARAM_ART_SYS, InvData());
        data[1] = datapack.subparamValue.value(SUB_PARAM_ART_DIA, InvData());
        data[2] = datapack.subparamValue.value(SUB_PARAM_ART_MAP, InvData());
        alarms[0] = datapack.subparamAlarm.value(SUB_PARAM_ART_SYS, false);
        alarms[1] = datapack.subparamAlarm.value(SUB_PARAM_ART_DIA, false);
        alarms[2] = datapack.subparamAlarm.value(SUB_PARAM_ART_MAP, false);
        break;
    case SUB_PARAM_PA_SYS:
        data[0] = datapack.subparamValue.value(SUB_PARAM_PA_SYS, InvData());
        data[1] = datapack.subparamValue.value(SUB_PARAM_PA_DIA, InvData());
        data[2] = datapack.subparamValue.value(SUB_PARAM_PA_MAP, InvData());
        alarms[0] = datapack.subparamAlarm.value(SUB_PARAM_PA_SYS, false);
        alarms[1] = datapack.subparamAlarm.value(SUB_PARAM_PA_DIA, false);
        alarms[2] = datapack.subparamAlarm.value(SUB_PARAM_PA_MAP, false);
        break;
    case SUB_PARAM_CVP_MAP:
        data[0] = datapack.subparamValue.value(SUB_PARAM_CVP_MAP, InvData());
        alarms[0] = datapack.subparamAlarm.value(SUB_PARAM_CVP_MAP, false);
        break;
    case SUB_PARAM_LAP_MAP:
        data[0] = datapack.subparamValue.value(SUB_PARAM_LAP_MAP, InvData());
        alarms[0] = datapack.subparamAlarm.value(SUB_PARAM_LAP_MAP, false);
        break;
    case SUB_PARAM_RAP_MAP:
        data[0] = datapack.subparamValue.value(SUB_PARAM_RAP_MAP, InvData());
        alarms[0] = datapack.subparamAlarm.value(SUB_PARAM_RAP_MAP, false);
        break;
    case SUB_PARAM_ICP_MAP:
        data[0] = datapack.subparamValue.value(SUB_PARAM_ICP_MAP, InvData());
        alarms[0] = datapack.subparamAlarm.value(SUB_PARAM_ICP_MAP, false);
        break;
    case SUB_PARAM_AUXP1_SYS:
        data[0] = datapack.subparamValue.value(SUB_PARAM_AUXP1_SYS, InvData());
        data[1] = datapack.subparamValue.value(SUB_PARAM_AUXP1_DIA, InvData());
        data[2] = datapack.subparamValue.value(SUB_PARAM_AUXP1_MAP, InvData());
        alarms[0] = datapack.subparamAlarm.value(SUB_PARAM_AUXP1_SYS, false);
        alarms[1] = datapack.subparamAlarm.value(SUB_PARAM_AUXP1_DIA, false);
        alarms[2] = datapack.subparamAlarm.value(SUB_PARAM_AUXP1_MAP, false);
        break;
    case SUB_PARAM_AUXP2_SYS:
        data[0] = datapack.subparamValue.value(SUB_PARAM_AUXP2_SYS, InvData());
        data[1] = datapack.subparamValue.value(SUB_PARAM_AUXP2_DIA, InvData());
        data[2] = datapack.subparamValue.value(SUB_PARAM_AUXP2_MAP, InvData());
        alarms[0] = datapack.subparamAlarm.value(SUB_PARAM_AUXP2_SYS, false);
        alarms[1] = datapack.subparamAlarm.value(SUB_PARAM_AUXP2_DIA, false);
        alarms[2] = datapack.subparamAlarm.value(SUB_PARAM_AUXP2_MAP, false);
        break;
    default:
        break;
    }
}

static QString contructPressTrendStringItem(SubParamID subParamId, TrendDataType *data, bool *almFlag,
        UnitType unit, UnitType defaultUnit)
{
    QString valueStr;
    Q_ASSERT(data != NULL);
    Q_ASSERT(almFlag != NULL);

    // get value
    TrendDataType sys = InvData();
    TrendDataType dia = InvData();
    TrendDataType map = InvData();
    // have 3 press value by default
    int valueNum = 1;

    switch (subParamId)
    {
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP2_SYS:
    case SUB_PARAM_NIBP_SYS:
        valueNum = 3;
        sys = data[0];
        dia = data[1];
        map = data[2];
        break;
    default:
        map = data[0];
        break;
    }

    if (valueNum == 1)
    {
        if (almFlag[0])
        {
            valueStr = "(*%1)";
        }
        else
        {
            valueStr = "(%1)";
        }
        if (map == InvData())
        {
            valueStr = valueStr.arg(InvStr());
        }
        else
        {
            int mul = paramInfo.getMultiOfSubParam(subParamId);
            if (mul == 1)
            {
                valueStr = valueStr.arg(Unit::convert(unit, defaultUnit, map));
            }
            else
            {
                valueStr = valueStr.arg(Unit::convert(unit, defaultUnit, map * 1.0 / mul));
            }
        }
    }
    else
    {
        if (almFlag[0])
        {
            valueStr = "*%1";
        }
        else
        {
            valueStr = "%1";
        }

        if (almFlag[1])
        {
            valueStr += "/*%2";
        }
        else
        {
            valueStr += "/%2";
        }

        if (almFlag[2])
        {
            valueStr += "(*%3)";
        }
        else
        {
            valueStr += "(%3)";
        }

        if (sys == InvData())
        {
            valueStr = valueStr.arg(InvStr()).arg(InvStr()).arg(InvStr());
        }
        else
        {
            int mul = paramInfo.getMultiOfSubParam(subParamId);
            if (mul == 1)
            {
                valueStr = valueStr.arg(Unit::convert(unit, defaultUnit, sys))
                           .arg(Unit::convert(unit, defaultUnit, dia))
                           .arg(Unit::convert(unit, defaultUnit, map));
            }
            else
            {
                valueStr = valueStr.arg(Unit::convert(unit, defaultUnit, sys * 1.0 / mul))
                           .arg(Unit::convert(unit, defaultUnit, dia * 1.0 / mul))
                           .arg(Unit::convert(unit, defaultUnit, map * 1.0 / mul));
            }
        }
    }

    return valueStr;
}
static QString constructNormalValueString(SubParamID subParamId, TrendDataType data, bool almFlag,
        UnitType unit, UnitType defaultUnit, short co2Bro)
{
    QString valueStr;
    // value
    int mul = paramInfo.getMultiOfSubParam(subParamId);
    if (almFlag)
    {
        valueStr += "*";
    }
    if (data == InvData())
    {
        valueStr += InvStr();
    }
    else
    {
        if (1 == mul)
        {
            valueStr += Unit::convert(unit, defaultUnit, data, co2Bro);
        }
        else
        {
            valueStr += Unit::convert(unit, defaultUnit, data * 1.0 / mul, co2Bro);
        }
    }

    return valueStr;
}

void TrendTablePageGeneratorPrivate::addSubParamValueToStringList(const TrendDataPackage &datapack,
        const QList<SubParamID> &subParamIDs, const unsigned eventType)
{
    bool needAddCaption = stringLists.isEmpty();  // if the stringLists is empty, we need to add caption;

    int index = 0;

    if (needAddCaption)
    {
        stringLists.append(QStringList() << trs("Time"));
        stringLists.append(QStringList() << trs("Event"));
    }

    QString timeDateStr = timeDate->getDateTime(datapack.time, true, true);
    stringLists[index++].append(timeDateStr);

    // 优先打印报警事件状态
    if (eventType & TrendDataStorageManager::CollectStatusAlarm)
    {
        stringLists[index++].append("A");
    }
    else if (eventType & TrendDataStorageManager::CollectStatusPrint ||
            eventType & TrendDataStorageManager::CollectStatusFreeze ||
            eventType & TrendDataStorageManager::CollectStatusNIBP)
    {
        stringLists[index++].append("M");
    }
    else
    {
        stringLists[index++].append("");
    }

    foreach(SubParamID subParamID, subParamIDs)
    {
        bool display = true;
        QString caption;
        QString valueStr;
        ParamID paramID = paramInfo.getParamID(subParamID);
        switch (subParamID)
        {
        case SUB_PARAM_NIBP_DIA:
        case SUB_PARAM_NIBP_MAP:
        case SUB_PARAM_ART_DIA:
        case SUB_PARAM_ART_MAP:
        case SUB_PARAM_PA_DIA:
        case SUB_PARAM_PA_MAP:
        case SUB_PARAM_AUXP1_DIA:
        case SUB_PARAM_AUXP1_MAP:
        case SUB_PARAM_AUXP2_DIA:
        case SUB_PARAM_AUXP2_MAP:
        case SUB_PARAM_FICO2:
        case SUB_PARAM_FIN2O:
        case SUB_PARAM_FIAA1:
        case SUB_PARAM_FIAA2:
        case SUB_PARAM_FIO2:
        case SUB_PARAM_T2:
        case SUB_PARAM_TD:
            // Note: ignore this subparamid, because it will display with other subparam togethor
            // for example, nibp_dia and nibp_map will display with nibp_sys
            display = false;
            break;
        case SUB_PARAM_NIBP_SYS:
        {
            if (needAddCaption)
            {
                caption = paramInfo.getParamName(paramID);
            }

            TrendDataType datas[3];
            bool alarms[3];
            preparePressSubParamInfos(subParamID, datapack, datas, alarms, eventType);
            // set datas[] as InvData() when it is not NIBP event type
            if (!(eventType & TrendDataStorageManager::CollectStatusNIBP))
            {
                datas[0] = datas[1] = datas[2] = InvData();
            }
            valueStr = contructPressTrendStringItem(subParamID,
                                                    datas,
                                                    alarms,
                                                    paramManager.getSubParamUnit(paramID, subParamID),
                                                    paramInfo.getUnitOfSubParam(subParamID));
        }
        break;
        case SUB_PARAM_ART_SYS:
        case SUB_PARAM_PA_SYS:
        case SUB_PARAM_CVP_MAP:
        case SUB_PARAM_LAP_MAP:
        case SUB_PARAM_RAP_MAP:
        case SUB_PARAM_AUXP1_SYS:
        case SUB_PARAM_AUXP2_SYS:
        {
            if (needAddCaption)
            {
                caption = paramInfo.getIBPPressName(subParamID);
            }

            TrendDataType datas[3];
            bool alarms[3];
            preparePressSubParamInfos(subParamID, datapack, datas, alarms, eventType);
            valueStr = contructPressTrendStringItem(subParamID,
                                                    datas,
                                                    alarms,
                                                    paramManager.getSubParamUnit(paramID, subParamID),
                                                    paramInfo.getUnitOfSubParam(subParamID));
        }
        break;
        case SUB_PARAM_ETCO2:
        case SUB_PARAM_ETN2O:
        case SUB_PARAM_ETAA1:
        case SUB_PARAM_ETAA2:
        case SUB_PARAM_ETO2:
        {
            // ET and FI display together
            if (needAddCaption && subParamID != SUB_PARAM_ETCO2)
            {
                caption = QString("%1/%2").arg(trs(paramInfo.getSubParamName(subParamID)))
                          .arg(trs(paramInfo.getSubParamName((SubParamID)(subParamID + 1))));
            }
            // 更新etco2的打印表头显示，与趋势表中显示表头保持一致
            if (needAddCaption && subParamID == SUB_PARAM_ETCO2)
            {
                QString name;
                name = paramInfo.getSubParamName(SUB_PARAM_ETCO2);
                caption = name.right(3);
                caption += '(';
                caption += trs(name.left(name.length() - 3));
                caption += '/';
                name = paramInfo.getSubParamName(SUB_PARAM_FICO2);
                caption += trs(name.left(name.length() - 3));
                caption += ')';
            }

            QString valueEt = constructNormalValueString(subParamID,
                              datapack.subparamValue[subParamID],
                              datapack.subparamAlarm[subParamID],
                              paramManager.getSubParamUnit(paramID, subParamID),
                              paramInfo.getUnitOfSubParam(subParamID),
                              datapack.co2Baro);
            SubParamID nextSubParamID = (SubParamID)(subParamID + 1);
            QString valueFi = constructNormalValueString(nextSubParamID,
                              datapack.subparamValue[nextSubParamID],
                              datapack.subparamAlarm[nextSubParamID],
                              paramManager.getSubParamUnit(paramID, nextSubParamID),
                              paramInfo.getUnitOfSubParam(nextSubParamID),
                              datapack.co2Baro);
            valueStr = QString("%1/%2").arg(valueEt).arg(valueFi);
        }
        break;
        case SUB_PARAM_T1:
        {
            if (needAddCaption)
            {
                caption = QString("%1/%2/%3").arg(trs(paramInfo.getSubParamName(SUB_PARAM_T1)))
                          .arg(trs(paramInfo.getSubParamName(SUB_PARAM_T2)))
                          .arg(trs(paramInfo.getSubParamName(SUB_PARAM_TD)));
            }

            QString valueT1 = constructNormalValueString(SUB_PARAM_T1,
                              datapack.subparamValue[SUB_PARAM_T1],
                              datapack.subparamAlarm[SUB_PARAM_T1],
                              paramManager.getSubParamUnit(paramID, SUB_PARAM_T1),
                              paramInfo.getUnitOfSubParam(SUB_PARAM_T1),
                              datapack.co2Baro);
            QString valueT2 = constructNormalValueString(SUB_PARAM_T2,
                              datapack.subparamValue[SUB_PARAM_T2],
                              datapack.subparamAlarm[SUB_PARAM_T2],
                              paramManager.getSubParamUnit(paramID, SUB_PARAM_T2),
                              paramInfo.getUnitOfSubParam(SUB_PARAM_T2),
                              datapack.co2Baro);

            QString valueTD;
            if (datapack.subparamValue[SUB_PARAM_T1] == InvData()
                    || datapack.subparamValue[SUB_PARAM_T2] == InvData())
            {
                // 有一个无效数据，计算的温度差则无效
                valueTD = InvStr();
            }
            else
            {
                QString t1 = valueT1;
                if (t1.contains("*"))
                {
                    t1.remove("*");
                }

                QString t2 = valueT2;
                if (t2.contains("*"))
                {
                    t2.remove("*");
                }

                if (datapack.subparamAlarm[SUB_PARAM_TD])
                {
                    valueTD += "*";
                }
                valueTD += QString::number(qAbs(t1.toFloat() - t2.toFloat()), 'f', 1);
            }

            valueStr = QString("%1/%2/%3").arg(valueT1).arg(valueT2).arg(valueTD);
        }
        break;
        default:
        {
            if (needAddCaption)
            {
                caption = trs(paramInfo.getSubParamName(subParamID));
            }

            valueStr = constructNormalValueString(subParamID,
                                                  datapack.subparamValue[subParamID],
                                                  datapack.subparamAlarm[subParamID],
                                                  paramManager.getSubParamUnit(paramID, subParamID),
                                                  paramInfo.getUnitOfSubParam(subParamID),
                                                  datapack.co2Baro);
        }
        break;
        }

        if (display)
        {
            if (needAddCaption)
            {
                UnitType unit = paramManager.getSubParamUnit(paramID, subParamID);
                QString s = QString("%1(%2)").arg(caption)
                            .arg(trs(Unit::getSymbol(unit)));

                stringLists.append(QStringList() << s);
            }
            stringLists[index++].append(valueStr);
        }
    }
}

TrendTablePageGenerator::TrendTablePageGenerator(IStorageBackend *backend, const TrendTablePrintInfo &printInfo,
                                                 const PatientInfo &patientInfo, QObject *parent)
    : RecordPageGenerator(parent), d_ptr(new TrendTablePageGeneratorPrivate)
{
    if (printInfo.startIndex < printInfo.stopIndex)
    {
        d_ptr->curIndex = printInfo.stopIndex;
        d_ptr->stopIndex = printInfo.startIndex;
    }
    else
    {
        d_ptr->curIndex = printInfo.startIndex;
        d_ptr->stopIndex = printInfo.stopIndex;
    }

    if (backend && d_ptr->curIndex >= static_cast<int>(backend->getBlockNR()))
    {
        d_ptr->curIndex = backend->getBlockNR() - 1;
    }

    d_ptr->backend = backend;
    d_ptr->interval = printInfo.interval;
    d_ptr->subParamList = printInfo.list;
    d_ptr->timestampEventMap = printInfo.timestampEventMap;
    d_ptr->patientInfo = patientInfo;
}

TrendTablePageGenerator::~TrendTablePageGenerator()
{
}

int TrendTablePageGenerator::type() const
{
    return Type;
}

RecordPage *TrendTablePageGenerator::createPage()
{
    switch (d_ptr->curPageType)
    {
    case TitlePage:
        // BUG: patient info of the event might not be the current session patient
        d_ptr->curPageType = TrendTablePage;
        return createTitlePage(QString(trs("TabularTrendsPrint")), d_ptr->patientInfo);
    case TrendTablePage:
        if (!d_ptr->stringLists.isEmpty() || d_ptr->loadStringList())
        {
            QStringList strList = d_ptr->stringLists.takeFirst();
            return createStringListSegemnt(strList);
        }
    // fall through when the stringLists is empty and can't load any more data
    case EndPage:
        d_ptr->curPageType = NullPage;
        return createEndPage();
    default:
        break;
    }
    return NULL;
}
