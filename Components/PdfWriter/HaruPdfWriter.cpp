#include "HaruPdfWriter.h"
#include "hpdf.h"
#include <QDebug>
#include "Utility.h"
#include <QMap>
#include <QElapsedTimer>
#include <QFile>
#include <stdlib.h>
#include "hpdf_conf.h"

#define TEMPFILE "/tmp/temp.pdf"
#define DEFAULT_DPI (72.0)
static inline int mmToPoints(int mm) { return mm * DEFAULT_DPI / 25.4; }

class HaruPdfWriterPrivate
{
public:
    HaruPdfWriterPrivate(int widthMM, int heightMM);
    ~HaruPdfWriterPrivate();
    const char *standardFontName(HaruPdfWriter::StandardFont font);

    static void errorHandling(HPDF_STATUS errorNo, HPDF_STATUS detailNo, void *userData);

    static void *waitMemAlloc(quint32 size);
    static void unwaitMemFree(void *ptr);

    static HPDF_Doc pdf;
    static QMap<QString, QString> loadFonts;
    HPDF_Page curPage;
    int width;
    int height;
    int curDPI;
    int curFontSize;
    int curLineWidth;
    QString curFontname;
};

HPDF_Doc HaruPdfWriterPrivate::pdf = NULL;
QMap<QString, QString> HaruPdfWriterPrivate::loadFonts;

HaruPdfWriterPrivate::HaruPdfWriterPrivate(int widthMM, int heightMM)
    :curPage(NULL), width(widthMM), height(heightMM),
      curDPI(DEFAULT_DPI), curFontSize(16), curLineWidth(1)
{
    if(pdf == NULL)
    {
        //pdf = HPDF_New(errorHandling, this);
        pdf = HPDF_NewEx(errorHandling, waitMemAlloc, unwaitMemFree, 0, this);
        HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
        HPDF_UseUTFEncodings(pdf);
        HPDF_SetCurrentEncoder(pdf, "UTF-8");
    }
    else
    {
        //reuse the load font and encoder resources
        HPDF_NewDoc(pdf);
    }

    curPage = HPDF_AddPage(pdf);

    width = mmToPoints(widthMM);
    height = mmToPoints(heightMM);

    HPDF_Page_SetWidth(curPage, width);
    HPDF_Page_SetHeight(curPage, height);


    curFontname = standardFontName(HaruPdfWriter::Courier);
    HPDF_Font standardfont = HPDF_GetFont(pdf, qPrintable(curFontname), NULL);
    HPDF_Page_SetFontAndSize(curPage, standardfont, curFontSize);

    HPDF_Page_GSave(curPage);
}

HaruPdfWriterPrivate::~HaruPdfWriterPrivate()
{
}

const char *HaruPdfWriterPrivate::standardFontName(HaruPdfWriter::StandardFont font)
{
    switch(font)
    {
    case HaruPdfWriter::Courier:
        return "Courier";
    case HaruPdfWriter::CourierBold:
        return "Courier-Bold";
    case HaruPdfWriter::CourierOblique:
        return "Courier-Oblique";
    case HaruPdfWriter::CourierBoldOblique:
        return  "Courier-BoldOblique";
    case HaruPdfWriter::Helvetica:
        return "Helvetica";
    case HaruPdfWriter::HelveticaBold:
        return "Helvetica-Bold";
    case HaruPdfWriter::HelveticaOblique:
        return "Helvetica-Oblique";
    case HaruPdfWriter::HelveticaBoldOblique:
        return "Helvetica-BoldOblique";
    case HaruPdfWriter::TimesRoman:
        return "Times-Roman";
    case HaruPdfWriter::TimesBold:
        return "Times-Bold";
    case HaruPdfWriter::TimesItalic:
        return "Times-Italic";
    case HaruPdfWriter::TimesBoldItalic:
        return "Times-BoldItalic";
    case HaruPdfWriter::Symbol:
        return "Symbol";
    case HaruPdfWriter::ZapfDingbats:
        return "ZapfDingbats";
    default:
        return NULL;
    }
}

void HaruPdfWriterPrivate::errorHandling(HPDF_STATUS errorNo, HPDF_STATUS detailNo, void *userData)
{
    qDebug()<<Q_FUNC_INFO<<"Pdf error: "<<errorNo<<", "<<detailNo;

    HaruPdfWriterPrivate *d = reinterpret_cast<HaruPdfWriterPrivate *>(userData);

    if(d->pdf)
    {
        HPDF_Free(d->pdf);
        d->pdf = NULL;
        d->curPage = NULL;
    }
}

void *HaruPdfWriterPrivate::waitMemAlloc(quint32 size)
{
    void *ptr = ::malloc(size);
    if(size >= HPDF_STREAM_BUF_SIZ)
    {
        Util::waitInEventLoop(5);
    }
    return ptr;
}

void HaruPdfWriterPrivate::unwaitMemFree(void *ptr)
{
    ::free(ptr);
}

