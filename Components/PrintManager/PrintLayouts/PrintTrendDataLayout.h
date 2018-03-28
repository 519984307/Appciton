#pragma once
#include "PrintLayoutIFace.h"
#include "RescueDataReview.h"
#include "ParamDataStorageManager.h"

class CircleFile;
class PrintTrendDataLayout : public PrintLayoutIFace
{
public:
    PrintTrendDataLayout();
    ~PrintTrendDataLayout();

    void start(void); // 开始布局。
    void abort(void); // 布局中止。

    // 对打印内容进行布局排版，构建相应布局的打印页
    PrintPage *exec(PrintContent *content);

private:
    PrintPage *_startPage(const ParamDataDescription &info);
    PrintPage *_rescuePage();
    int _drawRescuePageTitle(PrintPage *page, unsigned int timestamp);
    int _drawRescuePageData(PrintPage *page, ParamDataStorageManager::ParamBuf &buf, int high);
    void _drawCompleteInfo(PrintPage *page, int high);
    bool _dataInit();

private:

    QList<SubParamID> _paramList;
    QStringList _filePath;
    IStorageBackend *_file;
    int _fileNum;                      // 当前打印文件的索引
    int _index;                        // 当前索引项
    int _totalData;                    // 本次打印的总项数
    int _interval;                     // 趋势分辨率
    int _timeWidth;                    // 时间宽度
    int _nibpParamWidth;               // nibp参数宽度
    int _otherParamWidth;              // 其它参数宽度
    RescueDataPrintType _type;         // 打印类型
    bool _printStartPage;              // 是否打印开始页
    ParamDataDescription _descInfo;
};

