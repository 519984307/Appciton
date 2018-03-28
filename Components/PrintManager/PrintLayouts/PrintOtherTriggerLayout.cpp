#include <QPainter>
#include "PrintOtherTriggerLayout.h"
#include "TimeDate.h"
#include "LanguageManager.h"
#include "PrintManager.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PrintOtherTriggerLayout::PrintOtherTriggerLayout() : PrintLayoutIFace(PRINT_LAYOUT_ID_OTHER_TRIGGER_REPORT)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PrintOtherTriggerLayout::~PrintOtherTriggerLayout()
{

}

/**************************************************************************************************
 * 开始。
 *************************************************************************************************/
void PrintOtherTriggerLayout::start()
{
    _is30JSelftestEnd = false;
    printManager.enablePrinterSpeed(printManager.getPrintSpeed());
}

/**************************************************************************************************
 * 终止。
 *************************************************************************************************/
void PrintOtherTriggerLayout::abort()
{

}

/**************************************************************************************************
 * 执行。
 *************************************************************************************************/
PrintPage *PrintOtherTriggerLayout::exec(PrintContent *content)
{
    if (NULL == content)
    {
        return NULL;
    }

    switch (content->printType)
    {
        case PRINT_TYPE_TRIGGER_30J_SELFTEST:
            return _30JSelfTest(content);
        default:
            return NULL;
    }
}

/**************************************************************************************************
 * 30J自测触发打印。
 *************************************************************************************************/
PrintPage *PrintOtherTriggerLayout::_30JSelfTest(PrintContent *content)
{
    if (_is30JSelftestEnd)
    {
        return NULL;
    }

    if (content->extraDataLen != sizeof(ResultOf30JSelftst))
    {
        return NULL;
    }

    if (NULL == content->extraData)
    {
        return NULL;
    }

    ResultOf30JSelftst *result = reinterpret_cast<ResultOf30JSelftst*>(content->extraData);
    if (NULL == result)
    {
        return NULL;
    }

    int gap = 3;
    int fontH = fontManager.textHeightInPixels(printFont());
    int width = (fontH + gap) * 2 - gap;
    PrintPage *page = new PrintPage(width);
    if (NULL == page)
    {
        return NULL;
    }

    static int id = 0;
    page->setID(QString("30Jselftest%1").arg(id++));
    QRect r = page->rect();
    QRect drawR(-r.height(), 0, r.height(), r.width());
    QPainter painter(page);
    painter.setFont(printFont());
    painter.setPen(Qt::white);
    painter.rotate(-90.0);

    QString str;
    timeDate.getDateTime(result->time, str, true, true);
    str += "     ";
    switch (result->result)
    {
        case TEST_OK:
            str += trs("TestOK");
            break;

        case TEST_FAIL:
            str += trs("TestFail");
            break;

        case TEST_NOT_PERFORMED:
            str += "30J ";
            str += trs("TestNotPerformed");
            break;

        default:
            break;
    }

    painter.drawText(drawR, Qt::AlignHCenter | Qt::AlignTop, str);

    if (result->hasShockData)
    {
        str.clear();

        str += trs("PatCurrent");
        str += "= ";
        str += QString().sprintf("%.1f", result->current / 10.0);
        str += "A";
        str += "  ";

        str += QString::number(result->del);
        str += " J ";
        str += trs("DEL.");
        str += "  ";

        str += trs("DefibImped");
        str += "= ";
        str += QString().sprintf("%d", result->impedance / 10);
        str += "  ";
        painter.drawText(drawR, Qt::AlignHCenter | Qt::AlignBottom, str);
    }

    _is30JSelftestEnd = true;

    return page;
}
