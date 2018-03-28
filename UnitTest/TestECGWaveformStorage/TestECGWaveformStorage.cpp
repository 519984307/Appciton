#include <unistd.h>
#include <assert.h>
#include "Debug.h"
#include "ECGWaveformStorage.h"

#define DATA_NR 100
static ECGWaveformItem initTestData;

static void initData(void)
{
    initTestData.t = 123654789;

    int lead = ECG_I_WAVEFORM;
    for ( int i = 0; i < ECG_WAVEFORM_STORAGE_NR; i++)
    {
        initTestData.LeadName[i] = (ParamWaveformIDs)lead;
    }

    short value;
    for (int i = 0; i < ECG_SAMPLE; i++)
    {
        for ( int k = 0; k < ECG_WAVEFORM_STORAGE_NR; k++)
        {
            value = rand() & 0xFFFF;
            initTestData.waveform[i * ECG_WAVEFORM_STORAGE_NR + k] = value;
        }
    }

    initTestData.checkSum = initTestData.calcSum();
}

static void selfPlus(ECGWaveformItem &item)
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
    ECGWaveformItem testData = initTestData;
    for (int i = 0; i < DATA_NR; i++)
    {
        ecgWaveformStorage.addData(testData);
        selfPlus(testData);
    }
}

static void assertData(void)
{
    assert(ecgWaveformStorage.getTotalItems() == DATA_NR);

    ECGWaveformItem testData = initTestData;
    ECGWaveformItem current;
    for (int i = 0; i < DATA_NR; i++)
    {
        ecgWaveformStorage.readData(current, i);
        assert(testData == current);
        selfPlus(testData);
    }

    assert(ecgWaveformStorage.readData(current, -1) == false);
    assert(ecgWaveformStorage.readData(current, 1250) == false);
}

int main(void)
{
    catchFatalError();
    initData();

    ecgWaveformStorage.clearAllData();

    produceData();
    assertData();
    qDebug("All test cases in TestECGWaveformStorage.cpp are passed!");

    return 0;
}
