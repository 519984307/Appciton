#pragma once
/**************************************************************************************************
* 校准模式压力
*************************************************************************************************/
enum CodeMarkersNum
{
    CodeMarker1,
    CodeMarker2,
    CodeMarker3,
    CodeMarker4,
    CodeMarker5,
    CodeMarker6,
    CodeMarker7,
    CodeMarker8,
    CodeMarker9,
    CodeMarker10,
    CodeMarker11,
    CodeMarker12,
    CodeMarker13,
    CodeMarker14,
    CodeMarker15,
    CodeMarker16,
    CodeMarker17,
    CodeMarker18,
    CodeMarker19,
    CodeMarker20,
    CodeMarker21,
    CodeMarker22,
    CodeMarker23,
    CodeMarker24,
    CodeMarker25,
    CodeMarker26,
    CodeMarker27,
    CodeMarker28,
    CodeMarker29,
    CodeMarker30,
    CodeMarker31,
    CodeMarker32,
};

// 将定义的枚举转换成符号。
class CodeMarkerList
{
public:
    static const char *CodeMarkerEnglish(CodeMarkersNum index)
    {
        static const char *symbol[30] =
        {
            "CPR",
            "EPI",
            "Atrop",
            "DOPA",
            "Bicarb",
            "Aspirin",
            "Oxygen",
            "IV",
            "Morphine",
            "Valium",
            "B-block",
            "LIDO",
            "Mag Sulf",
            "Thrombo",
            "Heparin",
            "Procain",
            "Cordar",
            "Thiamine",
            "Intubate",
            "Narcan",
            "Atrovent",
            "Adenosin",
            "Fentanyl",
            "Digoxin",
            "Vasopr",
            "Dextrose",
            "Paralytic",
            "Nitro",
            "Lasix",
            "Calcium",
        };
        return symbol[index];
    }

    static const char *CodeMarkerChinese(CodeMarkersNum index)
    {
        static const char *symbol[32] =
        {
            "CPR",
            "EPI",
            "Atrop",
            "DOPA",
            "PHEN",
            "Bicarb",
            "Aspirin",
            "Oxygen",
            "IV",
            "Morphine",
            "Valium",
            "B-block",
            "LIDO",
            "Mag Sulf",
            "Thrombo",
            "Sedation",
            "Heparin",
            "Procain",
            "Amio",
            "Thiamine",
            "Dilantin",
            "Intubate",
            "Narcan",
            "Atrovent",
            "Adenosin",
            "Fentanyl",
            "Digoxin",
            "Vasopr",
            "Dextrose",
            "Paralytic",
            "Albuterol",
            "Nitro",
        };
        return symbol[index];
    }
};
