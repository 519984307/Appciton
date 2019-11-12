/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/13
 **/

#include "ContinuousPageGenerator.h"
#include "Framework/Language/LanguageManager.h"
#include "Framework/TimeDate/TimeDate.h"
#include "Framework/Utility/Utility.h"
#include "PatientManager.h"
#include "TrendCache.h"
#include "SystemManager.h"
#include "ParamInfo.h"
#include "WaveformCache.h"
#include "RecorderManager.h"
#include "ECGParam.h"
#include "RESPParam.h"
#include "SPO2Param.h"
#include "CO2Param.h"
#include "AGParam.h"
#include "IBPParam.h"
#include "ParamManager.h"
#include "IConfig.h"
#include "PrintSettingMenuContent.h"
#include "LayoutManager.h"
#include "EventStorageItem.h"
#include <QPointer>
#include <QDebug>

class ContinuousPageGeneratorPrivate
{
public:
    explicit ContinuousPageGeneratorPrivate(ContinuousPageGenerator *const q_ptr)
        : q_ptr(q_ptr),
          curPageType(RecordPageGenerator::TitlePage),
          curDrawWaveSegment(0),
          totalDrawWaveSegment(-1)
    {
        TrendCacheData data;
        TrendAlarmStatus almStatus;
        unsigned t = timeDate->time();
        trendCache.getTrendData(t, &data);
        trendCache.getTrendAlarmStatus(t, &almStatus);
        bool alarm = false;
        foreach(bool st, almStatus.alarms)
        {
            if (st)
            {
                alarm = true;
                break;
            }
        }
        trendData.subparamValue = data.values;
        trendData.subparamAlarm = almStatus.alarms;
        trendData.co2Baro = data.co2baro;
        trendData.time = t;
        trendData.alarmFlag = alarm;

        PrintTime timeSec = recorderManager.getPrintTime();
        int time = -1;
        if (timeSec == PRINT_TIME_EIGHT_SEC)
        {
            time = 8;
        }
        totalDrawWaveSegment = time;
    }

    QList<RecordWaveSegmentInfo> getPrintWaveInfos();

    void fetchWaveData(bool isRealtime);

    bool isParamStop(WaveformID id);

    ContinuousPageGenerator *q_ptr;
    RecordPageGenerator::PageType curPageType;
    TrendDataPackage trendData;
    QList<RecordWaveSegmentInfo> waveInfos;
    QList<WaveformDataSegment *> waveSegments;      // 前4s波形数据
    QList<WaveformID> waves;                        // 波形ID
    int curDrawWaveSegment;
    int totalDrawWaveSegment;   // total wave segments that need to draw, -1 means unlimit
};

/**
 * @brief ContinuousPageGeneratorPrivate::getPrintWaveInfo get the continuous print wave info
 * @return a list contain the wave segment info of the waves that need to print
 */
QList<RecordWaveSegmentInfo> ContinuousPageGeneratorPrivate::getPrintWaveInfos()
{
    QList<WaveformID> waves;

    // 打印波形信息更新为从配置文件中选择型的方式
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        QString path;
        int w = WAVE_NONE;
        path = QString("Print|SelectWave%1").arg(i + 1);
        systemConfig.getNumValue(path, w);
        waves.append(static_cast<WaveformID>(w));
    }

    return RecordPageGenerator::getWaveInfos(waves);
}

/**
 * @brief ContinuousPageGeneratorPrivate::fetchWaveData
 *          get the wave data from the realtime channel in the wavecache.
 *          we should fetch 1 second wave data form the realtimeChannel,
 *          if wavecache don't have enough data, we fill invalid data.
 */
void ContinuousPageGeneratorPrivate::fetchWaveData(bool isRealtime)
{
    QList<RecordWaveSegmentInfo>::iterator iter;
    for (iter = waveInfos.begin(); iter < waveInfos.end(); ++iter)
    {
        if (iter->secondWaveBuff.size() != iter->sampleRate)
        {
            iter->secondWaveBuff.resize(iter->sampleRate);
        }

        int retryCount = 0;
        if (isRealtime)
        {
            int lastReadSize = 0;
            int curSize = 0;
            while (curSize < iter->sampleRate)
            {
                if (recorderManager.isAbort())
                {
                    // param stop
                    break;
                }

                if (isParamStop(iter->id))
                {
                    // param stop
                    break;
                }

                curSize += waveformCache.readRealtimeChannel(iter->id,
                           iter->sampleRate - curSize,
                           iter->secondWaveBuff.data() + curSize);

                if (++retryCount >= 200)  // 1000 ms has passed and haven't finished reading
                {
                    if (curSize == lastReadSize)
                    {
                        break;
                    }
                }

                if (retryCount > 100 && curSize == 0)
                {
                    // not data after 100 try, no data for this waveform id
                    // stop any further try.
                    break;
                }

                lastReadSize = curSize;
                QPointer<ContinuousPageGenerator> guard(q_ptr);
                Util::waitInEventLoop(5);
                if (guard.isNull()) {
                    qWarning() << Q_FUNC_INFO << "generator is already destroyed, Abort!";
                    return;
                }
            }

            if (curSize < iter->sampleRate)
            {
                // no enough data, fill with invalid
                qFill(iter->secondWaveBuff.data() + curSize, iter->secondWaveBuff.data() + iter->sampleRate,
                       (WaveDataType)(INVALID_WAVE_FALG_BIT << 16));
                qDebug() << Q_FUNC_INFO << "no enough data, fill invalid";
            }
        }
        else
        {
            /* FIXME: this while loop is useless, delete it later
             * curDrawWaveSegment is always less than totalDrawWaveSegement / 2
             */
            while (curDrawWaveSegment >= (totalDrawWaveSegment / 2))
            {
                if (recorderManager.isAbort())
                {
                    // print abort
                    break;
                }

                if (++retryCount >= 220)  // wait more than 1 second
                {
                    break;
                }

                QPointer<ContinuousPageGenerator> guard(q_ptr);
                Util::waitInEventLoop(5);
                if (guard.isNull()) {
                    qWarning() << Q_FUNC_INFO << "generator is already destroyed, Abort!";
                    return;
                }
            }

            if (curDrawWaveSegment < (totalDrawWaveSegment / 2))
            {
                int waveIndex = waves.indexOf(iter->id);
                WaveformDataSegment *waveSegment = waveSegments.at(waveIndex);
                int startIndex = curDrawWaveSegment * waveSegment->sampleRate;
                qMemCopy(iter->secondWaveBuff.data(), waveSegment->waveData + startIndex,
                         waveSegment->sampleRate * sizeof(WaveDataType));
            }
            else
            {
                // fill with invalid data
                iter->secondWaveBuff.fill((WaveDataType)INVALID_WAVE_FALG_BIT);
            }
        }
    }
}

