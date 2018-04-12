#pragma once
#include <QPen>
#include <QImage>
#include <QPainter>
#include <QString>
#include "FontManager.h"
#include "PrintDefine.h"

#define PIXELS_PER_MM       (8)     // pixel
#define PRINT_PAGE_HEIGHT  (48)    // mm
#define PRINT_PAGE_WIDTH   (90)    // mm

#define IMAGE_WIDTH  (PIXELS_PER_MM * PRINT_PAGE_WIDTH)
#define IMAGE_HEIGHT (PIXELS_PER_MM * PRINT_PAGE_HEIGHT)
/**************************************************************************************************
 * 抽象打印页
 *************************************************************************************************/
class PrintPage : public QImage
{
public:
    // id控制。
    void setID(const QString &id);
    const QString &getID(void);

    PrintPage(int width = IMAGE_WIDTH);
    PrintPage(const QImage &image);
    virtual ~PrintPage();

    // set the height that content has been drawn
    void setDrawHeight(const int height);
    // get the height that content has been drawn
    int getDrawHeight() const;

    //set page print speed
    void setPrintSpeed(PrintSpeed speed);
    //get page print speed
    PrintSpeed getPrintSpeed() const;

private:
     QString _id;
     int _drawHeight;
     PrintSpeed _speed;
};
