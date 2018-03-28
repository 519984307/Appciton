#include <unistd.h>
#include <assert.h>
#include "Debug.h"
#include "SPO2WaveformStorage.h"

#define DATA_NR 100
static SPO2WaveformItem initTestData;

static void initData(void)
{
    initTestData.t = 123654789;

    short value;
    for (int i = 0; i < SPO2_SAMPLE; i++)
    {
        value = rand() & 0xFFFF;
        initTestData.waveform[i] = value;
    }

    initTestData.checkSum = initTestData.calcSum();
}

static void selfPlus(SPO2WaveformItem &item)
{
    item.t++;
    for (unsigned i = 0; i < sizeof(item.waveform) / sizeof(item.waveform[0]); i++)
    {
        item.waveform[i]++;
    }
    item.checkSum = item.calcSum();
}

static void produceData(void)
{
    SPO2WaveformItem testData = initTestData;
    for (int i = 0; i < DATA_NR; i++)
    {
        spo2WaveformStorage.addData(testData);
        selfPlus(testData);
    }
}

static void assertData(void)
{
    assert(spo2WaveformStorage.getTotalItems() == DATA_NR);

    SPO2WaveformItem testData = initTestData;
    SPO2WaveformItem current;
    for (int i = 0; i < DATA_NR; i++)
    {
        spo2WaveformStorage.readData(current, i);
        assert(testData == current);
        selfPlus(testData);
    }

    assert(spo2WaveformStorage.readData(current, -1) == false);
    assert(spo2WaveformStorage.readData(current, 1250) == false);
}

int main(void)
{
    catchFatalError();
    initData();

    spo2WaveformStorage.clearAllData();

    produceData();
    assertData();
    qDebug("All test cases in TestSPO2WaveformStorage.cpp are passed!");

    return 0;
}