HaruPdfWriter::HaruPdfWriter(int widthMM, int heightMM)
    :d_ptr(new HaruPdfWriterPrivate(widthMM, heightMM))
{
}

HaruPdfWriter::~HaruPdfWriter()
{

}

void HaruPdfWriter::setResolution(int dpi)
{
    if(d_ptr->curPage)
    {
        d_ptr->curDPI = dpi;
        //restore the state
        HPDF_Page_GRestore(d_ptr->curPage); //reset default
        HPDF_Page_Concat(d_ptr->curPage, DEFAULT_DPI / dpi, 0, 0, DEFAULT_DPI / dpi, 0, 0);
        HPDF_Page_GSave(d_ptr->curPage); //save
    }
}

void HaruPdfWriter::setLineWidth(int width)
{
    if(d_ptr->curPage && d_ptr->curLineWidth != width)
    {
        d_ptr->curLineWidth = width;
        HPDF_Page_SetLineWidth(d_ptr->curPage, width);
    }
}

void HaruPdfWriter::drawRect(const QRectF &rect)
{
    if(d_ptr->curPage)
    {
        HPDF_Page_GSave(d_ptr->curPage); //save
        HPDF_Page_Concat(d_ptr->curPage, 1, 0, 0, -1, 0,  d_ptr->curDPI * d_ptr->height / DEFAULT_DPI - d_ptr->curLineWidth);
        HPDF_Page_Rectangle(d_ptr->curPage,
                            rect.x(),
                            rect.y(),
                            rect.width(),
                            rect.height());
        HPDF_Page_Stroke(d_ptr->curPage);
        HPDF_Page_GRestore(d_ptr->curPage); //reset default
    }
}

void HaruPdfWriter::setFontAndSize(HaruPdfWriter::StandardFont font, int size)
{
    if(d_ptr->curPage)
    {
        d_ptr->curFontSize  = size;
        d_ptr->curFontname = d_ptr->standardFontName(font);
        HPDF_Font standardfont = HPDF_GetFont(d_ptr->pdf, qPrintable(d_ptr->curFontname), NULL);
        HPDF_Page_SetFontAndSize(d_ptr->curPage, standardfont, size);
    }
}

QString HaruPdfWriter::loadTrueTypeFont(const QString &filepath)
{
    QString fontname;
    if(d_ptr->pdf)
    {
       if(d_ptr->loadFonts.contains(filepath))
       {
           fontname = d_ptr->loadFonts.value(filepath);
       }
       else
       {
           fontname = HPDF_LoadTTFontFromFile( d_ptr->pdf, qPrintable(filepath), HPDF_TRUE);
           d_ptr->loadFonts.insert(filepath, fontname);
       }
    }
    return fontname;
}

void HaruPdfWriter::setFontAndSize(const QString &fontname, int size)
{
    if(d_ptr->curPage)
    {
        d_ptr->curFontSize = size;
        d_ptr->curFontname = fontname;
        HPDF_Font font = HPDF_GetFont(d_ptr->pdf, qPrintable(fontname), "UTF-8");
        HPDF_Page_SetFontAndSize(d_ptr->curPage, font, size);
    }
}

void HaruPdfWriter::setFontSize(int size)
{
    if(d_ptr->curPage)
    {
        d_ptr->curFontSize = size;
        HPDF_Font font = HPDF_GetFont(d_ptr->pdf, qPrintable(d_ptr->curFontname), "UTF-8");
        HPDF_Page_SetFontAndSize(d_ptr->curPage, font, size);
    }
}

void HaruPdfWriter::setStorkeColor(const QColor &color)
{
    if(d_ptr->curPage)
    {
        HPDF_Page_SetRGBStroke(d_ptr->curPage, color.red() / 255.0, color.green() /255.0, color.green()/255.0);
    }
}

void HaruPdfWriter::drawText(const QPointF &pos, const QString &text)
{
    if(d_ptr->curPage)
    {
        HPDF_Page_GSave(d_ptr->curPage);
        //use qt axis
        HPDF_Page_Concat(d_ptr->curPage, 1, 0, 0, 1, pos.x(),  d_ptr->curDPI * d_ptr->height / DEFAULT_DPI - pos.y());
        HPDF_Page_BeginText(d_ptr->curPage);

        HPDF_Page_TextOut(d_ptr->curPage, 0, 0, text.toUtf8().constData());

        HPDF_Page_EndText(d_ptr->curPage);
        HPDF_Page_GRestore(d_ptr->curPage);
    }
}

