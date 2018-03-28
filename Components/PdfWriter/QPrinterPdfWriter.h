#pragma once
#include "PdfWriterBase.h"
#include <QScopedPointer>

class QPrinterPdfWriterPrivate;
class QPrinterPdfWriter : public PdfWriterBase
{
public:
    QPrinterPdfWriter(int widthMM, int heightMM, int dpi);
    ~QPrinterPdfWriter();

    ///// begin interface of PdfWriterBase
    //set the line width
    void setLineWidth(int width);
    //set the font size;
    void setFontSize(int size);

    //set the color
    void setStorkeColor(const QColor &color);

    //draw a rectangle
    void drawRect(const QRectF &rect);

    //draw text
    void drawText(const QPointF &pos, const QString &text);

    //draw dash line
    void drawDashLine(const QPointF &p1, const QPointF &p2, const QVector<quint16> &pattern, quint32 phase);

    //draw line
    void drawLine(const QPointF &p1, const QPointF &p2);

    //draw path
    void drawPath(const QPainterPath &path);

    QRect pageRect() const;

    void saveContent(QByteArray &content);

    void saveState();
    void restoreState();
    //// end interface of PdfWriterBase

    void setFont(const QFont &font);


private:
    QScopedPointer<QPrinterPdfWriterPrivate> d_ptr;

};
