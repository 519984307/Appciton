#include <unistd.h>
#include <assert.h>
#include "Debug.h"
#include "NIBPDataStorage.h"


#define DATA_NR 100
static NIBPDataStoreItem initTestData(1236548752, 120, 85, 90, 50);

static void selfPlus(NIBPDataStoreItem &item)
{
    item.t++;
    item.sys++;
    item.dia++;
    item.map++;
    item.pr++;
    item.checkSum = item.calcSum();
}

static void produceData(void)
{
    NIBPDataStoreItem testData = initTestData;
    for (int i = 0; i < DATA_NR; i++)
    {
        nibpDataStorage.addData(testData);
        selfPlus(testData);
    }
}

static void assertData(void)
{
    assert(nibpDataStorage.getTotalItems() == DATA_NR);

    NIBPDataStoreItem testData = initTestData;
    NIBPDataStoreItem current;
    for (int i = 0; i < DATA_NR; i++)
    {
        nibpDataStorage.readData(current, i);
        assert(testData == current);
        selfPlus(testData);
    }

    assert(nibpDataStorage.readData(current, -1) == false);
    assert(nibpDataStorage.readData(current, 1250) == false);
}

int main(void)
{
    catchFatalError();

    nibpDataStorage.clearAllData();

    produceData();
    assertData();
    qDebug("All test cases are passed!");

    return 0;
}
