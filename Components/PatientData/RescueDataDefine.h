#pragma once

#define DATA_STORAGE_WAVE_CHANNEL_NAME ("dataStorageWave")
#define DATA_STORAGE_WAVE_TIME (120)

//数据类型
enum RescueDataType
{
    TREND_RESCUE_DATA,             //趋势
    SUMMARY_RESCUE_DATA,           //概要报告
    ECG12L_RESCUE_DATA,            //12L
    RESCUE_DATA_NR
};

//打印类型
enum RescueDataPrintType
{
    PRINT_SELECT_RESCUE,              //选择的营救
    PRINT_RANGE_RESCUE,               //营救范围
    PRINT_SELECT,                     //当前选择,一个营救中的一个或几个快照
    PRINT_SELECT_NR
};

//打印类型
enum ECG12LeadDataPrintType
{
    PRINT_12L_SELECT_RESCUE,              //选择的营救
    PRINT_12L_SELECT,                     //当前选择,一个营救中的一个或几个快照
    PRINT_12L_SELECT_NR
};

//传送类型
enum ECG12LEADTransferType
{
    USB_RESCUE,                //选择的营救
    USB_SELECT,                //当前选择,一个营救中的一个或几个快照
    EMAIL_SELECT,
};

//打印选择
enum RANGESELECT
{
    RANGE_SELECT,
    ALL_RESCUES
};

//打印类型
enum SummaryDataPrintType
{
    //单个营救
    PRINT_SELECTED_SNAPSHOT,             //选择当前查看营救中的一个或者多个快照
    PRINT_RESCUE_HEAD_AND_SNAPSHOTS,     //当前查看的营救数据
    PRINT_RESCUE_LOG,                    //当前查看的营救日志

    PRINT_RESCUE_MORE,                   //更多选择

    //选择的一个或多个营救
    PRINT_SELECTED_HEAD_AND_SNAPSHOTS,   //选择营救的头部和快照
    PRINT_SELECTED_LOG,                  //选择营救的日志
    PRINT_ALL_RESCUE_HEADS,              //所有营救的头部
    PRINT_ALL_RESCUE                    //所有营救
};

//页面加载方向
enum PageLoadType
{
    PAGE_UP,
    PAGE_DOWN,
    PAGE_LEFT,
    PAGE_RIGHT,
    PAGR_NR
};

