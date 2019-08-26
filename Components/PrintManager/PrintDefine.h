/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/19
 **/



#pragma once
#include <QString>

/**
 * @brief The PrintTime enum
 */
enum PrintTime
{
    PRINT_TIME_CONTINOUS = 0,
    PRINT_TIME_EIGHT_SEC,
    PRINT_TIME_NR
};

// 打印速度设置
enum PrintSpeed
{
    PRINT_SPEED_125 = 0,  // 12.5 mm/s
    PRINT_SPEED_250 = 1,  // 25 mm/s
    PRINT_SPEED_500 = 2,  // 50 mm/s
    PRINT_SPEED_NR
};

// 打印波形数量
enum PrintWaveformNum
{
    PRINT_WAVEFORM_NUM_1 = 1,
    PRINT_WAVEFORM_NUM_2,
    PRINT_WAVEFORM_NUM_3,
    PRINT_WAVEFORM_NUM_4,
};

// 打印触发开关。
enum PrintTrigger
{
    PRINT_TRIGER_OFF,
    PRINT_TRIGER_ON,
    PRINT_TRIGER_NR
};

// 12导快照打印格式。
enum Print12LeadSnapshotFormat
{
    PRINT_12LEAD_SNAPSHOT_STAND,
    PRINT_12LEAD_SNAPSHOT_CABRELA,
    PRINT_12LEAD_SNAPSHOT_NR
};

// 12导PDF打印格式。
enum Print12LeadPDFFormat
{
    PRINT_12LEAD_PDF_3X4_STAND = 0,
    PRINT_12LEAD_PDF_2X6_STAND,
    PRINT_12LEAD_PDF_3X4_CABRELA,
    PRINT_12LEAD_PDF_2X6_CABRELA,
    PRINT_12LEAD_PDF_NR
};


/**************************************************************************************************
 * OneShot事件定义，
 *************************************************************************************************/
enum PrintOneShotType
{
    PRINT_ONESHOT_ALARM_COMMUNICATION_STOP,     // 通信中断
    PRINT_ONESHOT_ALARM_OUT_OF_PAPER,   // 缺纸
    PRINT_ONESHOT_ALARM_OVER_HEATING,   // 过热
    PRINT_ONESHOT_ALARM_FAULT,          // 打印机故障
    PRINT_ONESHOT_NR,
};

// 将定义的枚举转换成符号。
class PrintSymbol
{
public:
    static const QString &convert(PrintTrigger index)
    {
        static const QString symbol[PRINT_TRIGER_NR] =
        {
            "Off", "On"
        };

        return symbol[index];
    }

    static const QString &convert(PrintSpeed index)
    {
        static const QString symbol[PRINT_SPEED_NR] =
        {
            "12.5 mm/s", "25 mm/s", "50 mm/s"
        };

        return symbol[index];
    }

    static int convertToPixel(PrintSpeed index)
    {
        static int symbol[PRINT_SPEED_NR] =
        {
            100, 200, 400
        };

        return symbol[index];
    }

    static const QString &convert(Print12LeadSnapshotFormat index)
    {
        static const QString symbol[PRINT_12LEAD_SNAPSHOT_NR] =
        {
            "Standard", "Cabrera"
        };

        return symbol[index];
    }

    static const QString &convert(Print12LeadPDFFormat index)
    {
        static const QString symbol[PRINT_12LEAD_PDF_NR] =
        {
            "4X3 Standard", "2X6 Standard", "4X3 Cabrera", "2X6 Cabrera",
        };

        return symbol[index];
    }

    static const char *convert(PrintOneShotType index)
    {
        static const char *symbol[PRINT_ONESHOT_NR] =
        {
            "PrinterCommunicationStop", // 打印机通信中断
            "PrinterOutOfPaper",    // 打印机缺纸
            "PrinterOverHeating",   // 打印头过热
            "PrinterFault",         // 打印机故障
        };
        return symbol[index];
    }

    static const QString &convert(PrintTime index)
    {
        static const QString symbol[PRINT_TIME_NR] =
        {
            "Continous", "_8sec"
        };

        return symbol[index];
    }
};
