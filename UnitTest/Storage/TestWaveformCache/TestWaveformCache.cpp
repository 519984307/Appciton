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

#define READ_REALTIME_TIME       (0)
#define READ_REALTIME_NUM        (1)

TestWaveformCache::TestWaveformCache()
{
}

void TestWaveformCache::init()
{
}

void TestWaveformCache::cleanup()
{
    waveformCache.clear();
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

    QTest::newRow("register source spo2")
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
    waveformCache.getInstance().getRange(id, &min, &max);
    int line;
    line = waveformCache.getInstance().getBaseline(id);
    QString title;
    title = waveformCache.getInstance().getTitle(id);

    QCOMPARE(waveformCache.getInstance().getSampleRate(id), rateResult);
    QCOMPARE(min, minResult);
    QCOMPARE(max, maxResult);
    QCOMPARE(line, baselineResult);
    QCOMPARE(title, titleResult);
}

void TestWaveformCache::testChannelDuration()
{
    QCOMPARE(waveformCache.channelDuration(), static_cast<unsigned>(120));
}

Q_DECLARE_METATYPE(WaveDataType)
void TestWaveformCache::testReadStorageChannel_data()
{
    QTest::addColumn<int>("rate");
    QTest::addColumn<int>("time");
    QTest::addColumn<WaveDataType>("data");
    QTest::addColumn<WaveDataType>("dataResult");

    QTest::newRow("rate 125, time 1s")
            << 125
            << 1
            << static_cast<WaveDataType>(0x32)
            << static_cast<WaveDataType>(0x32);

    QTest::newRow("rate 250, time 10s")
            << 250
            << 10
            << static_cast<WaveDataType>(0xffffffff)
            << static_cast<WaveDataType>(0xffffffff);

    QTest::newRow("rate 500, time 100s")
            << 500
            << 100
            << static_cast<WaveDataType>(0x12345678)
            << static_cast<WaveDataType>(0x12345678);
}

void TestWaveformCache::testReadStorageChannel()
{
    QFETCH(int, rate);
    QFETCH(int, time);
    QFETCH(WaveDataType, data);
    QFETCH(WaveDataType, dataResult);

    WaveformID id = WAVE_AA1;
    QString title = "aa1";
    waveformCache.registerSource(id, rate, 0, 255, title, 0);

    for (int i = 0; i < waveformCache.getSampleRate(id) * time; i++)
    {
        waveformCache.addData(id, data);
    }
    QVector<WaveDataType> wavedata;
    wavedata.resize(waveformCache.getSampleRate(id) * time);
    waveformCache.readStorageChannel(id, wavedata.data(), time);

    for (int i = 0; i < waveformCache.getSampleRate(id) * time; i++)
    {
        QCOMPARE(wavedata.at(i), dataResult);
    }
}

void TestWaveformCache::testReadRealtimeChannel_data()
{
    QTest::addColumn<int>("rate");
    QTest::addColumn<int>("time");
    QTest::addColumn<WaveDataType>("data");
    QTest::addColumn<bool>("stopRealtime");
    QTest::addColumn<int>("readMethod");        // 调用不同的构造函数
    QTest::addColumn<WaveDataType>("dataResult");

    QTest::newRow("rate 125, time 1s, start realtime, time funtion")
            << 125
            << 1
            << static_cast<WaveDataType>(0x32)
            << false
            << 0
            << static_cast<WaveDataType>(0x32);

    QTest::newRow("rate 250, time 10s, start realtime, time funtion")
            << 250
            << 8
            << static_cast<WaveDataType>(0xffffffff)
            << false
            << 0
            << static_cast<WaveDataType>(0xffffffff);

    QTest::newRow("rate 500, time 100s, stop realtime, time funtion")
            << 500
            << 5
            << static_cast<WaveDataType>(0x12345678)
            << true
            << 0
            << static_cast<WaveDataType>(0);

    QTest::newRow("rate 500, time 100s, stop realtime, num funtion")
            << 500
            << 3
            << static_cast<WaveDataType>(0x87654321)
            << true
            << 1
            << static_cast<WaveDataType>(0);
}

