#pragma once
#include "PrintLayoutIFace.h"
#include "PrintSummaryLayout.h"
#include "SummaryItem.h"

class PrintTriggerSummaryLayout : public PrintLayoutIFace, public PrintSummaryLayout
{
public:
    PrintTriggerSummaryLayout();
    ~PrintTriggerSummaryLayout();

    void start(void); // 开始布局。
    void abort(void); // 布局中止。

    // 对打印内容进行布局排版，构建相应布局的打印页
    PrintPage *exec(PrintContent *content);

private:
    PrintPage *_drawWave(unsigned char *data, int datalen);

    int _timeNum;   // 打印的时间
    int _pageTimeLen; // 每页打印的时间长度
    SummaryItem *_summaryitem;
    typedef QList<PrintWaveInfo> WaveList;
    QList<PrintWaveInfo> _waveInfoList; //波形信息列表
};