void HaruPdfWriter::drawDashLine(const QPointF &p1, const QPointF &p2, const QVector<quint16>& pattern, quint32 phase)
{
    if(d_ptr->curPage)
    {
        HPDF_Page_GSave(d_ptr->curPage); //save
        HPDF_Page_Concat(d_ptr->curPage, 1, 0, 0, -1, 0,  d_ptr->curDPI * d_ptr->height / DEFAULT_DPI - d_ptr->curLineWidth);

        HPDF_Page_SetDash(d_ptr->curPage, pattern.constData(), pattern.size(), phase);
        HPDF_Page_MoveTo(d_ptr->curPage, p1.x(), p1.y());
        HPDF_Page_LineTo(d_ptr->curPage, p2.x(), p2.y());
        HPDF_Page_Stroke(d_ptr->curPage);

        HPDF_Page_GRestore(d_ptr->curPage); //reset default
    }
}

void HaruPdfWriter::drawLine(const QPointF &p1, const QPointF &p2)
{
    if(d_ptr->curPage)
    {
        HPDF_Page_GSave(d_ptr->curPage); //save
        HPDF_Page_Concat(d_ptr->curPage, 1, 0, 0, -1, 0,  d_ptr->curDPI * d_ptr->height / DEFAULT_DPI - d_ptr->curLineWidth);

        HPDF_Page_MoveTo(d_ptr->curPage, p1.x(), p1.y());
        HPDF_Page_LineTo(d_ptr->curPage, p2.x(), p2.y());
        HPDF_Page_Stroke(d_ptr->curPage);

        HPDF_Page_GRestore(d_ptr->curPage); //reset default
    }
}

void HaruPdfWriter::drawPath(const QPainterPath &path)
{
    int count = path.elementCount();
    if(d_ptr->curPage && count > 0)
    {
        HPDF_Page_GSave(d_ptr->curPage); //save
        HPDF_Page_Concat(d_ptr->curPage, 1, 0, 0, -1, 0,  d_ptr->curDPI * d_ptr->height / DEFAULT_DPI - d_ptr->curLineWidth);

        int start = -1;
        for(int i = 0; i < count; ++i)
        {
            const QPainterPath::Element &elm = path.elementAt(i);
            switch(elm.type)
            {
            case QPainterPath::MoveToElement:
                if(start >= 0
                        && path.elementAt(start).x == path.elementAt(i-1).x
                        && path.elementAt(start).y == path.elementAt(i-1).y)
                {
                    HPDF_Page_ClosePath(d_ptr->curPage);
                }
                HPDF_Page_MoveTo(d_ptr->curPage, elm.x, elm.y);
                start = i;
                break;
            case QPainterPath::LineToElement:
                HPDF_Page_LineTo(d_ptr->curPage, elm.x, elm.y);
                break;
            case QPainterPath::CurveToElement:
                HPDF_Page_CurveTo(d_ptr->curPage, elm.x, elm.y,
                                  path.elementAt(i+1).x, path.elementAt(i+1).y,
                                  path.elementAt(i+2).x, path.elementAt(i+2).y);
                break;
            default:
                qDebug()<<Q_FUNC_INFO<<"unhandled type:"<<elm.type;
                break;
            }
        }
        if(start >= 0
                && path.elementAt(start).x == path.elementAt(count-1).x
                && path.elementAt(start).y == path.elementAt(count-1).y)
        {
            HPDF_Page_ClosePath(d_ptr->curPage);
        }

        HPDF_Page_Stroke(d_ptr->curPage);

        HPDF_Page_GRestore(d_ptr->curPage); //reset default
    }
}

int HaruPdfWriter::pageWidth() const
{
    return d_ptr->width * d_ptr->curDPI / DEFAULT_DPI;
}

int HaruPdfWriter::pageHeight() const
{
    return d_ptr->height * d_ptr->curDPI / DEFAULT_DPI;
}

QRect HaruPdfWriter::pageRect() const
{
    int marginInPix = PageMarginMM * d_ptr->curDPI / 25.4;
    int width = d_ptr->width * d_ptr->curDPI / DEFAULT_DPI;
    int height = d_ptr->height * d_ptr->curDPI / DEFAULT_DPI;
    QRect rect(0, 0, width, height);

    return rect.adjusted(marginInPix, marginInPix, -marginInPix, -marginInPix);
}

void HaruPdfWriter::saveContent(QByteArray &content)
{
    QElapsedTimer timer;
    timer.start();
    if(d_ptr->pdf && (HPDF_SaveToStream(d_ptr->pdf) == HPDF_OK))
    {
        qDebug()<<"save pdf stream time "<<timer.elapsed()<<" ms";
        HPDF_UINT32 size = HPDF_GetStreamSize(d_ptr->pdf);
        if(size > 0)
        {
            content.resize(size);
            HPDF_ReadFromStream(d_ptr->pdf, (HPDF_BYTE*)content.data(), &size);
        }
    }
}

void HaruPdfWriter::saveState()
{
    if(d_ptr->curPage)
    {
        HPDF_Page_GSave(d_ptr->curPage);
    }
}

void HaruPdfWriter::restoreState()
{
    if(d_ptr->curPage)
    {
        HPDF_Page_GRestore(d_ptr->curPage);
    }
}
