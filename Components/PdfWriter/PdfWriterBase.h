#pragma once
#include <QRectF>
#include <QPainterPath>
#include <QColor>


class PdfWriterBase
{
public:
    virtual ~PdfWriterBase() {}

    enum PageMarginType {
        PageMarginMM = 15
    };

    //set the line width
    virtual void setLineWidth(int width) = 0;
    //set the font size;
    virtual void setFontSize(int size) = 0;

    //set color
    virtual void setStorkeColor(const QColor &color) = 0;

    //draw a rectangle
    virtual void drawRect(const QRectF &rect) = 0;

    //draw text
    virtual void drawText(const QPointF &pos, const QString &text) = 0;

    //draw dash line
    virtual void drawDashLine(const QPointF &p1, const QPointF &p2, const QVector<quint16> &pattern, quint32 phase) = 0;

    //draw line
    virtual void drawLine(const QPointF &p1, const QPointF &p2) = 0;

    //draw path
    virtual void drawPath(const QPainterPath &path) = 0;

    //page rect
    virtual QRect pageRect() const = 0;

    //save the pdf to the content
    virtual void saveContent(QByteArray &content) = 0;

    //save current graphic state
    virtual void saveState() = 0;
    //restore graphic state
    virtual void restoreState() = 0;
};
