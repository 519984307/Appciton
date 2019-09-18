/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/29
 **/



#pragma once
#include <QString>
#include "ECGDefine.h"

// 将定义的枚举转换成符号。
class ECGSymbol
{
public:
    static const char *convert(HRSourceType index)
    {
        static const char *symbol[HR_SOURCE_NR] =
        {
            "HRSourceECG",
            "HRSourceSPO2",
            "HRSourceIBP",
            "HRSourceAuto",
        };

        return symbol[index];
    }

    static const char *convert(ECGLeadMode index)
    {
        static const char *symbol[ECG_LEAD_MODE_NR] =
        {
            "ECG3-Lead", "ECG5-Lead", "ECG12-Lead"
        };

        return symbol[index];
    }

    static const char *convert(ECGDisplayMode index)
    {
        static const char *symbol[ECG_DISPLAY_NR] =
        {
            "ECGNormal", "ECG12-Lead-Full"
        };

        return symbol[index];
    }

    static const char *convert(ECGLead index, ECGLeadNameConvention convention,
                               bool is12L = false, bool isCabrera = false)
    {
        if ((index == ECG_LEAD_AVR) && is12L && isCabrera)
        {
            static const char *ecg12LAVR = "-aVR";
            return ecg12LAVR;
        }
        else
        {
            int is12LInt = is12L ? 1 : 0;
            if (!is12LInt && index >= ECG_LEAD_V1)
            {
                if (convention == ECG_CONVENTION_AAMI)
                {
                    return "V";
                }
                else
                {
                    return "C";
                }
            }
            else
            {
                static const char *symbol[ECG_CONVENTION_NR][ECG_LEAD_NR] =
                {
                    {
                        "I",  "II", "III", "aVR", "aVL", "aVF",
                        "V1", "V2",  "V3", "V4",  "V5",  "V6"
                    },

                    {
                        "I",  "II", "III", "aVR", "aVL", "aVF",
                        "C1", "C2",  "C3", "C4",  "C5",  "C6"
                    }
                };
                return symbol[convention][index];
            }
        }
    }

    static const char *convert(ECGGain index)
    {
        static const char *symbol[ECG_GAIN_NR] =
        {
            "X0.125", "X0.25", "X0.5", "X1", "X2",  "X4", "Auto"
//            "0.5 cm/mV", "1.0 cm/mV", "1.5 cm/mV", "2.0 cm/mV", "3.0 cm/mV", "AUTO"
        };

        return symbol[index];
    }

    static const char *convert(ECGST index)
    {
        static const char *symbol[ECG_ST_NR] =
        {
            "I",  "II", "III", "aVR", "aVL", "aVF",
            "V1", "V2", "V3", "V4", "V5", "V6"
        };

        return symbol[index];
    }

    static const char *convert(ECGPaceMode index)
    {
        static const char *symbol[ECG_PACE_NR] =
        {
            "Off", "On"
        };
        return symbol[index];
    }

    static const char *convert(ECGAlarmSource src)
    {
        static const char *symbol[ECG_ALARM_SRC_NR] =
        {
            "HR",
            "PR",
            "Auto",
        };
        return symbol[src];
    }

    static const char *convert(ECGSweepSpeed index)
    {
        static const char *symbol[ECG_SWEEP_SPEED_NR] =
        {
            "6.25 mm/s", "12.5 mm/s", "25.0 mm/s", "50.0 mm/s"
        };

        return symbol[index];
    }

    static const char *convert(ECGBandwidth index)
    {
        // add one more item to store multiple BW string
        static const char *symbol[ECG_BANDWIDTH_NR + 1] =
        {
            "0.67-20 Hz",
            "0.67-40 Hz",
            "0.525-40 Hz",
            "0.525-150 Hz",
            "Multiple BW",
        };

        return symbol[index];
    }

    static const char *convert(ECGFilterMode mode)
    {
        static const char *symbol[ECG_FILTERMODE_NR] =
        {
            "Surgery",
            "Monitor",
            "Diagnostic",
            "ST",
        };
        return symbol[mode];
    }

    static const char *convert(ECGNotchFilter index)
    {
        static const char *symbol[ECG_NOTCH_NR] =
        {
            "Off", "50 Hz", "60 Hz", "50&60 Hz"
        };

        return symbol[index];
    }

    /**********************************************************************************************
     * 以下为报警字串转换。
     ********************************************************************************************/
    static const char *convert(ECGDupLimitAlarmType index)
    {
        static const char *symbol[ECG_DUP_LIMIT_ALARM_NR] =
        {
            "HRLow", "HRHigh", "SPO2PRLow", "SPO2PRHigh"
        };

        return symbol[index];
    }

    static const char *convert(ECGOneShotType index, ECGLeadNameConvention convention, bool is12Lead = true)
    {
        static const char *symbol[ECG_ONESHOT_NR] =
        {
            "ECGArrASYSTOLE",
            "ECGArrVFIBVTAC",
            "CheckPatient",

            "ECGLeadOff",         "ECGV1LeadOff",
            "ECGV2LeadOff",       "ECGV3LeadOff",
            "ECGV4LeadOff",       "ECGV5LeadOff",
            "ECGV6LeadOff",       "ECG12LTransfersuccess",
            "ECG12LTransferfail", "ECGOverLoad",
            "ECGCommunicationStop",
            "ECGSendCommunicationStop"
        };


        if (index >= ECG_ONESHOT_ALARM_V1_LEADOFF && index <= ECG_ONESHOT_ALARM_V6_LEADOFF)
        {
            if (convention == ECG_CONVENTION_AAMI)
            {
                if (is12Lead)
                {
                    return symbol[index];
                }
                else
                {
                    return "ECGVLeadOff";
                }
            }
            else
            {
                if (is12Lead)
                {
                    static const char *leadOff[6] =
                    {
                        "ECGC1LeadOff",
                        "ECGC2LeadOff",
                        "ECGC3LeadOff",
                        "ECGC4LeadOff",
                        "ECGC5LeadOff",
                        "ECGC6LeadOff"
                    };
                    return leadOff[index - ECG_ONESHOT_ALARM_V1_LEADOFF];
                }
                else
                {
                    return "ECGCLeadOff";
                }
            }
        }
        else
        {
            return symbol[index];
        }
    }

    static const char *convert(ECGLeadNameConvention index)
    {
        static const char *symbol[ECG_CONVENTION_NR] =
        {
            "AAMI",
            "IEC"
        };

        return symbol[index];
    }

    static const QString &convert(Display12LeadFormat index)
    {
        static const QString symbol[DISPLAY_12LEAD_NR] =
        {
            "Standard", "Cabrera"
        };

        return symbol[index];
    }
};
