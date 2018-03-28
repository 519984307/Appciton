#pragma once
#include <QFont>
#include "SummaryDefine.h"
#include "ParamDefine.h"

//波形绘画的高度
#define SUMMARY_WAVE_HEIGHT (70 * PIXELS_PER_MM) //70mm

struct PrintWaveInfo;

// 不同summary类型的后续处理
typedef void (*summaryTypeCallBack)(QPainter &, const PrintWaveInfo &);

struct PrintWaveInfo
{
    WaveformID id;            // 波形ID
    int pageNum;              // 第几页波形
    int pageWidth;            // 打印页宽度
    int waveDataNum;          // 波形数据的个数。
    int waveBuff[600];        // 波形数据的缓存。
    int startY;               // 不同波形的y坐标起始偏移像素点。
    int medialY;              // 不同波形的y坐标中间位置。
    int endY;                 // 不同波形的y坐标的结束偏移像素点。
    int ecgGain;              // ecg增益
    int ecgBandwidth;        // ecg bandwidth
    int respGain;             // resp增益
    int co2Zoom;              // co2增益
    int spo2Gain;             // spo2增益
    unsigned time;            // 事件时间
    char *data;               // 原始数据
    double offsetX;           // 数据的x轴偏移。
    summaryTypeCallBack callBack;
    char summaryType;         //summary type
    char diagECGBandWidth;   //only valid for diagnostic ECG snapshot
    ECGLead calcLead;          //calc lead
    bool drawAllEcgGain;        //draw all ecg gain flag

    PrintWaveInfo()
    {
        id = WAVE_NONE;
        pageNum = 0;
        pageWidth = 0;
        waveDataNum = 0;
        memset(waveBuff, 0, sizeof(waveBuff));
        startY = 0;
        medialY = 0;
        endY = 0;
        ecgGain = 0;
        ecgBandwidth = 0;
        respGain = 0;
        co2Zoom = 0;
        spo2Gain = 0;
        offsetX = 0;
        time = 0;
        callBack = NULL;
        data = NULL;
        diagECGBandWidth = ECG_BANDWIDTH_0525_40HZ;
        drawAllEcgGain = false;
    }
};


// summary打印
class PrintPage;
class QPainter;
class PrintSummaryLayout
{
public:
    PrintSummaryLayout();
    virtual ~PrintSummaryLayout();

    // 构建打印页
    PrintPage *buildPage(const unsigned char *data, int len, const QFont &font);

    // 构建开始打印页
    PrintPage *buildStartPage(const unsigned char *data, int len, const QFont &font);

    // 构建波形打印页
    PrintPage *buildWavePage(const QList<PrintWaveInfo> &printWaveInfo, const QFont &font);

    // 打印页初始化
    void init();

    // 开始页是否打印完
    bool isStartPageOver() const {return _isStartPageOver;}

    //is start page init
    bool isStartPageInit() const {return _isStartPageInit;}

    //check when content string has been printed.
    bool isContentStrPrintFinished() const { return _curItem >= _contentStr.count();}

private:
    // 开始打印页初始化
    void _startInit(const SummaryHead &head);

    // 获取公共信息
    void _getValue(const SummaryItemDesc &desc, const SummaryCommonInfo &info);
    void _getCodeMarkerValue(SummaryCodeMarker *data);
    void _getNIBPValue(SummaryNIBP *data);

    // 获取波形数值范围
    void _getWaveRange(const PrintWaveInfo &info, double &max, double &min);

    // 映射波形数值
    double _mapWaveValue(const PrintWaveInfo &info, short value, double max, double min);

    // 计算打印页宽度
    int _calPageWidth(const QFont &font);

    // 画波形标签
    void _drawWaveLabel(QPainter &painter, const PrintWaveInfo &info, int index);

    // 构建各种类型的打印页
    PrintPage *_drawPage(const QFont &font);

private:
    QStringList _contentStr;     // 需打印的字符串
    int _curItem;                // 当前项
    bool _isStartPageInit;           // 开始页初始化
    bool _isStartPageOver;           // 开始页打印完
    bool _isOtherPageInit;                // 其它页初始化
};

