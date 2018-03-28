#include "QPrinterPdfWriter.h"
#include <QPainter>
#include <QPrinter>
#include <QPrintEngine>
#include <QBuffer>
#include <QFile>
#include "Debug.h"

#define TEMP_FILE "/tmp/tmp.pdf"
#define PPK_CustomIODevice ((QPrintEngine::PrintEnginePropertyKey)(QPrintEngine::PPK_CustomBase + 1))

class QPrinterPdfWriterPrivate
{
public:
    QPrinterPdfWriterPrivate(int widthMM, int heightMM, int dpi);
    ~QPrinterPdfWriterPrivate();

    QPrinter *printer;
    QPainter *painter;
    bool supportCustomEngine;
    QByteArray content;
    QIODevice *dev;
};

/***************************************************************************************************
 * test the pdf print engiene and check whether support custom IO device
 **************************************************************************************************/
static bool printEngineTest()
{
    //NOTE: we need to test whether the engine support the custom IODevice, the Qt libray in iDM has
    //      been modify to support the custom iodevice, for other Qt library, we have to save the pdf output
    //      to a file
    static int result = -1;

    if(result == -1)
    {
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        QPrintEngine *engine = printer.printEngine();

        QBuffer buf;
        QIODevice *dev = &buf;
        engine->setProperty(PPK_CustomIODevice, qVariantFromValue((void *)dev));
        QVariant var = engine->property(PPK_CustomIODevice);

        result = var.value<void *>() == (void*)dev;
    }

    return result;
}

QPrinterPdfWriterPrivate::QPrinterPdfWriterPrivate(int widthMM, int heightMM, int dpi)
    :printer(new QPrinter(QPrinter::HighResolution)), painter(NULL),
      supportCustomEngine(false)
{
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setPaperSize(QSizeF(widthMM, heightMM), QPrinter::Millimeter);
    printer->setResolution(dpi);
    printer->setPageMargins(0,0,0,0,QPrinter::Millimeter);


    supportCustomEngine = printEngineTest();
    if(supportCustomEngine)
    {
        dev = new QBuffer(&content);
        dev->open(QIODevice::WriteOnly);
        QPrintEngine *engine = printer->printEngine();
        engine->setProperty(PPK_CustomIODevice, qVariantFromValue((void *)dev));
    }
    else
    {
        printer->setOutputFileName(TEMP_FILE);
        dev = new QFile(TEMP_FILE);
    }

    painter = new QPainter(printer);
}

QPrinterPdfWriterPrivate::~QPrinterPdfWriterPrivate()
{

   if(painter)
   {
       delete painter;
       painter = NULL;
   }

   if(printer)
   {
       delete printer;
       printer = NULL;
   }

   if(dev)
   {
       delete dev;
       dev = NULL;
   }
}

QPrinterPdfWriter::QPrinterPdfWriter(int widthMM, int heightMM, int dpi)
    :d_ptr(new QPrinterPdfWriterPrivate(widthMM, heightMM, dpi))
{

}

QPrinterPdfWriter::~QPrinterPdfWriter()
{
}


void QPrinterPdfWriter::setLineWidth(int width)
{
    if(d_ptr->painter)
    {
        QPen pen = d_ptr->painter->pen();
        pen.setWidth(width);
        d_ptr->painter->setPen(pen);
    }
}

void QPrinterPdfWriter::setFontSize(int size)
{
    if(d_ptr->painter)
    {
        QFont font= d_ptr->painter->font();
        font.setPixelSize(size);
        d_ptr->painter->setFont(font);
    }
}

void QPrinterPdfWriter::setStorkeColor(const QColor &color)
{
    if(d_ptr->painter)
    {
        QPen pen = d_ptr->painter->pen();
        pen.setColor(color);
        d_ptr->painter->setPen(pen);
    }
}

void QPrinterPdfWriter::drawRect(const QRectF &rect)
{
    if(d_ptr->painter)
    {
        d_ptr->painter->drawRect(rect);
    }
}

void QPrinterPdfWriter::drawText(const QPointF &pos, const QString &text)
{
    if(d_ptr->painter)
    {
        d_ptr->painter->drawText(pos, text);
    }
}

void QPrinterPdfWriter::drawDashLine(const QPointF &p1, const QPointF &p2, const QVector<quint16> &pattern, quint32 phase)
{
    if(d_ptr->painter)
    {
        d_ptr->painter->save();

        QPen pen = d_ptr->painter->pen();
        QVector<qreal> dashPat;
        foreach(quint16 v, pattern)
        {
            dashPat.append(v);
        }
        pen.setDashPattern(dashPat);
        pen.setDashOffset(phase);
        d_ptr->painter->setPen(pen);
        d_ptr->painter->drawLine(p1, p2);

        d_ptr->painter->restore();
    }
}

void QPrinterPdfWriter::drawLine(const QPointF &p1, const QPointF &p2)
{
    if(d_ptr->painter)
    {
        d_ptr->painter->drawLine(p1, p2);
    }
}

void QPrinterPdfWriter::drawPath(const QPainterPath &path)
{
    if(d_ptr->painter)
    {
        d_ptr->painter->drawPath(path);
    }
}

QRect QPrinterPdfWriter::pageRect() const
{
    if(d_ptr->printer)
    {
        QRect rect = d_ptr->printer->paperRect();
        int marginInPix = PageMarginMM * d_ptr->printer->resolution() / 25.4;
        return rect.adjusted(marginInPix, marginInPix, -marginInPix, -marginInPix);
    }
    return QRect();
}

void QPrinterPdfWriter::saveContent(QByteArray &content)
{
    if(d_ptr->painter)
    {
        d_ptr->painter->end();
        if(d_ptr->supportCustomEngine)
        {
            d_ptr->dev->close();
            content = d_ptr->content;
        }
        else
        {
            if(d_ptr->dev->open(QIODevice::ReadOnly))
            {
                content = d_ptr->dev->readAll();
                d_ptr->dev->close();
            }
            else
            {
                qdebug("Open pdf file failed.");
            }
        }
    }
}

void QPrinterPdfWriter::saveState()
{
    if(d_ptr->painter)
    {
        d_ptr->painter->save();
    }
}

void QPrinterPdfWriter::restoreState()
{
    if(d_ptr->painter)
    {
        d_ptr->painter->restore();
    }
}

void QPrinterPdfWriter::setFont(const QFont &font)
{
    if(d_ptr->painter)
    {
        d_ptr->painter->setFont(font);
    }
}
