/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/1/7
 **/


#pragma once

typedef short TrendDataType;
typedef unsigned WaveDataType;

enum WaveFlagBit
{
    ECG_INTERNAL_FLAG_BIT = 1,                  // 内部起搏
    ECG_EXTERNAL_SOLD_FLAG_BIT = (1 << 1),      // 外部起搏(实线)
    INVALID_WAVE_FALG_BIT = (1 << 14),           // 无效波形
    ECG_WAVE_RATE_250_BIT = (1 << 15)           // 波形采样速率为250/S
};

#define InvData()     (-5000)
#define InvStr()      ("---")
#define EmptyStr()    ("")

#define UnknownData() (-8000)
#define UnknownStr()  ("???")

#define MAX_PATIENT_ID_LEN      (120 + 1)
#define MAX_PATIENT_NAME_LEN    (120 + 1)
#define MAX_WAVE_SEG_INFO_REMARK (30 + 1)

#define MAX_TIME_LEN      (32)    // 单位:秒
#define MAX_SAMPLE_RATE   (250)   // 单位:个
#define MAX_WAVEFORM_CACHE_LEN \
    (MAX_TIME_LEN * MAX_SAMPLE_RATE * sizeof(WaveDataType))    // 单位:字节

#define MAX_DEVICE_ID_LEN (11 + 1)  // 设备标识符长度
#define MAX_SERIAL_NUM_LEN (11 + 1)
