#pragma once

#include "StorageManager.h"
#include "BaseDefine.h"
#include "ECGDefine.h"

#define ECG12L_WAVE_TIME (10)

#define ECG_WAVE_RATE_250    (250)
#define ECG_WAVE_RATE_500    (500)
#define ECG_WAVE_DATA_NUM    (ECG_WAVE_RATE_500 * ECG12L_WAVE_TIME)

class ECG12LDataStoragePrivate;
class ECG12LDataStorage : public StorageManager
{
public:
    struct ECG12LCommonInfo
    {
        unsigned int calSum()
        {
            unsigned int sum = 0;

            sum += (timeStamp + rescueStartTime + ms);
            sum += (format + bandWidth + patientSex + patientAge);
            sum += notchFilter + hr + notchFilter + pacerMarkersOn;

            int nameCount = sizeof(patientName) / sizeof(patientName[0]);
            for (int i = 0; i < nameCount; ++i)
            {
                sum += patientName[i];
            }

            int idCount = sizeof(patientID) / sizeof(patientID[0]);
            for (int i = 0; i < idCount; ++i)
            {
                sum += patientID[i];
            }

            int deviceIDCount = sizeof(deviceID) / sizeof(deviceID[0]);
            for (int i = 0; i < deviceIDCount; ++i)
            {
                sum += deviceID[i];
            }

            int serialsNumCount = sizeof(serialsNum) / sizeof(serialsNum[0]);
            for (int i = 0; i < serialsNumCount; ++i)
            {
                sum += serialsNum[i];
            }

            int fdCount = sizeof(fulldisclosureName) / sizeof(fulldisclosureName[0]);
            for (int i = 0; i < fdCount; ++i)
            {
                sum += fulldisclosureName[i];
            }

            return sum;
        }

        unsigned int timeStamp;
        unsigned rescueStartTime;
        unsigned short ms;
        short hr;
        unsigned char format;
        unsigned char bandWidth;
        unsigned char gain;
        unsigned char notchFilter;
        unsigned char pacerMarkersOn;
        unsigned char patientName[MAX_PATIENT_NAME_LEN];
        unsigned char patientID[MAX_PATIENT_ID_LEN];
        unsigned char patientSex;
        unsigned char patientAge;
        unsigned char deviceID[MAX_DEVICE_ID_LEN];
        unsigned char serialsNum[MAX_SERIAL_NUM_LEN];
        unsigned char fulldisclosureName[35];
    }__attribute__((packed));

    struct ECG12LeadData
    {
        ECG12LeadData()
        {
        }
        unsigned int calSum()
        {
            unsigned int sum = 0;

            sum += commonInfo.calSum();

            int dataLen = ECG_WAVE_DATA_NUM;
            for (int i = 0; i <= ECG_LEAD_V6; ++i)
            {
                for (int j = 0; j < dataLen; ++j)
                {
                    sum += wave[i][j];
                }
            }

            return sum;
        }

        ECG12LCommonInfo commonInfo;
        WaveDataType wave[ECG_LEAD_V6 + 1][ECG_WAVE_DATA_NUM];
        WaveDataType checkSum;
    }__attribute__((packed));

    static ECG12LDataStorage &construction()
    {
        if(_selfObj == NULL)
        {
            _selfObj = new ECG12LDataStorage();
        }

        return *_selfObj;
    }

    static ECG12LDataStorage *_selfObj;

    ~ECG12LDataStorage();

    // 添加一个12L
    bool add12LData(ECG12LeadData *data);

    // 获取Common信息
    void getCommonInfo(ECG12LCommonInfo &commonInfo);

    // 循环运行
    void run();

    //获取打印偏移
    //void getPrintPath(QStringList &list);
private:
    Q_DECLARE_PRIVATE(ECG12LDataStorage)
    quint32 _identifier;
    ECG12LDataStorage();
    //ECG12LeadWaveGainList _waveGainList;
    virtual void createDir();
};

#define ecg12LDataStorage (ECG12LDataStorage::construction())
#define deleteecg12LDataStorage() (delete ECG12LDataStorage::_selfObj)
