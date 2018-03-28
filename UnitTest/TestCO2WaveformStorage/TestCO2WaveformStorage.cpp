#include <unistd.h>
#include <assert.h>
#include "Debug.h"
#include "CO2WaveformStorage.h"

#define DATA_NR 100
static CO2WaveformItem initTestData;

static void initData(void)
{
    initTestData.t = 123654789;

    short value;
    for (int i = 0; i < CO2_SAMPLE; i++)
    {
        value = rand() & 0xFFFF;
        initTestData.waveform[i] = value;
    }

    initTestData.checkSum = initTestData.calcSum();
}

static void selfPlus(CO2WaveformItem &item)
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
    CO2WaveformItem testData = initTestData;
    for (int i = 0; i < DATA_NR; i++)
    {
        co2WaveformStorage.addData(testData);
        selfPlus(testData);
    }
}

static void assertData(void)
{
    assert(co2WaveformStorage.getTotalItems() == DATA_NR);

    CO2WaveformItem testData = initTestData;
    CO2WaveformItem current;
    for (int i = 0; i < DATA_NR; i++)
    {
        co2WaveformStorage.readData(current, i);
        assert(testData == current);
        selfPlus(testData);
    }

    assert(co2WaveformStorage.readData(current, -1) == false);
    assert(co2WaveformStorage.readData(current, 1250) == false);
}

int main(void)
{
    catchFatalError();
    initData();

    co2WaveformStorage.clearAllData();

    produceData();
    assertData();
    qDebug("All test cases in TestCO2WaveformStorage.cpp are passed!");

    return 0;
}
