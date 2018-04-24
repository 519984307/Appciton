#pragma once

/**************************************************************************************************
 * 压力标名。
 *************************************************************************************************/
enum PressureDesigner_d
{
    ART_D,
    PA_D,
    AO_D,
    UAP_D,
    BAP_D,
    FAP_D,
    CVP_D,
    LAP_D,
    RAP_D,
    ICP_D,
    UVP_D,
    LV_D,
    P1_D,
    P2_D,
    P3_D,
    P4_D,
    PD_NR
};

/**************************************************************************************************
 * 波形标尺。
 *************************************************************************************************/
enum WaveformRuler_d
{
    WAVEFORM_PULER_AUTO,
    WAVEFORM_PULER_MANUAL,
    WAVEFORM_PULER_A,
    WAVEFORM_PULER_B,
    WAVEFORM_PULER_C,
    WAVEFORM_PULER_D,
    WAVEFORM_PULER_E,
    WAVEFORM_PULER_F,
    WAVEFORM_PULER_G,
    WAVEFORM_PULER_H,
    WAVEFORM_PULER_I,
    WAVEFORM_PULER_J,
    WAVEFORM_PULER_K,
    WAVEFORM_PULER_L,
    WAVEFORM_PULER_M,
    WAVEFORM_PULER_NR
};

/**************************************************************************************************
 * 波形速度。
 *************************************************************************************************/
enum SweepSpeed_d
{
    SWEEP_SPEED_A,
    SWEEP_SPEED_B,
    SWEEP_SPEED_C,
    SWEEP_SPEED_D,
    SWEEP_SPEED_NR
};

/**************************************************************************************************
 * 滤波频率。
 *************************************************************************************************/
enum FrequencyFiltering_d
{
    FREQUENCY_FILTERING_A,
    FREQUENCY_FILTERING_B,
    FREQUENCY_FILTERING_C,
    FREQUENCY_FILTERING_D,
    FREQUENCY_FILTERING_NR
};

/**************************************************************************************************
 *PPV来源。
 *************************************************************************************************/
enum PPVSource_d
{
    PPV_SOURCE_IBP,
    PPV_SOURCE_ART,
    PPV_SOURCE_AO,
    PPV_SOURCE_FAP,
    PPV_SOURCE_BAP,
    PPV_SOURCE_PA,
    PPV_SOURCE_UAP,
    PPV_SOURCE_P1,
    PPV_SOURCE_P2,
    PPV_SOURCE_P3,
    PPV_SOURCE_P4,
    PPV_SOURCE_LV,
    PPV_SOURCE_AUTO,
    PPV_SOURCE_NR
};

