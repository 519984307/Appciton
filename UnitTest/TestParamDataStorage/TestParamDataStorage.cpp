#include <unistd.h>
#include <assert.h>
#include "Debug.h"
#include "ParamDataStorage.h"

#define DATA_NR 100
static ParamDataStoreItem initTestData(123654851, 50, 12, 85, 10, 0, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111);

static void selfPlus(ParamDataStoreItem &item)
{
    item.t++;
    item.hr++;
    item.etco2++;
    item.fico2++;
    item.rr++;
    item.spo2++;
    item.staVF++;
    item.staVL++;
    item.staVR++;
    item.stI++;
    item.stII++;
    item.stIII++;
    item.staVF++;
    item.staVL++;
    item.staVR++;
    item.stV1++;
    item.stV2++;
    item.stV3++;
    item.stV4++;
    item.stV5++;
    item.stV6++;
    item.checkSum = item.calcSum();
}

static void produceData(void)
{
    ParamDataStoreItem testData = initTestData;
    for (int i = 0; i < DATA_NR; i++)
    {
        paramDataStorage.addData(testData);
        selfPlus(testData);
    }
}

static void assertData(void)
{
    assert(paramDataStorage.getTotalItems() == DATA_NR);

    ParamDataStoreItem testData = initTestData;
    ParamDataStoreItem current;
    for (int i = 0; i < DATA_NR; i++)
    {
        paramDataStorage.readData(current, i);
        assert(testData == current);
        selfPlus(testData);
    }

    assert(paramDataStorage.readData(current, -1) == false);
    assert(paramDataStorage.readData(current, 1250) == false);
}

int main(void)
{
    catchFatalError();

    paramDataStorage.clearAllData();

    produceData();
    assertData();
    qDebug("All test cases in TestParamDataStorage.cpp are passed!");

    return 0;
}