void TestWaveformCache::testReadRealtimeChannel()
{
    QFETCH(int, rate);
    QFETCH(int, time);
    QFETCH(WaveDataType, data);
    QFETCH(bool, stopRealtime);
    QFETCH(int, readMethod);
    QFETCH(WaveDataType, dataResult);

    WaveformID id = WAVE_AA1;
    QString title = "aa1";
    waveformCache.registerSource(id, rate, 0, 255, title, 0);
    waveformCache.startRealtimeChannel();
    if (stopRealtime)
    {
        waveformCache.stopRealtimeChannel();
    }

    for (int i = 0; i < waveformCache.getSampleRate(id) * time; i++)
    {
        waveformCache.addData(id, data);
    }
    QVector<WaveDataType> wavedata;
    wavedata.resize(waveformCache.getSampleRate(id) * time);
    if (readMethod == READ_REALTIME_TIME)
    {
        waveformCache.readRealtimeChannel(id, wavedata.data(), time);
    }
    else if (readMethod == READ_REALTIME_NUM)
    {
        waveformCache.readRealtimeChannel(id, time * rate, wavedata.data());
    }

    for (int i = 0; i < waveformCache.getSampleRate(id) * time; i++)
    {
        QCOMPARE(wavedata.at(i), dataResult);
    }
}

void TestWaveformCache::testRegisterWaveformRecorder_data()
{
    QTest::addColumn<WaveDataType>("data");
    QTest::addColumn<bool>("unRegister");
    QTest::addColumn<WaveDataType>("dataResult");

    QTest::newRow("register recorder")
            << static_cast<WaveDataType>(0x134)
            << false
            << static_cast<WaveDataType>(0x134);

    QTest::newRow("register recorder 0xffffffff")
            << static_cast<WaveDataType>(0xffffffff)
            << false
            << static_cast<WaveDataType>(0xffffffff);

    QTest::newRow("unregister recorder")
            << static_cast<WaveDataType>(0x134)
            << true
            << static_cast<WaveDataType>(0x40000000);

    QTest::newRow("unregister recorder 0xffffffff")
            << static_cast<WaveDataType>(0xffffffff)
            << true
            << static_cast<WaveDataType>(0x40000000);
}

static void waveCacheDurationIncrease(WaveformID id, void *obj)
{
    Q_UNUSED(id)
    Q_UNUSED(obj)
}

static void waveCacheCompleteCallback(WaveformID id, void *obj)
{
    Q_UNUSED(id)
    Q_UNUSED(obj)
}

void TestWaveformCache::testRegisterWaveformRecorder()
{
    QFETCH(WaveDataType, data);
    QFETCH(bool, unRegister);
    QFETCH(WaveDataType, dataResult);

    WaveformRecorder recorder;
    WaveDataType waveData[250] = {0};
    recorder.buf = waveData;
    recorder.curRecWaveNum = 0;
    recorder.totalRecWaveNum = 250;
    recorder.recObj = this;
    recorder.recordCompleteCallback = waveCacheCompleteCallback;
    recorder.recordDurationIncreaseCallback = waveCacheDurationIncrease;
    QString title = "aa1";
    waveformCache.registerSource(WAVE_AA1, 250, 0, 255, title, 0);
    waveformCache.registerWaveformRecorder(WAVE_AA1, recorder);
    if (unRegister)
    {
        waveformCache.unRegisterWaveformRecorder(WAVE_AA1, recorder.recObj);
    }

    for (int i = 0; i < waveformCache.getSampleRate(WAVE_AA1); i++)
    {
        waveformCache.addData(WAVE_AA1, data);
    }

    for (int i = 0; i < waveformCache.getSampleRate(WAVE_AA1); i++)
    {
        QCOMPARE(waveData[i], dataResult);
    }
}
