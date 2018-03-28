#pragma once
#include "PrintLayoutIFace.h"

enum
{
    TEST_FAIL,
    TEST_OK,
    TEST_NOT_PERFORMED,
};

// 30J自测结果
struct ResultOf30JSelftst
{
    bool hasShockData;   //是否有放电数据
    int result;
    unsigned time;
    int current;
    int del;
    int impedance;

    ResultOf30JSelftst()
    {
        hasShockData = false;
        result = TEST_FAIL;
        time = 0;
        current = 0;
        del = 0;
        impedance = 0;
    }
};

class PrintOtherTriggerLayout : public PrintLayoutIFace
{
public:
    PrintOtherTriggerLayout();
    ~PrintOtherTriggerLayout();

    void start(void); // 开始布局。
    void abort(void); // 布局中止。

    // 对打印内容进行布局排版，构建相应布局的打印页
    PrintPage *exec(PrintContent *content);

private:
    PrintPage *_30JSelfTest(PrintContent *content);

private:
    bool _is30JSelftestEnd;     //30J自测打印是否结束
};

