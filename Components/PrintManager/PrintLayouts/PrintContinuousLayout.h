#pragma once
#include "PrintLayoutIFace.h"
#include "ParamDefine.h"
#include "TrendCache.h"
#include "PatientDefine.h"
#include "SystemDefine.h"

#define REALTIME_WAVECACHE_CHANNEL_NAME ("PrintContinuousLayout")
#define MAX_WAVE_TRACE (3)//打印最大波形道数

//current print wave and gain
struct WaveGain
{
    WaveGain(WaveformID waveID, int waveGain)
    {
        id = waveID;
        gain = waveGain;
    }

    WaveformID id;
    int gain;

    bool operator !=(const struct WaveGain &waveGain) const
    {
        if (id != waveGain.id || gain != waveGain.gain)
        {
            return true;
        }

        return false;
    }
};

typedef QList<struct WaveGain> WaveGainList;

//current peint wave info
struct WaveformDesc
{
    void reset(void)
    {
        isSameWave = false;
        waveRate = 0;
        waveRangeMax = 0;
        waveRangeMin = 0;
        waveBaseline = 0;
        waveDataNum = 0;
        actualDrawWaveDataNum = 0;
        startY = 0;
        mediumY = 0;
        endY = 0;
        positionY = 0;
        offsetX = 0;
        offsetPage = 0;
        waveID = WAVE_NONE;
        waveBuf.clear();
    }

    bool isSameWave;          // 是否为重复波形，即和前几道波形相同
    int waveRate;             // 波形速率。
    int waveRangeMax;         // 波形数据最大值。
    int waveRangeMin;         // 波形数据最小值。
    int waveBaseline;         // 波形基线值。
    int waveDataNum;          // 一页大小需要的波形数据的个数。
    int actualDrawWaveDataNum;// 实际绘制的波形数据个数
    int startY;               // 不同波形的y坐标起始偏移像素点。
    int mediumY;              // 不同波形的y坐标中间位置。
    int endY;                 // 不同波形的y坐标的结束偏移像素点。
    qreal positionY;          // 起始Y坐标
    double offsetX;           // 数据的x轴偏移。
    double offsetPage;        // 页面误差
    QString waveTitle;        // 波形标识。
    WaveformID waveID;        // 波形ID
    QVector<WaveDataType> waveBuf; //波形数据
};

typedef QList<struct WaveformDesc> WaveformDescList;

class PrintContinuousLayout : public PrintLayoutIFace
{
public:
    virtual void start(void);  // 开始布局。
    virtual void abort(void);  // 布局中止。
    virtual PrintPage *exec(PrintContent *content); // 执行布局，产生打印页。

    PrintContinuousLayout();
    virtual ~PrintContinuousLayout();

    //设置放电信息
    static void setShockInfo(unsigned t, int curEngry, int delEngry, int current, int ttl);

private:
    bool _checkWaveform();
    bool _checkPatientInfo(void);
    bool _checkPrintSpeed(void);
    bool _checkWorkMode(void);
    bool _checkParamStop(WaveformID id);
    bool _checkAllParamStop(void);
    void _checkNeedDrawBandwidth(int wavePos, int bandwidth, QPainter &painter, int pageWidth, int offx);
    bool _isNeedFirstPage(void);
    void _getWaveForm(WaveGainList &waveList);
    void _getWaveFormCommonDesc(void);
    void _getWaveData(struct WaveformDesc &waveDesc, int sec);
    bool _checkWaveNum(int wavePos, WaveformID id, int pageWidth);
    void _clearWaveData(void);
    void _getDrawText(void);
    int _caclPageWidth(QStringList &text);
    void _drawText(QPainter &painter, QStringList &text, qreal x, qreal y);
    void _drawText(QPainter &painter, QString text, qreal x, qreal y);
    void _drawDottedLine(QPainter &painter, qreal x1, qreal y1, qreal x2, qreal y2);
    PrintPage *_produceFirstPage(void);

    double _mapWaveValue(const WaveGain &waveGain, const WaveformDesc &waveDesc, int waveValue);
    void _drawWaveLabel(QPainter &painter, const WaveformDesc &waveDesc, const WaveGain &waveGain,
                        int pos);
    void _drawWave(int index, QPainter &painter, int pageWidth);
    void _drawShockImage();
    void _drawECGBandwidth(int wavePos, int bandwidth, QPainter &painter, int pageWidth, int offx);
    void _preHandleECGBandwidth(QPainter &painter, int pageWidth);

private:
    bool _startContinuousPrint;        //print start flag
    bool _isNeedDrawFirstPage;         //draw first page need draw other wave flag
    bool _shockDeliveried;             //shock
    bool _updateCo2Ruler;              //update co2 ruler
    int _pageNum;                      //use to named print picture in x11 platform
    int _numOfWaveTrace;               //the number of print wave trace
    int _totalPageWidth;               //the total of the draw page width in 10 seconds
    unsigned _time;                    //time
    qreal _drawLeadFaultStart[MAX_WAVE_TRACE];
    qreal _drawLeadFaultLen[MAX_WAVE_TRACE];

    UserFaceType _faceType;
    PatientInfo _patInfo;              //patient info
    PrintSpeed _printSpeed;            //printer speed

    QStringList _firstLineDrawStr;            //record every 10s time
    QStringList _secLineDrawStr;              //param value str
    QStringList _thirdLineDrawStr;
    WaveGainList _waveGainList;
    WaveformDescList _waveformDescList;//wave data info

    PrintPage *_curpage;               //current page point
    QImage *_leadoffImage;             //lead off image
    QImage *_leadFaultImage;           //lead fault image

// CPR bar graph
private:
    int _lastDrawCPRBarWidth;          //last draw cpr bar width
    int _lastDrawCPRLineWidth;         //last draw cpr line width
    int _lastCPRWaveMapValue;          //last CPR wave map value

// shock delivery info
private:
    static unsigned _shockTime;        //shock time
    static int _curSelEngry;           //select engry
    static int _curSelEngryBk;         //backup of select engry
    static int _delEngey;              //deliveried engry
    static int _current;               //current
    static int _ttl;                   //imdependent

    bool _drawShockFlag;               //if already draw shock image
    int _drawShockInfoOff;             //draw shock info offset
    int _drawShockInfoWidth;           //already draw shock info width
    QImage *_shockInfoImage;           //shock info image

// bandwidth
private:
    struct BandWidthAttr
    {
        void reset()
        {
            isInit = false;
            startOffx = 0;
            yPoint = 0;
            drawLen = 0;
            bdVec.clear();
        }

        BandWidthAttr()
        {
            reset();
        }

        bool isInit;                   //whether this trace need draw bandwidth
        qreal startOffx;               //draw x point
        qreal yPoint;                  //draw y point
        qreal drawLen;                 //already draw image width
        QVector<int> bdVec;            //need draw bandwidth
    };

    BandWidthAttr _ecgBdVec[MAX_WAVE_TRACE];  //ecg trace  bandwidth attr
    QImage *_bandwidthImage[ECG_BANDWIDTH_NR];
};
