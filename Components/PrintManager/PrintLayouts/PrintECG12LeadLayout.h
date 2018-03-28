#pragma once
#include "PrintLayoutIFace.h"
#include "RescueDataDefine.h"
#include "TrendDataWidget.h"
#include "PrintContinuousLayout.h"
#include "ECG12LDataStorage.h"
#include "ParamDataStorageManager.h"

//current print wave and gain

#define WAVE_START_PIXEL_Y    (fontHeight() + 4)         // y轴的波形起始位置。
#define WAVE_PIXEL_Y          (560)                      // y轴波形区域所占像素值。(7cm）

#define DRAW_PARAMS_BURST     (20 * PIXELS_PER_MM)       // 画参数时的分页大小。

#define ECG12LEAD_WAVECACHE_CHANNEL_NAME ("PrintECG12LeadLayout")
#define MAX_WAVE_ECG12LEAD (12)                         //打印最大波形道数

#define ECG_WAVE_RATE_250    (250)
#define ECG_WAVE_RATE_500    (500)
#define ECG12LEAD_WAVE_NUM_RATE_250 (ECG_WAVE_RATE_250 * 2.5)
#define ECG12LEAD_WAVE_NUM_RATE_500 (ECG_WAVE_RATE_500 * 2.5)

#define WAVEWIDTHSHORT     ECG12LEAD_WAVE_NUM_RATE_500

//current peint wave info
struct ECG12LeadWaveformInfo
{
    void reset(void)
    {
        waveRate = 0;
        waveRangeMax = 0;
        waveRangeMin = 0;
        waveBaseline = 0;
        waveDataNum = 0;
        startY = 0;
        mediumY = 0;
        endY = 0;
        offsetX = 0;
        waveID = WAVE_NONE;
        waveBuf.clear();
    }

    int waveRate;                  // 波形速率。
    int waveRangeMax;              // 波形数据最大值。
    int waveRangeMin;              // 波形数据最小值。
    int waveBaseline;              // 波形基线值。
    int waveDataNum;               // 一页大小需要的波形数据的个数。
    int startY;                    // 不同波形的y坐标起始偏移像素点。
    int mediumY;                   // 不同波形的y坐标中间位置。
    int endY;                      // 不同波形的y坐标的结束偏移像素点。
    double offsetX;                // 数据的x轴偏移。
    QString waveTitle;             // 波形标识。
    WaveformID waveID;             // 波形ID
    QVector<WaveDataType> waveBuf; //波形数据
};
typedef QList<struct ECG12LeadWaveformInfo> WaveformECG12LEAD;


//current print wave and gain
struct ECGWaveGain
{
    ECGWaveGain(WaveformID waveID, int waveGain)
    {
        id = waveID;
        gain = waveGain;
    }

    WaveformID id;
    int gain;

    bool operator !=(const struct ECGWaveGain &ECGWaveGain) const
    {
        if (id != ECGWaveGain.id || gain != ECGWaveGain.gain)
        {
            return true;
        }

        return false;
    }
};
//12导联
typedef QList<struct ECGWaveGain> ECG12LeadWaveGainList;

class PrintECG12LeadLayout : public PrintLayoutIFace
{
public:
    static PrintECG12LeadLayout &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new PrintECG12LeadLayout();
        }

        return *_selfObj;
    }

    static PrintECG12LeadLayout *_selfObj;

    PrintECG12LeadLayout();
    ~PrintECG12LeadLayout();

    //开始布局
    void start(void);

    //布局中止
    void abort(void);

    //对打印内容进行布局排版，构建相应布局的打印页
    PrintPage *exec(PrintContent *content);


protected:
    //数据初始化
    bool _dataInit(bool IsRealtime);

private:
    //是否是实时打印 1是实时打印 0是趋势打印
    PrintPage *_rescuePage(bool IsRealtime);

    //选择营救打印页
    PrintPage *_drawSelectPage(bool IsRealtime);

    //打印开始页 判断是否是实时打印
    PrintPage *_startPage(const ECG12LDataStorage::ECG12LeadData *info);

    //绘制波形增益和导联标签
    void _drawWaveLabel(QPainter &painter, const ECG12LeadWaveformInfo &waveDesc);

    //绘制其他信息的文本
    void _drawText(QPainter &painter, QString text, qreal x, qreal y);

    //得到2.5s短波的绘制速度/开始的坐标轴/波形ID等信息 标准or卡布雷诺格式
    void _getWaveFormShortWave();

    //得到10s短波的绘制速度/开始的坐标轴/波形ID等信息  标准or卡布雷诺格式
    void _getWaveFormLongWave();

    //绘制2.5秒的十二道短波形
    void _drawShortWave(int index, QPainter &painter, int pageWidth,int count,
            const ECG12LDataStorage::ECG12LeadData *info);

    //绘制10秒的二道长波形
    void _drawlongWave(int index,QPainter &painter, int pageWidth,
            const ECG12LDataStorage::ECG12LeadData *info);

    //得到波形增益
    void _getWaveForm(WaveGainList &waveList);

    //12导的虚线绘制
    void _drawDottedLine(QPainter &painter, qreal x1, qreal y1, qreal x2, qreal y2);

    //根据增益映射波形的数值
    double _mapWaveValue(const WaveGain &waveGain, const ECG12LeadWaveformInfo &waveDesc,
            int waveValue);

    //得到波形的数值（暂时舍弃）
    void _getWaveData(struct ECG12LeadWaveformInfo &waveDesc, int sec,
            const ECG12LDataStorage::ECG12LeadData *info);

    //普通导联和cabrera的导联切换映射
    ECGLead _leadMap(ECGLead index);

    //标志打印的时间段
    void _markTimeSegments(QPainter &painter,int index);
private:
    QStringList _filePath;                              //路径
    IStorageBackend *_file;                             //文件路径
    ECG12LDataStorage::ECG12LeadData *_data;

    int _fileNum;                                       //当前打印文件的索引
    int _realstartid;                                   //快照打印的头文件索引
    int _rescuetartid;                                  //趋势打印的头文件索引
    int _index;                                         // 当前索引项
    int _step;                                          //将画图的步骤分为9步通过_step控制
    bool _printStartPage;
    int _pagewidth;                                     //绘制图片的大小
    bool _isNeedDrawFirstPage;                          //draw first page need draw other wave flag
    int _totalData;                                     // 本次打印的总项数
    int _shortWaveOffest;                               //2,5S的长波偏移计数
    int _longWaveOffest;                                //10S的长波偏移计数
    int _numOfWaveTrace;
    ECG12LeadDataPrintType _type;                          // 打印类型
    PrintSpeed _printSpeed;                             //printer speed
    WaveGainList _waveGainList;
    WaveformECG12LEAD _waveformDescList;                //wave data info
    qreal _drawLeadFaultStart[MAX_WAVE_ECG12LEAD];      //计算laedoff的距离
    qreal _drawLeadFaultLen[MAX_WAVE_ECG12LEAD];        //如果leadoff标志超过界面长度 记录超过的长度
    QImage *_leadoffImage;                              //lead off mage
private:
    unsigned int _t;
    unsigned int _ms;
    bool _realtimePrint;                                //是否是实时打印 1是实时打印 0是趋势打印
    bool _isCabrera;                                    //判断是否是标准格式打印
private:                                                //12导联选择营救变量
    int _dataLen;
    ECG12LDataStorage::ECG12LeadData *_selectdata;

private:
    int _debugNewCount;                                 //用作Debug内存的增删，检查是否有内存泄露，以后删除
};

