/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/12
 **/

#include "TestWaveformCache.h"

TestWaveformCache::TestWaveformCache()
{
}

void TestWaveformCache::init()
{
}

void TestWaveformCache::cleanup()
{
}

Q_DECLARE_METATYPE(WaveformID)
void TestWaveformCache::testRegisterSource_data()
{
    QTest::addColumn<WaveformID>("id");
    QTest::addColumn<int>("rate");
    QTest::addColumn<int>("minValue");
    QTest::addColumn<int>("maxValue");
    QTest::addColumn<QString>("waveTitle");
    QTest::addColumn<int>("baseline");
    QTest::addColumn<int>("rateResult");
    QTest::addColumn<int>("minResult");
    QTest::addColumn<int>("maxResult");
    QTest::addColumn<QString>("titleResult");
    QTest::addColumn<int>("baselineResult");

    QTest::newRow("register source ECG_1")
            << WAVE_ECG_I
            << 250
            << -408
            << 408
            << "I"
            << 0
            << 250
            << -408
            << 408
            << "I"
            << 0;

    QTest::newRow("register source Resp")
            << WAVE_RESP
            << 125
            << -0x4000
            << 0x3FFF
            << "Resp"
            << 0
            << 125
            << -0x4000
            << 0x3FFF
            << "Resp"
            << 0;

    QTest::newRow("register source empty")
            << WAVE_SPO2
            << 125
            << 0
            << 255
            << "SPO2"
            << 0
            << 125
            << 0
            << 255
            << "SPO2"
            << 0;

    QTest::newRow("register source empty")
            << WAVE_NR
            << 0
            << 0
            << 0
            << ""
            << 0
            << 0
            << 0
            << 0
            << ""
            << 0;
}

void TestWaveformCache::testRegisterSource()
{
    QFETCH(WaveformID, id);
    QFETCH(int, rate);
    QFETCH(int, minValue);
    QFETCH(int, maxValue);
    QFETCH(QString, waveTitle);
    QFETCH(int, baseline);
    QFETCH(int, rateResult);
    QFETCH(int, minResult);
    QFETCH(int, maxResult);
    QFETCH(QString, titleResult);
    QFETCH(int, baselineResult);

    waveformCache.getInstance().registerSource(id, rate, minValue, maxValue, waveTitle, baseline);

    int min;
    int max;
    waveformCache.getInstance().getRange(id, min, max);
    int line;
    waveformCache.getInstance().getBaseline(id, line);
    QString title;
    waveformCache.getInstance().getTitle(id, title);

    QCOMPARE(waveformCache.getInstance().getSampleRate(id), rateResult);
    QCOMPARE(min, minResult);
    QCOMPARE(max, maxResult);
    QCOMPARE(line, baselineResult);
    QCOMPARE(title, titleResult);
}
