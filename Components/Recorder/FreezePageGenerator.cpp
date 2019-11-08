/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/30
 **/

#include "FreezePageGenerator.h"
#include "PatientManager.h"
#include "RecorderManager.h"
#include <QBitArray>
#include <QPainter>
#include "FontManager.h"
#include <QDateTime>
#include "Framework/Language/LanguageManager.h"

class FreezePageGeneratorPrivate
{
public:
    FreezePageGeneratorPrivate()
        : curPageType(RecordPageGenerator::TitlePage),
          curDrawWaveSegment(0),
          allWaveProcess(false)
    {
    }

    /**
     * @brief loadWaveData load wave from the wave segement
     * @param waveSegment
     * @return load success or not
     */
    bool loadWaveData(int waveSegment);


    /**
     * @brief checkAndDrawTimestamp check and draw the timestamp of the last second wave samples
     * @param page
     */
    void checkAndDrawTimestamp(RecordPage *page);


    RecordPageGenerator::PageType curPageType;
    TrendDataPackage trendData;
    QList<RecordWaveSegmentInfo> waveSegInfos;
    QList<FreezePageGenerator::FreezeWaveInfo> freezeWaveInfos;
    QBitArray needDrawTimestamp;
    int curDrawWaveSegment;
    bool allWaveProcess;
};

bool FreezePageGeneratorPrivate::loadWaveData(int waveSegment)
{
    bool hasWaveData = false;
    for (int i = 0; i < freezeWaveInfos.size(); i++)
    {
        int sampleRate = waveSegInfos.at(i).sampleRate;
        int startIndex = waveSegment * sampleRate;
        const QVector<WaveDataType> &dataBuf = freezeWaveInfos.at(i).data;
        QVector<WaveDataType> &destBuf = waveSegInfos[i].secondWaveBuff;
        if (startIndex >= dataBuf.size())
        {
            // no enough data
            destBuf.clear();
        }
        else
        {
            hasWaveData = true;
            int size = dataBuf.size();
            if (destBuf.size() != sampleRate)
            {
                destBuf.resize(sampleRate);
            }
            if (startIndex + sampleRate < size)
            {
                for (int i = 0; i < sampleRate; i++)
                {
                    destBuf[i] = dataBuf[startIndex + i];
                }
            }
            else
            {
                // no enough data to fill the buffer
                int count = 0;
                for (; count + startIndex < size; count++)
                {
                    destBuf[count] = dataBuf[count + startIndex];
                }
                destBuf.resize(count);
                needDrawTimestamp[i] = true;
            }
        }
    }

    return hasWaveData;
}

void FreezePageGeneratorPrivate::checkAndDrawTimestamp(RecordPage *page)
{
    if (!page)
    {
        return;
    }

    for (int i = 0; i < needDrawTimestamp.size(); i++)
    {
        if (needDrawTimestamp.at(i))
        {
            QPainter p(page);
            QFont font = RecordPageGenerator::font();
            p.setFont(font);
            p.setPen(Qt::white);

            QRect rect(0, waveSegInfos.at(i).startYOffset, page->width(), fontManager.textHeightInPixels(font));

            QDateTime dt = QDateTime::fromTime_t(freezeWaveInfos.at(i).timestampOfLastSecond);
            QString time = dt.toString("hh:mm:ss");

            p.drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, time);

            needDrawTimestamp[i] = false;
        }
    }
}

FreezePageGenerator::FreezePageGenerator(const TrendDataPackage &trendData,
        const QList<FreezeWaveInfo> &freezeWaveInfos, QObject *parent)
    : RecordPageGenerator(parent), d_ptr(new FreezePageGeneratorPrivate())
{
    d_ptr->trendData = trendData;
    d_ptr->freezeWaveInfos = freezeWaveInfos;
    d_ptr->needDrawTimestamp = QBitArray(freezeWaveInfos.size());
    QList<WaveformID> ids;
    for (int i = 0; i < freezeWaveInfos.size(); i++)
    {
        ids.append(freezeWaveInfos.at(i).id);
    }

    d_ptr->waveSegInfos = RecordPageGenerator::getWaveInfos(ids);
}

FreezePageGenerator::~FreezePageGenerator()
{
}

int FreezePageGenerator::type() const
{
    return Type;
}

RecordPage *FreezePageGenerator::createPage()
{
    switch (d_ptr->curPageType)
    {
    case TitlePage:
        d_ptr->curPageType = TrendPage;
        return createTitlePage(QString(trs("FrozenWavePrint")), patientManager.getPatientInfo());

    case TrendPage:
        d_ptr->curPageType = WaveScalePage;
        return createTrendPage(d_ptr->trendData, true, QString(trs("FreezeTime")));

    case WaveScalePage:
        d_ptr->curPageType = WaveSegmentPage;
        if (d_ptr->freezeWaveInfos.size() > 0)
        {
            return createWaveScalePage(d_ptr->waveSegInfos, recorderManager.getPrintSpeed());
        }
    // fall through
    case WaveSegmentPage:
        if (d_ptr->loadWaveData(d_ptr->curDrawWaveSegment))
        {
            RecordPage *page;
            page = createWaveSegments(&d_ptr->waveSegInfos, d_ptr->curDrawWaveSegment++,
                                      recorderManager.getPrintSpeed());

            d_ptr->checkAndDrawTimestamp(page);
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

