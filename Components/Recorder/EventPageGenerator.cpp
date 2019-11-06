/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/28
 **/

#include "EventPageGenerator.h"
#include "PatientManager.h"
#include <EventDataDefine.h>
#include <QDebug>
#include "EventDataParseContext.h"
#include "Alarm.h"
#include "ParamInfo.h"
#include "EventStorageManager.h"
#include "ParamManager.h"
#include "AlarmConfig.h"
#include "Framework/Utility/Utility.h"
#include "WaveformCache.h"
#include "ECGParam.h"
#include "FontManager.h"
#include "RESPParam.h"
#include "SPO2Param.h"
#include "AGParam.h"
#include "IBPParam.h"
#include "CO2Param.h"
#include "LayoutManager.h"
#include "NIBPSymbol.h"
#include "LanguageManager.h"

class EventPageGeneratorPrivate
{
public:
    explicit EventPageGeneratorPrivate(EventPageGenerator *const q_ptr)
        : q_ptr(q_ptr),
          curPageType(RecordPageGenerator::TitlePage),
          curDrawWaveSegment(0), backend(NULL), eventIndex(0),
          hasParseData(false), ecgGain(3)
    {
    }

    // parse the event data
    bool parseData()
    {
        if (!ctx.parse(backend, eventIndex))
        {
            qWarning() << "Parse event data failed";
            return false;
        }

        switch (ctx.infoSegment->type)
        {
        case EventPhysiologicalAlarm:
        {
            SubParamID subparamID = (SubParamID) ctx.almSegment->subParamID;
            AlarmPriority priority;
            unsigned char alarmInfo = ctx.almSegment->alarmInfo;
            unsigned char alarmId = ctx.almSegment->alarmType;
            if (alarmInfo & 0x01)   // oneshot 报警事件
            {
                AlarmOneShotIFace *alarmOneShot = alertor.getAlarmOneShotIFace(subparamID);
                if (alarmOneShot)
                {
                    priority = alarmOneShot->getAlarmPriority(alarmId);
                }
            }
            else
            {
                AlarmLimitIFace *almIface = alertor.getAlarmLimitIFace(subparamID);
                if (almIface)
                {
                    priority = almIface->getAlarmPriority(alarmId);
                }
            }

            QString titleStr;

            if (priority == ALARM_PRIO_LOW)
            {
                titleStr = "*";
            }
            else if (priority == ALARM_PRIO_MED)
            {
                titleStr = "**";
            }
            else if (priority == ALARM_PRIO_HIGH)
            {
                titleStr = "***";
            }

            ParamID paramId = paramInfo.getParamID(subparamID);
            // oneshot 报警
            if (alarmInfo & 0x01)
            {
                // 将参数ID转换为oneshot报警对应的参数ID
                if (paramId == PARAM_DUP_ECG)
                {
                    paramId = PARAM_ECG;
                }
                else if (paramId == PARAM_DUP_RESP)
                {
                    paramId = PARAM_RESP;
                }
            }
            titleStr += " ";
            titleStr += trs(Alarm::getPhyAlarmMessage(paramId,
                            ctx.almSegment->alarmType,
                            ctx.almSegment->alarmInfo & 0x01));

            if (!(ctx.almSegment->alarmInfo & 0x01))
            {
                if (ctx.almSegment->alarmInfo & 0x02)
                {
                    titleStr += " > ";
                }
                else
                {
                    titleStr += " < ";
                }

                UnitType unit = paramManager.getSubParamUnit(paramId, subparamID);
                LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subparamID, unit);

                titleStr += Util::convertToString(ctx.almSegment->alarmLimit, config.scale);
                titleStr += " ";
                titleStr += trs(Unit::getSymbol(unit));
            }

            eventTitle = titleStr;
        }
        break;
        case EventCodeMarker:
        {
            eventTitle = trs(QString(ctx.codeMarkerSegment->codeName));
        }
        break;
        case EventRealtimePrint:
        {
            eventTitle = trs("RealtimePrintSegment");
        }
        break;
        case EventNIBPMeasurement:
        {
            eventTitle = trs("NibpMeasurement");
            if (ctx.measureSegment->measureResult == NIBP_ONESHOT_NONE)
            {
                extraInfo = trs("NIBPMEASURE") + trs("ServiceSuccess");
            }
            else
            {
                extraInfo = trs("NIBPMEASURE") + trs("NIBPFAILED") + ",";
                extraInfo += trs(NIBPSymbol::convert((NIBPOneShotType)(ctx.measureSegment->measureResult)));
            }
        }
        break;
        case EventWaveFreeze:
        {
            eventTitle = trs("WaveFreeze");
        }
        break;
        default:
            break;
        }

