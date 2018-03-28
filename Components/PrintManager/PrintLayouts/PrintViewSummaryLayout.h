#pragma once
#include "PrintLayoutIFace.h"
#include "PrintSummaryLayout.h"
#include "RescueDataDefine.h"

class StorageFile;
class PrintViewSummaryLayout : public PrintLayoutIFace, public PrintSummaryLayout
{
public:
    PrintViewSummaryLayout();
    ~PrintViewSummaryLayout();

    void start(void); // 开始布局。
    void abort(void); // 布局中止。

    // 对打印内容进行布局排版，构建相应布局的打印页
    PrintPage *exec(PrintContent *content);

private:
    // 数据初始化
    bool _dataInit(bool drawHeader = false);
    void _drawCompleteInfo(PrintPage *page, int high);
    PrintPage * _drawWaveCompleteInfo();

    // 构造一个快照打印页
    PrintPage *_drawOneSnapShock(unsigned char *data, int len);

    // 构造不同类型的打印页
    PrintPage *_drawSelectPage();
    PrintPage *_drawSelectRescuePage();
    PrintPage *_drawRescueLogPage();
    PrintPage *_drawRescueHeadPage();
    bool _noSelectSnapshotAfterIndex(int index);

private:
    QStringList _filePath;
    StorageFile *_file;
    QString _curIncident;
    SummaryDataPrintType _type;
    int _index;
    int _totalData;
    int _fileNum;
    int _timeNum;   // 打印的时间
    int _pageTimeLen; // 每页打印的时间长度
    int _pageWidth;   // 页宽

    unsigned char *_data;
    int _dataLen;
    bool _oneSnapShockOK;
    bool _drawWaveCompleteInfoFlag;
    typedef QList<PrintWaveInfo> WaveList;
    QList<PrintWaveInfo> _waveInfoList; //波形信息列表
    QVector<bool> _checkFlags;
};



