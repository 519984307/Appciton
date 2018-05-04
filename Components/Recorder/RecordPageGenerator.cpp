#include "RecordPageGenerator.h"
#include <QTimerEvent>
#include <QDateTime>
#include <qmath.h>
#include <QPainterPath>
#include <QPainter>
#include <QStringList>
#include "FontManager.h"
#include "LanguageManager.h"

#define DEFAULT_PAGE_WIDTH 200
RecordPageGenerator::RecordPageGenerator(QObject *parent)
    :QObject(parent), _requestStop(false), _generate(true), _timerID(-1)
{
}

RecordPageGenerator::~RecordPageGenerator()
{
}

int RecordPageGenerator::type() const
{
    return Type;
}

void RecordPageGenerator::start(int interval)
{
    _timerID = startTimer(interval);
}

void RecordPageGenerator::stop()
{
    _requestStop = true;
}

void RecordPageGenerator::pageControl(bool pause)
{
    _generate = !pause;
}

QFont RecordPageGenerator::font() const
{
    return fontManager.recordFont(24);
}

RecordPage *RecordPageGenerator::createPage()
{
    RecordPage *p = new RecordPage(DEFAULT_PAGE_WIDTH);
#if 1
    //draw a sin wave
    qreal yOffset = p->height() / 2;
    qreal xOffset = 2 * 3.1415926 / DEFAULT_PAGE_WIDTH;
    QPainter painter(p);
    painter.setPen(Qt::white);
    static QPointF lastP(0, yOffset);
    QPointF p2;

    for(int i = 0; i< p->width(); i++)
    {
        p2.setX(i);
        p2.setY(yOffset - qSin(i * xOffset) * (p->height() / 2 - 2));
        painter.drawLine(lastP, p2);
        lastP = p2;
    }

    lastP.setX(0);
    lastP.setY(lastP.y() - 1);


#endif
    return p;
}

RecordPage *RecordPageGenerator::createTitlePage(const QString &title, const PatientInfo &patInfo, unsigned timestamp)
{
    QStringList infos;
    infos.append(QString("%1: %2").arg(trs("Name")).arg(patInfo.name));
    infos.append(QString("%1: %2").arg(trs("Gender")).arg(PatientSymbol::convert(patInfo.sex)));
    infos.append(QString("%1: %2").arg(trs("PatientType")).arg(PatientSymbol::convert(patInfo.type)));
    infos.append(QString("%1: %2").arg(trs("Blood")).arg(PatientSymbol::convert(patInfo.blood)));
    QString str;
    str = QString("%1: ").arg(trs("Age"));
    if(patInfo.age > 0)
    {
        str += QString::number(patInfo.age);
    }
    infos.append(str);

    str = QString("%1: ").arg(trs("Weight"));
    if(patInfo.weight)
    {
        str += QString("%1 %2").arg(QString::number(patInfo.weight)).arg(PatientSymbol::convert(patInfo.weightUnit));
    }
    infos.append(str);

    str = QString("%1: ").arg(trs("Height"));
    if(patInfo.height)
    {
        str += QString::number(patInfo.height);
    }
    infos.append(str);

    infos.append(QString("%1: ").arg(trs("ID")).arg(patInfo.id));

    //calculate the info text width
    int textWidth = 0;
    int w = 0;
    QFont font = fontManager.recordFont(24);
    foreach (QString infoStr, infos) {
        w = fontManager.textWidthInPixels(infoStr, font);
        if(w > textWidth)
        {
            textWidth = w;
        }
    }

    //title width
    w = fontManager.textWidthInPixels(title, font);
    if(w > textWidth)
    {
        textWidth =  w;
    }

    QDateTime dt = QDateTime::currentDateTime();
    if(timestamp)
    {
        dt = QDateTime::fromTime_t(timestamp);
    }

    QString timeStr = QString("%1: %2").arg(trs("RecordTime")).arg(dt.toString("yyyy-MM-dd HH:mm:ss"));

    //record time width
    w = fontManager.textWidthInPixels(timeStr, font);
    if(w > textWidth)
    {
        textWidth =  w;
    }

    int pageWidth = textWidth + font.pixelSize() * 3;
    int fontH = fontManager.textHeightInPixels(font);

    RecordPage *page = new RecordPage(pageWidth);
    QPainter painter(page);
    painter.setPen(Qt::white);
    painter.setFont(font);

    //we assume the page can hold all the rows
    QRect textRect(font.pixelSize(), fontH, textWidth, fontH);
    painter.drawText(textRect, Qt::AlignLeft|Qt::AlignVCenter, title);

    //left one empty row
    textRect.translate(0, fontH/2);

    foreach (QString infoStr, infos) {
        textRect.translate(0, fontH);
        painter.drawText(textRect, Qt::AlignLeft|Qt::AlignVCenter, infoStr);
    }

    //recording time in the bottom
    textRect.setTop(page->height() - fontH - fontH/2);
    textRect.setBottom(page->height() - fontH / 2);
    painter.drawText(textRect, Qt::AlignLeft|Qt::AlignVCenter, timeStr);

    return page;
}

RecordPage *RecordPageGenerator::createTrendPage(const TrendDataPackage &trendData, bool showEventTime)
{

}

QStringList RecordPageGenerator::getTrendStringList(const TrendDataPackage &trendData)
{
    QStringList strList;
    return strList;
}

void RecordPageGenerator::timerEvent(QTimerEvent *ev)
{
    if(_timerID == ev->timerId())
    {
        if(_requestStop)
        {
            killTimer(_timerID);
            _timerID = -1;
            _requestStop = false;
            emit stop();
            return;
        }

        if(!_generate)
        {
            return;
        }

        RecordPage *page = createPage();
        if(page == NULL)
        {
            killTimer(_timerID);
            emit stop();
            return;
        }

        emit generatePage(page);
    }
}
