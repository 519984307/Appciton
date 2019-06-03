/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/4/12
 **/

#pragma once

#define DATA_STORE_PATH ("/usr/local/nPM/data/")
#define DATA_PRINT_PATH ("/usr/local/nPM/data/print/")

#define PARAM_DATA_FILE_NAME ("/Trend")
#define SUMMARY_DATA_FILE_NAME ("/SummaryData")
#define ECG12L_FILE_NAME ("/12lead")
#define TREND_DATA_FILE_NAME ("/TrendData")
#define EVENT_DATA_FILE_NAME ("/EventData")
#define PATIENT_INFO_FILE_NAME ("/PatientInfo")

#define ECG12L_PDFDIR_PATH ("/tmp/pdf")
#define ECG12L_PDF_PATH ("/tmp/pdf/")
#define ECG12L_EMAIL_NAME ("/usr/local/nPM/bin/")
#define FILE_DATA_SUFFIX  (".data")
#define FILE_ENTRY_SUFFIX (".entry")

#define MAX_DATA_SIZE (1 << 31)  // 最大存储空间2G
#define SIGNAL_RESCUE_MAX_DATA_SIZE (1 << 29) // 单个营救的最大存储空间512M
