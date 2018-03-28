#include "Debug.h"
#include "PrintPage.h"

/**************************************************************************************************
 * 设置ID。
 *************************************************************************************************/
void PrintPage::setID(const QString &id)
{
    _id = id;
}

/**************************************************************************************************
 * 返回ID。
 *************************************************************************************************/
const QString &PrintPage::getID(void)
{
    return _id;
}

/**************************************************************************************************
 * 构造函数
 *************************************************************************************************/
PrintPage::PrintPage(int width)
    : QImage(width, IMAGE_HEIGHT, QImage::Format_MonoLSB),
      _drawHeight(0),
      _speed(PRINT_SPEED_NR)
{
    QPainter painter(this);
    fill(painter.brush().color());
}

/**************************************************************************************************
 * 构造函数
 *************************************************************************************************/
PrintPage::PrintPage(const QImage &image)
    : QImage(image),
      _drawHeight(0),
      _speed(PRINT_SPEED_NR)
{
}

/**************************************************************************************************
 * 析构函数
 *************************************************************************************************/
PrintPage::~PrintPage()
{

}

void PrintPage::setDrawHeight(const int height)
{
    _drawHeight = height;
}

int PrintPage::getDrawHeight() const
{
    return _drawHeight;
}

void PrintPage::setPrintSpeed(PrintSpeed speed)
{
    _speed = speed;
}

PrintSpeed PrintPage::getPrintSpeed() const
{
    return _speed;
}