        return true;
    }

    /**
     * @brief getPrintWaveInfos get the print wave infos base on the wave segment
     * @param waveSegments wave segments
     * @return print wave segment info list
     */
    QList<RecordWaveSegmentInfo> getPrintWaveInfos(const QVector<WaveformDataSegment *> waveSegments)
    {
        QList<RecordWaveSegmentInfo> infos;

        foreach(WaveformDataSegment *waveSeg, waveSegments)
        {
            WaveformID id = waveSeg->waveID;
            QString caption = trs(paramInfo.getParamName(paramInfo.getParamID(id)));
            int captionLength = 0;
            RecordWaveSegmentInfo info;
            info.id = id;
            info.sampleRate = waveSeg->sampleRate;
            waveformCache.getRange(id, &info.minWaveValue, &info.maxWaveValue);
            info.waveBaseLine = waveformCache.getBaseline(id);
            switch (id)
            {
            case WAVE_ECG_I:
            case WAVE_ECG_II:
            case WAVE_ECG_III:
            case WAVE_ECG_aVR:
            case WAVE_ECG_aVL:
            case WAVE_ECG_aVF:
            case WAVE_ECG_V1:
            case WAVE_ECG_V2:
            case WAVE_ECG_V3:
            case WAVE_ECG_V4:
            case WAVE_ECG_V5:
            case WAVE_ECG_V6:
            {
                info.waveInfo.ecg.gain = static_cast<ECGGain>(ecgGain);
                QString remarks = QString(QLatin1String(waveSeg->remarks));
                QString filterMode = remarks.section(" ", 0, 0);
                QString notchFilter = remarks.section(" ", 1);
                if (filterMode == ECGSymbol::convert(ECG_FILTERMODE_DIAGNOSTIC))
                {
                    caption = QString("%1   %2   %3%4").arg(ECGSymbol::convert(ecgParam.waveIDToLeadID(id),
                                                                               ecgParam.getLeadConvention()))
                            .arg(trs(filterMode)).arg(trs("Notch"))
                            .arg(trs(notchFilter));
                }
                else
                {
                    caption = QString("%1   %2").arg(ECGSymbol::convert(ecgParam.waveIDToLeadID(id),
                                                     ecgParam.getLeadConvention()))
                              .arg(trs(filterMode));
                }
                info.waveInfo.ecg.in12LeadMode = layoutManager.getUFaceType() == UFACE_MONITOR_ECG_FULLSCREEN;
                info.waveInfo.ecg._12LeadDisplayFormat = ecgParam.get12LDisplayFormat();
                captionLength = fontManager.textWidthInPixels(caption, q_ptr->font());
                break;
            }
            case WAVE_RESP:
                info.waveInfo.resp.zoom = respParam.getZoom();
                break;
            case WAVE_SPO2:
                caption = trs(paramInfo.getParamWaveformName(WAVE_SPO2));
                break;
            case WAVE_CO2:
                info.waveInfo.co2.zoom = co2Param.getDisplayZoom();
                break;
            case WAVE_N2O:
                info.waveInfo.ag.zoom = agParam.getDisplayZoom(AG_TYPE_N2O);
                break;
            case WAVE_AA1:
                info.waveInfo.ag.zoom = agParam.getDisplayZoom(AG_TYPE_AA1);
                break;
            case WAVE_AA2:
                info.waveInfo.ag.zoom = agParam.getDisplayZoom(AG_TYPE_AA2);
                break;
            case WAVE_O2:
                info.waveInfo.ag.zoom = agParam.getDisplayZoom(AG_TYPE_O2);
                break;
            case WAVE_ART:
            case WAVE_PA:
            case WAVE_CVP:
            case WAVE_LAP:
            case WAVE_RAP:
            case WAVE_ICP:
            case WAVE_AUXP1:
            case WAVE_AUXP2:
            {
                info.waveInfo.ibp.pressureName = ibpParam.getPressureName(id);
                caption = IBPSymbol::convert(ibpParam.getPressureName(id));
                IBPScaleInfo scaleInfo = ibpParam.getIBPScale(info.waveInfo.ibp.pressureName);
                info.waveInfo.ibp.high = scaleInfo.high;
                info.waveInfo.ibp.low = scaleInfo.low;
                info.waveInfo.ibp.isAuto = scaleInfo.isAuto;
                info.waveInfo.ibp.unit = paramManager.getSubParamUnit(PARAM_IBP, SUB_PARAM_ART_SYS);
                break;
            }
            default:
                break;
            }

            captionLength = fontManager.textWidthInPixels(caption, q_ptr->font());
            info.drawCtx.captionPixLength = captionLength;
            info.drawCtx.caption = caption;
            info.drawCtx.curPageFirstXpos = 0.0;
            info.drawCtx.prevSegmentLastYpos = 0.0;
            info.drawCtx.dashOffset = 0.0;
            info.drawCtx.lastWaveFlags = 0;

            info.secondWaveBuff.resize(info.sampleRate);
            infos.append(info);
        }


        if (infos.size() == 0)
        {
            return infos;
        }

        // calculate the wave region in the print page
        int waveRegionHeight = (RECORDER_PAGE_HEIGHT - RECORDER_WAVE_UPPER_MARGIN - RECORDER_WAVE_LOWER_MARGIN)
                               / infos.size();

        // wave heights when has 3 waves
        int waveHeights[] = {120, 120, 80};

        QList<RecordWaveSegmentInfo>::iterator iter;
        int yOffset = RECORDER_WAVE_UPPER_MARGIN;
        int j = 0;
        for (iter = infos.begin(); iter != infos.end(); ++iter)
        {
            iter->startYOffset = yOffset;
            if (infos.size() == 3)
            {
                yOffset += waveHeights[j++];
            }
            else
            {
                yOffset += waveRegionHeight;
            }
            iter->endYOffset = yOffset;
            iter->middleYOffset = (iter->startYOffset + iter->endYOffset) / 2;
        }

        return infos;
    }

    /**
     * @brief loadWaveData laod wave data for specific wave segment
     * @param waveSegmentIndex  wave segment index
     * @return true if load successfully, otherwise, false
     */
    bool loadWaveData(int waveSegmentIndex)
    {
        if (waveInfos.isEmpty() || ctx.waveSegments.isEmpty() || waveInfos.size() != ctx.waveSegments.size())
        {
            return false;
        }

        for (int i = 0; i < waveInfos.size(); i++)
        {
            if ((waveSegmentIndex + 1) * waveInfos.at(i).sampleRate > ctx.waveSegments.at(i)->waveNum)
            {
                return false;
            }

            // copy one second wavedata
            int startIndex = waveSegmentIndex * waveInfos.at(i).sampleRate;
            for (int j = 0; j < waveInfos.at(i).sampleRate; j++)
            {
                waveInfos[i].secondWaveBuff[j] = ctx.waveSegments.at(i)->waveData[startIndex++];
            }
        }

        return true;
    }


    EventPageGenerator *const q_ptr;
    RecordPageGenerator::PageType curPageType;
    QString eventTitle;
    QString extraInfo;
    TrendDataPackage trendData;
    QList<RecordWaveSegmentInfo> waveInfos;
    int curDrawWaveSegment;
    EventDataPraseContext ctx;
    IStorageBackend *backend;
    int eventIndex;
    bool hasParseData;
    PatientInfo patientInfo;
    int ecgGain;
};

