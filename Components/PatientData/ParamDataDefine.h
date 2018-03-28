#pragma once
#include "BaseDefine.h"
#include "ParamDefine.h"

//参数单元
struct ParamItem
{
    ParamItem()
    {
        paramID = 0;
        alarmFlag = 0;
        value = InvData();
    }

    unsigned calcSum()
    {
        return (unsigned)(paramID + alarmFlag + value);
    }

    unsigned char paramID;
    char alarmFlag;
    TrendDataType value;
}__attribute__((packed));

// 存放趋势参数数据的结构体，不包括NIBP，NIBP独立存储。
struct ParamDataItem
{
    ParamDataItem()
    {
        t = 0;
        isAlarm = 0;
        isNibpShowMeasureValue = 0;
        co2Baro = 0;
        dataLen = 0;
        checkSum = 0;
        isApneaAlarm = 0;
        hrSource = 0;
        brSource = 0;
        isFico2Display = 0;
    }

    unsigned calcSum(void)
    {
        unsigned sum = 0;

        sum += t;
        sum += isAlarm;
        sum += isNibpShowMeasureValue;
        sum += dataLen;
        sum += co2Baro;
        sum += isApneaAlarm;
        sum += hrSource;
        sum += brSource;
        sum += isFico2Display;

        return sum;
    }

    void makeCheckSum()
    {
        checkSum = calcSum();
    }

    bool isValid(void)
    {
        return (calcSum() == checkSum);
    }

    unsigned t;
    unsigned char isAlarm;            //发生报警而增加的数据
    unsigned char isNibpShowMeasureValue;    //Nibp measurement finished need to show value, nibp alarm also need to show value
    unsigned char isApneaAlarm;      //Apnea alarm active or not, 0: not active, 1: active
    unsigned char hrSource;           //hr source, 0: ecg, 1: spo2
    unsigned char brSource;           //br source, 0: co2, 1: Impedence respiration
    unsigned char isFico2Display;    //showing fico2 or not, 0: no displayed, 1: display
    short dataLen;
    short co2Baro;
    unsigned checkSum;
}__attribute__((packed));

//数据描述，指出文件中各类型的数据的数量
struct ParamDataDescription
{
    ParamDataDescription()
    {
        startTime = 0;
        memset(deviceID, 0, sizeof(deviceID));
        memset(patientName, 0, sizeof(patientName));
        memset(PatientID, 0, sizeof(PatientID));
        memset(fdFileName, 0, sizeof(fdFileName));
        moduleConfig = 0;
        checkSum = 0;
    }

    unsigned sum()
    {
        unsigned sum = 0;
        sum += startTime;
        sum += moduleConfig;

        for (unsigned i = 0; i < sizeof(deviceID); ++i)
        {
            sum += (unsigned)deviceID[i];
        }

        for (unsigned i = 0; i < sizeof(patientName); ++i)
        {
            sum += (unsigned)patientName[i];
        }

        for (unsigned i = 0; i < sizeof(PatientID); ++i)
        {
            sum += (unsigned)PatientID[i];
        }

        for (unsigned i = 0; i < sizeof(serialNum); ++i)
        {
            sum += (unsigned)serialNum[i];
        }

        for (unsigned i = 0; i < sizeof(fdFileName); ++i)
        {
            sum += (unsigned)fdFileName[i];
        }

        return sum;
    }

    unsigned startTime;
    short moduleConfig;                                       // module config
    char deviceID[MAX_DEVICE_ID_LEN];                         // 设备标识符
    char patientName[MAX_PATIENT_NAME_LEN];                     // 病人姓名
    char PatientID[MAX_PATIENT_ID_LEN];                         // 设备标识符
    char serialNum[MAX_SERIAL_NUM_LEN];                        // 设备序列号
    char fdFileName[50];                       // fulldisclosure文件名
    unsigned checkSum;
}__attribute__((packed));
