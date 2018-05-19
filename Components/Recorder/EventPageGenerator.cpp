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
#include "Utility.h"
#include "TrendDataStorageManager.h"
#include "WaveformCache.h"
#include "ECGParam.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "RESPParam.h"
#include "SPO2Param.h"
#include "AGParam.h"
#include "IBPParam.h"
#include "CO2Param.h"


class EventPageGeneratorPrivate
{
public:
    EventPageGeneratorPrivate(EventPageGenerator * const q_ptr)
        : q_ptr(q_ptr),
          curPageType(RecordPageGenerator::TitlePage),
          curDrawWaveSegment(0),
          hasParseData(false)
    {

    }

    // parse the event data
    bool parseData()
    {
        if(!ctx.parse(backend, eventIndex))
        {
            qWarning()<<"Parse event data failed";
            return false;
        }

        switch(ctx.infoSegment->type)
        {
        case EventPhysiologicalAlarm:
        {
            SubParamID subparamID = (SubParamID) ctx.almSegment->subParamID;
            AlarmLimitIFace *almIface = alertor.getAlarmLimitIFace(subparamID);
            AlarmPriority priority;
            if (almIface)
            {
                priority = almIface->getAlarmPriority(ctx.almSegment->alarmType);
            }
            else
            {
                return false;
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
            else if(priority == ALARM_PRIO_HIGH)
            {
                titleStr = "***";
            }

            ParamID paramId = paramInfo.getParamID(subparamID);
            titleStr += " ";
            titleStr += trs(eventStorageManager.getPhyAlarmMessage(paramId,
                                                                   ctx.almSegment->alarmType,
                                                                   ctx.almSegment->alarmInfo & 0x01));

            if(!(ctx.almSegment->alarmInfo & 0x01))
            {
                if(ctx.almSegment->alarmInfo & 0x02)
                {
                    titleStr += " > ";
                }
                else {
                    titleStr += " < ";
                }

                UnitType unit = paramManager.getSubParamUnit(paramId, subparamID);
                LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subparamID, unit);

                titleStr += Util::convertToString(ctx.almSegment->alarmLimit, config.scale);
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

        foreach (WaveformDataSegment *waveSeg, waveSegments) {
            WaveformID id = waveSeg->waveID;
            QString caption = paramInfo.getParamName(paramInfo.getParamID(id));
            int captionLength = 0;
            RecordWaveSegmentInfo info;
            info.id = id;
            info.sampleRate = waveSeg->sampleRate;
            waveformCache.getRange(id, info.minWaveValue, info.maxWaveValue);
            waveformCache.getBaseline(id, info.waveBaseLine);
            switch(id)
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
                info.waveInfo.ecg.gain = ecgParam.getGain(ecgParam.waveIDToLeadID(id));
                caption = QString("%1   %2").arg(ECGSymbol::convert(ecgParam.waveIDToLeadID(id),
                                                                    ecgParam.getLeadConvention()))
                        .arg(ECGSymbol::convert(ecgParam.getFilterMode()));
                info.waveInfo.ecg.in12LeadMode = windowManager.getUFaceType() == UFACE_MONITOR_12LEAD;
                info.waveInfo.ecg._12LeadDisplayFormat = ecgParam.get12LDisplayFormat();
                captionLength = fontManager.textWidthInPixels(caption, q_ptr->font());
                break;
            case WAVE_RESP:
                info.waveInfo.resp.zoom = respParam.getZoom();
                break;
            case WAVE_SPO2:
                info.waveInfo.spo2.gain = spo2Param.getGain();
                break;
            case WAVE_CO2:
                info.waveInfo.co2.zoom = co2Param.getDisplayZoom();
                break;
            case WAVE_N2O:
            case WAVE_AA1:
            case WAVE_AA2:
            case WAVE_O2:
                info.waveInfo.ag.zoom = agParam.getDisplayZoom();
                break;
            case WAVE_IBP1:
            {
                info.waveInfo.ibp.pressureName = ibpParam.getEntitle(IBP_INPUT_1);
                IBPScaleInfo scaleInfo = ibpParam.getIBPScale(info.waveInfo.ibp.pressureName);
                info.waveInfo.ibp.high = scaleInfo.high;
                info.waveInfo.ibp.low = scaleInfo.low;
                info.waveInfo.ibp.isAuto = scaleInfo.isAuto;
                info.waveInfo.ibp.unit = paramManager.getSubParamUnit(PARAM_IBP, SUB_PARAM_ART_SYS);
            }
                break;
            case WAVE_IBP2:
            {
                info.waveInfo.ibp.pressureName = ibpParam.getEntitle(IBP_INPUT_2);
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
            Util::strlcpy(info.drawCtx.caption, qPrintable(caption), sizeof(info.drawCtx.caption));
            info.drawCtx.curPageFirstXpos = 0.0;
            info.drawCtx.prevSegmentLastYpos = 0.0;
            info.drawCtx.dashOffset = 0.0;
            info.drawCtx.lastWaveFlags = 0;

            info.secondWaveBuff.resize(info.sampleRate);
            infos.append(info);
        }

        //calculate the wave region in the print page
        int waveRegionHeight = (RECORDER_PAGE_HEIGHT - RECORDER_WAVE_UPPER_MARGIN - RECORDER_WAVE_LOWER_MARGIN)
                / infos.size();

        // wave heights when has 3 waves
        int waveHeights[] = {120, 120, 80};

        QList<RecordWaveSegmentInfo>::iterator iter;
        int yOffset = RECORDER_WAVE_UPPER_MARGIN;
        int j=0;
        for(iter = infos.begin(); iter != infos.end(); iter++)
        {
            iter->startYOffset = yOffset;
            if(infos.size() == 3)
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
        if(waveInfos.isEmpty() || ctx.waveSegments.isEmpty() || waveInfos.size() != ctx.waveSegments.size())
        {
            return false;
        }

        for(int i = 0; i< waveInfos.size(); i++)
        {
            if((waveSegmentIndex + 1) * waveInfos.at(i).sampleRate > ctx.waveSegments.at(i)->waveNum)
            {
                return false;
            }

            //copy one second wavedata
            int startIndex = waveSegmentIndex * waveInfos.at(i).sampleRate;
            for(int j = 0; j < waveInfos.at(i).sampleRate; j++)
            {
                waveInfos[i].secondWaveBuff[j] = ctx.waveSegments.at(i)->waveData[startIndex++];
            }
        }

        return true;
    }


    EventPageGenerator * const q_ptr;
    RecordPageGenerator::PageType curPageType;
    QString eventTitle;
    TrendDataPackage trendData;
    QList<RecordWaveSegmentInfo> waveInfos;
    int curDrawWaveSegment;
    EventDataPraseContext ctx;
    IStorageBackend *backend;
    int eventIndex;
    bool hasParseData;
};

EventPageGenerator::EventPageGenerator(IStorageBackend *backend, int eventIndex, QObject *parent)
    :RecordPageGenerator(parent), d_ptr(new EventPageGeneratorPrivate(this))
{

    d_ptr->backend = backend;
    d_ptr->eventIndex = eventIndex;
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
    if(!d_ptr->hasParseData)
    {
        d_ptr->hasParseData = true;
        if(!d_ptr->parseData())
        {
            return NULL;
        }
    }

    if(d_ptr->ctx.eventDataBuf == NULL)
    {
        //no event data
        return NULL;
    }

    switch (d_ptr->curPageType) {
    case TitlePage:
        d_ptr->curPageType = TrendPage;
        // BUG: patient info of the event might not be the current session patient
        return createTitlePage(d_ptr->eventTitle, patientManager.getPatientInfo());

    case TrendPage:
        d_ptr->curPageType = WaveScalePage;
        if(d_ptr->ctx.trendSegment)
        {
            d_ptr->trendData = TrendDataStorageManager::parseTrendSegment(d_ptr->ctx.trendSegment);
            return createTrendPage(d_ptr->trendData, true);
        }
        //fall through
    case WaveScalePage:
        d_ptr->curPageType = WaveSegmentPage;
        if(!d_ptr->ctx.waveSegments.isEmpty())
        {
            d_ptr->waveInfos = d_ptr->getPrintWaveInfos(d_ptr->ctx.waveSegments);
            return createWaveScalePage(d_ptr->waveInfos, recorderManager.getPrintSpeed());
        }
        //fall through
    case WaveSegmentPage:
    {
        if(!d_ptr->ctx.waveSegments.isEmpty()
                && d_ptr->loadWaveData(d_ptr->curDrawWaveSegment)
                && !recorderManager.isAbort())
        {
            RecordPage *page = NULL;

            page = createWaveSegments(d_ptr->waveInfos, d_ptr->curDrawWaveSegment++, recorderManager.getPrintSpeed());

            return page;
        }
    }
        //fall through
    case EndPage:
        d_ptr->curPageType = NullPage;
        return createEndPage();
    default:
        break;
    }

    return NULL;
}
