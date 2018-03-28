#include "SummaryDefine.h"

const char * getSummaryName(SummaryType type)
{
    static const char * summaryName[SUMMARY_NR] =
    {
        "ECGRhythm",
        "ECGForeAnalysis",
        "ShockDelivery",
        "CheckPatient",
        "PacerStartup",
        "PhyAlarm",
        "PrinterAction",
        "CodeMarkerSnapshot",
        "NIBPReading",
        "DiagnosticECG",
        "Monitor",
        "PrinterAction",
        "PrinterAction",
        "PrinterAction",
    };

    if(type >= SUMMARY_NR)
    {
        return "";
    }
    return summaryName[type];
}
