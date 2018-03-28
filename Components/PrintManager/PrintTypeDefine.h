#pragma once
#include <cstddef>

// 打印类型定义。
enum PrintType
{
    PRINT_TYPE_NONE,
    PRINT_TYPE_CONTINUOUS,                       // 连续打印
    PRINT_TYPE_REVIEW,                           // 存储打印
    PRINT_TYPE_TRIGGER_CODE_MARKER,              // CODE MARKER 触发打印
    PRINT_TYPE_TRIGGER_NIBP,                     // NIBP trigger print
    PRINT_TYPE_TRIGGER_PACER_STARTUP,            // 进入Pacer 触发打印
    PRINT_TYPE_TRIGGER_ECG_RHYTHM,               // ECG节律触发打印
    PRINT_TYPE_TRIGGER_FOREGROUND_ECG_ANALYSIS,  // 前景分析触发打印
    PRINT_TYPE_TRIGGER_SHOCK_DELIVERY,           // 电击触发打印
    PRINT_TYPE_TRIGGER_CHECK_PATIENT,            // 检查病人触发打印
    PRINT_TYPE_TRIGGER_PHY_ALARM,                // 生理报警触发打印
    PRINT_TYPE_TRIGGER_DIAG_ECG,                 // 诊断ECG触发打印
    PRINT_TYPE_TRIGGER_12L_ECG,                  // 12导快照触发打印
    PRINT_TYPE_TRIGGER_30J_SELFTEST,             // 30J自测触发打印
    PRINT_TYPE_NR
};

// 打印优先级
enum PrintPriority
{
    PRINT_PRIORITY_LEVEL_NONE,
    PRINT_PRIORITY_LEVEL_1,
    PRINT_PRIORITY_LEVEL_2,
    PRINT_PRIORITY_LEVEL_3,
    PRINT_PRIORITY_LEVEL_4,
    PRINT_PRIORITY_LEVEL_NR
};

// 这里定义所有的打印类型。
enum PrintLayoutID
{
    PRINT_LAYOUT_ID_NONE,
    PRINT_LAYOUT_ID_CONTINUOUS,               // 连续打印。
    PRINT_LAYOUT_ID_TREND_REPORT,             // 趋势数据
    PRINT_LAYOUT_ID_SUMMARY_TRIGGER_REPORT,   // summary触发打印
    PRINT_LAYOUT_ID_SUMMARY_REVIEW_REPORT,    // summary回顾打印
    PRINT_LAYOUT_ID_SUPERVISOR_CONFIG,        // 超级配置信息打印
    PRINT_LAYOUY_ID_12L_ECG,                  // 12导联打印
    PRINT_LAYOUT_ID_OTHER_TRIGGER_REPORT,     // 其它类型的触发打印
    PRINT_LAYOUT_ID_NR
};
