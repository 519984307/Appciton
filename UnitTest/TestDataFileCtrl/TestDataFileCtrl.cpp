#include <unistd.h>
#include "Debug.h"
#include "DataFileCtrl.h"

int main(void)
{
    catchFatalError();

    DataFileCtrl data_file("testDataFile", 64);

    char array[16];
    int cnt = 100;
    while(cnt--)
    {
        for (unsigned i = 0; i < sizeof(array); i++)
        {
            array[i] = rand() % ('Z'-'A' + 1) + 'A';
        }

        data_file.apendData(array, sizeof(array));
        usleep(500);
    }

    qDebug("All test cases in TestDataFile.cpp are passed!");
    return 1;
}
