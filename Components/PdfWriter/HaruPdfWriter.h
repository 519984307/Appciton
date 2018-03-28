#ifndef PDFWRITER_H
#define PDFWRITER_H

#include "PdfWriterBase.h"
#include <QScopedPointer>
#include <QRect>
#include <QByteArray>
#include <QVector>
#include <QPainterPath>

class HaruPdfWriterPrivate;
class HaruPdfWriter : public PdfWriterBase
{
public:
    enum StandardFont {
        Courier,
        CourierBold,
        CourierOblique,
        CourierBoldOblique,
        Helvetica,
        HelveticaBold,
        HelveticaOblique,
        HelveticaBoldOblique,
        TimesRoman,
        TimesBold,
        TimesItalic,
        TimesBoldItalic,
        Symbol,
        ZapfDingbats,
    };

    HaruPdfWriter(int widthMM, int heightMM);

    ~HaruPdfWriter();

    void setResolution(int dpi);
    void setLineWidth(int width);
    void drawRect(const QRectF &rect);
    void setFontAndSize(StandardFont font, int size);
    //load the true type font and return the font name
    QString loadTrueTypeFont(const QString &filepath);

    void setFontAndSize(const QString &fontname, int size);

    //set the font size;
    void setFontSize(int size);

    void setStorkeColor(const QColor &color);

    void drawText(const QPointF &pos, const QString &text);

    void drawDashLine(const QPointF &p1, const QPointF &p2, const QVector<quint16> &pattern, quint32 phase);

    void drawLine(const QPointF &p1, const QPointF &p2);

    void drawPath(const QPainterPath &path);

    //page pixel width for current dpi
    int pageWidth() const;
    //page pixel height for current dpi
    int pageHeight() const;

    QRect pageRect() const;

    void saveContent(QByteArray &content);

    void saveState();
    void restoreState();

private:
    QScopedPointer<HaruPdfWriterPrivate> d_ptr;
};

#endif // PDFWRITER_H