/**
 * @brief ContinuousPageGeneratorPrivate::isParamStop check whether the param is stopped
 * @param id waveform id
 * @return true if stopped, otherwise, return false;
 */
bool ContinuousPageGeneratorPrivate::isParamStop(WaveformID id)
{
    return paramManager.isParamStopped(paramInfo.getParamID(id));
}

ContinuousPageGenerator::ContinuousPageGenerator(QObject *parent)
    : RecordPageGenerator(parent), d_ptr(new ContinuousPageGeneratorPrivate(this))
{
    d_ptr->waveInfos = d_ptr->getPrintWaveInfos();
    // 获取前4秒波形数据
    QList<RecordWaveSegmentInfo>::iterator iter;
    for (iter = d_ptr->waveInfos.begin(); iter < d_ptr->waveInfos.end(); ++iter)
    {
        int waveNum = d_ptr->totalDrawWaveSegment / 2 * iter->sampleRate;
        WaveformDataSegment *waveSegment = reinterpret_cast<WaveformDataSegment *>(qMalloc(sizeof(WaveformDataSegment) +
                                           sizeof(WaveDataType) * waveNum));
        if (waveSegment == NULL)
        {
            qDebug() << Q_FUNC_INFO << "Out of memory!";
            break;
        }
        d_ptr->waveSegments.append(waveSegment);
        waveSegment->waveID = iter->id;
        waveSegment->sampleRate = iter->sampleRate;
        waveSegment->waveNum = waveNum;
        waveformCache.readStorageChannel(iter->id,
                                         waveSegment->waveData,
                                         d_ptr->totalDrawWaveSegment / 2, false);
        d_ptr->waves.append(iter->id);
    }
}

ContinuousPageGenerator::~ContinuousPageGenerator()
{
    foreach(WaveformDataSegment *waveSegment, d_ptr->waveSegments) {
        qFree(waveSegment);
    }
    d_ptr->waveSegments.clear();
}

int ContinuousPageGenerator::type() const
{
    return Type;
}

RecordPageGenerator::PrintPriority ContinuousPageGenerator::getPriority() const
{
    return PriorityContinuous;
}

RecordPage *ContinuousPageGenerator::createPage()
{
    switch (d_ptr->curPageType)
    {
    case TitlePage:
        d_ptr->curPageType = TrendPage;
        return createTitlePage(QString(trs("RealtimeSegmentWavePrint")), patientManager.getPatientInfo(), 0);
    case TrendPage:
        d_ptr->curPageType = WaveScalePage;
        return createTrendPage(d_ptr->trendData, false);
    case WaveScalePage:
        d_ptr->curPageType = WaveSegmentPage;
        return createWaveScalePage(d_ptr->waveInfos, recorderManager.getPrintSpeed());
    case WaveSegmentPage:
        if (!recorderManager.isAbort())
        {
            RecordPage *page;
            QPointer<ContinuousPageGenerator> guard(this);
            if (d_ptr->curDrawWaveSegment < d_ptr->totalDrawWaveSegment / 2)
            {
                d_ptr->fetchWaveData(false);
            }
            else
            {
                d_ptr->fetchWaveData(true);
            }

            if (guard) {
                page = createWaveSegments(&d_ptr->waveInfos, d_ptr->curDrawWaveSegment++,
                                          recorderManager.getPrintSpeed());
            } else {
                /* object is already destroyed, return NULL */
                qWarning() << Q_FUNC_INFO << "generator is already destroyed, Abort!";
                return NULL;
            }

            if (d_ptr->totalDrawWaveSegment > 0 && d_ptr->curDrawWaveSegment >= d_ptr->totalDrawWaveSegment)
            {
                d_ptr->curPageType = EndPage;
            }
            return page;
        }
    // fall through
    case EndPage:
        d_ptr->curPageType = NullPage;
        return createEndPage();
    default:
        break;
    }
    return NULL;
}

void ContinuousPageGenerator::onStartGenerate()
{
    waveformCache.startRealtimeChannel();
}

void ContinuousPageGenerator::onStopGenerate()
{
    waveformCache.stopRealtimeChannel();
}

