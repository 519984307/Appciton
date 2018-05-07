#pragma once

#define DATA_STORE_PATH ("/usr/local/nPM/data/")
#define DATA_PRINT_PATH ("/usr/local/nPM/data/print/")

#define PARAM_DATA_FILE_NAME ("/Trend")
#define SUMMARY_DATA_FILE_NAME ("/SummaryData")
#define ECG12L_FILE_NAME ("/12lead")
#define TREND_DATA_FILE_NAME ("/TrendData")
#define EVENT_DATA_FILE_NAME ("/EventData")
#define OXYCRG_DATA_FILE_NAME ("/OxyCRGData")


#define ECG12L_PDFDIR_PATH ("/tmp/pdf")
#define ECG12L_PDF_PATH ("/tmp/pdf/")
#define ECG12L_EMAIL_NAME ("/usr/local/nPM/bin/")
#define FILE_DATA_SUFFIX  (".data")
#define FILE_ENTRY_SUFFIX (".entry")

#define MAX_DATA_SIZE (1 << 31)  //最大存储空间2G
#define SIGNAL_RESCUE_MAX_DATA_SIZE (500 << 20) //单个营救的最大存储空间500M
