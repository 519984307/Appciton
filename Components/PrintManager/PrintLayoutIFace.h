#pragma once
#include "PrintTypeDefine.h"
#include "PrintPage.h"
#include "FontManager.h"

/**************************************************************************************************
 * 打印内容结构
 *************************************************************************************************/
enum PrintResult
{
    PrintCancel,
    PrintAbort,
    PrintSuccess,
};

typedef void (*PrintFinishedCallback)(PrintResult, unsigned char *data, int len);
struct PrintContent
{
    PrintContent(PrintType type = PRINT_TYPE_NONE,  PrintLayoutID id = PRINT_LAYOUT_ID_NONE,
            unsigned char *data = 0, int len = 0, PrintFinishedCallback cb = NULL)
    {
        printType = type;
        layoutID = id;
        extraDataLen = len;
        extraData = data;
        finishedCallback = cb;
    }

    void finished(PrintResult result)
    {
        if(finishedCallback)
        {
            finishedCallback(result, extraData, extraDataLen);
        }
    }

    ~PrintContent()
    {
        if (extraData != NULL)
        {
            delete[] extraData;
        }
    }

    PrintType printType;         // 打印内容类型，与打印布局类型匹配。
    PrintLayoutID layoutID;      // 对应打印布局对象ID。
    int extraDataLen;            // 打印请求的附加内容长度，单位:字节
    unsigned char *extraData;    // 打印请求的附加内容。
    PrintFinishedCallback finishedCallback;
};

/**************************************************************************************************
 * 打印布局
 *************************************************************************************************/
class PrintLayoutIFace
{
public:
    PrintLayoutIFace(PrintLayoutID id = PRINT_LAYOUT_ID_NONE)
    {
        _id = id;
        _font = fontManager.recordFont(24);
        _fontHeight = QFontMetrics(_font).height();
    }
    virtual ~PrintLayoutIFace() {}

    // 返回打印布局类型
    PrintLayoutID getLayoutID(void) const
    {
        return _id;
    }

    // 以下接口是派生布局器需要实现的，注意:它们在打印线程中调用，所以在实现时要注意多线程问题。
    virtual void start(void) = 0; // 开始布局。
    virtual void abort(void) = 0; // 布局中止。

    // 对打印内容进行布局排版，构建相应布局的打印页
    virtual PrintPage *exec(PrintContent *content) = 0;

protected:
    const QFont &printFont(void)
    {
        return _font;
    }

    int fontHeight(void)
    {
        return _fontHeight;
    }

private:
    PrintLayoutID _id;
    QFont _font;
    int _fontHeight;
};