EventPageGenerator::EventPageGenerator(IStorageBackend *backend, int eventIndex,
                                       const PatientInfo &patientInfo, int gain, QObject *parent)
    : RecordPageGenerator(parent), d_ptr(new EventPageGeneratorPrivate(this))
{
    d_ptr->backend = backend;
    d_ptr->eventIndex = eventIndex;
    d_ptr->patientInfo = patientInfo;
    d_ptr->ecgGain = gain;
}

EventPageGenerator::~EventPageGenerator()
{
}

int EventPageGenerator::type() const
{
    return Type;
}

RecordPage *EventPageGenerator::createPage()
{
    if (!d_ptr->hasParseData)
    {
        d_ptr->hasParseData = true;
        if (!d_ptr->parseData())
        {
            return NULL;
        }
    }

    if (d_ptr->ctx.eventDataBuf == NULL)
    {
        // no event data
        return NULL;
    }

    switch (d_ptr->curPageType)
    {
    case TitlePage:
        d_ptr->curPageType = TrendPage;
        // BUG: patient info of the event might not be the current session patient
        return createTitlePage(d_ptr->eventTitle, d_ptr->patientInfo);

    case TrendPage:
        d_ptr->curPageType = WaveScalePage;
        if (d_ptr->ctx.trendSegment)
        {
            d_ptr->trendData = parseTrendSegment(d_ptr->ctx.trendSegment);
            return createTrendPage(d_ptr->trendData, true, QString(), QString(), d_ptr->extraInfo);
        }
    // fall through
    case WaveScalePage:
        d_ptr->curPageType = WaveSegmentPage;
        if (!d_ptr->ctx.waveSegments.isEmpty())
        {
            d_ptr->waveInfos = d_ptr->getPrintWaveInfos(d_ptr->ctx.waveSegments);
            return createWaveScalePage(d_ptr->waveInfos, recorderManager.getPrintSpeed());
        }
    // fall through
    case WaveSegmentPage:
    {
        if (!d_ptr->ctx.waveSegments.isEmpty()
                && d_ptr->loadWaveData(d_ptr->curDrawWaveSegment)
                && !recorderManager.isAbort())
        {
            RecordPage *page = NULL;

            page = createWaveSegments(&d_ptr->waveInfos, d_ptr->curDrawWaveSegment++, recorderManager.getPrintSpeed());

            return page;
        }
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
