#include <QPainter>
#include "ILabel.h"
#include "FontManager.h"
#include "Debug.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ILabel::ILabel():QLabel(), _boarder(true), _antialiasing(true),
    _boarderColor(Qt::white), _bgColor(Qt::black),
    _fontColor(Qt::white), _direction(ILABEL_SINGLE_TEXT), _radius(8.0),
    _alignFlag(Qt::AlignCenter), _leftMargin(0), _topMargin(0), _rightMargin(0),
    _bottomMargin(0), _spacing(3), _picWidth(24), _picHight(24), _pic(QPixmap())
{
    _textList.clear();
    _fontSize.clear();
    _fontBold.clear();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ILabel::~ILabel()
{
    _textList.clear();
    _fontSize.clear();
    _fontBold.clear();
}

/**************************************************************************************************
 * 设置是否显示边框。
 *************************************************************************************************/
void ILabel::setBoarderEnable(bool enable)
{
    _boarder = enable;
}

/**************************************************************************************************
 * 设置是否抗锯齿。
 *************************************************************************************************/
void ILabel::setAntialiasingEnable(bool enable)
{
    _antialiasing = enable;
}

/**************************************************************************************************
 * 设置边框宽度。
 *************************************************************************************************/
void ILabel::setBoarderWidth(int width)
{
    _boarderWidth = width;
}

/**************************************************************************************************
 * 设置边框颜色
 *************************************************************************************************/
void ILabel::setBoarderColor(const QColor &color)
{
    _boarderColor = color;
}

/**************************************************************************************************
 * 设置背景颜色
 *************************************************************************************************/
void ILabel::setBackgroundColor(const QColor &color)
{
    _bgColor = color;
}

/**************************************************************************************************
 * 设置字体颜色
 *************************************************************************************************/
void ILabel::setFontColor(const QColor &color)
{
    _fontColor = color;
}

/**************************************************************************************************
 * 设置显示方向
 *************************************************************************************************/
void ILabel::setDirection(unsigned char direction)
{
    _direction = direction;
}

/**************************************************************************************************
 * 设置对齐方式
 *************************************************************************************************/
void ILabel::setAlign(unsigned flag)
{
    _alignFlag = flag;
}

/**************************************************************************************************
 * 设置字体大小
 *************************************************************************************************/
void ILabel::addFontSize(int fontsize)
{
    _fontSize.append(fontsize);
}

/**************************************************************************************************
 * 设置字体大小
 *************************************************************************************************/
void ILabel::changeFontSize(int fontSize, int index)
{
    if (index >= _fontSize.size())
    {
        return;
    }

    _fontSize[index] = fontSize;
}

/**************************************************************************************************
 * 设置字体加粗
 *************************************************************************************************/
void ILabel::addFontBold(bool bold)
{
    _fontBold.append(bold);
}

/**************************************************************************************************
 * 设置字体加粗
 *************************************************************************************************/
void ILabel::changeFontBold(bool bold, int index)
{
    if (index >= _fontBold.size())
    {
        return;
    }

    _fontBold[index] = bold;
}

/**************************************************************************************************
 * 设置显示内容
 *************************************************************************************************/
void ILabel::setText(const QStringList &textList)
{
    _textList.clear();
    _textList.append(textList);
    update();
}

/**************************************************************************************************
 * 设置显示内容
 *************************************************************************************************/
void ILabel::setText(const QString &text)
{
    _textList.clear();
    _textList.append(text);
    update();
}

/**************************************************************************************************
 * 设置显示内容,显示内容包含图片
 *************************************************************************************************/
void ILabel::setPic(const QPixmap &picture)
{
    _pic = picture;
    update();
}

/**************************************************************************************************
 * 设置水平偏移
 *************************************************************************************************/
void ILabel::setMargin(int left, int top, int right, int bottom)
{
    _leftMargin = left;
    _topMargin = top;
    _rightMargin = -right;
    _bottomMargin = -bottom;
}

/**************************************************************************************************
 * 设置水平偏移
 *************************************************************************************************/
void ILabel::setSpacing(int spacing)
{
    _spacing = spacing;
}

/**************************************************************************************************
 * 设置图片宽度
 *************************************************************************************************/
void ILabel::setPicSize(int width, int height)
{
    _picWidth = width;
    _picHight = height;
}

/**************************************************************************************************
 * 设置圆角的半径。
 *************************************************************************************************/
void ILabel::setRadius(double radius)
{
    _radius = radius;
}

/**************************************************************************************************
 * 清空
 *************************************************************************************************/
void ILabel::clear()
{
    _textList.clear();
    _pic = QPixmap();
    update();
}

/**************************************************************************************************
 * 绘画
 *************************************************************************************************/
void ILabel::paintEvent(QPaintEvent */*e*/)
{
    QPainter painter(this);

    //边框
    if (_boarder)
    {
        QPen pen(_boarderColor);
        pen.setWidth(_boarderWidth);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::Antialiasing, _antialiasing);
    }
    else
    {
        painter.setPen(Qt::NoPen);
        painter.setRenderHint(QPainter::Antialiasing, _antialiasing);
    }

    //背景
    QRect r = rect();
    r.adjust(2, 2, -2, -2);
    painter.setBrush(_bgColor);
    painter.drawRoundedRect(r, _radius, _radius);

    painter.setPen(_fontColor);
    _drawContent(painter, r);
}

/**************************************************************************************************
 * 绘画内容
 *************************************************************************************************/
void ILabel::_drawContent(QPainter &painter, QRect r)
{
    r.adjust(_leftMargin, _topMargin, _rightMargin, _bottomMargin);

    switch (_direction)
    {
        case ILABEL_SINGLE_TEXT:
            if (!_textList.isEmpty())
            {
                int fontSize = fontManager.getFontSize(4);
                bool bold = false;
                if (!_fontSize.isEmpty())
                {
                    fontSize = _fontSize[0];
                }

                if (!_fontBold.isEmpty())
                {
                    bold = _fontBold[0];
                }

                //if text too long,change fontSize
                fontSize = _adjustFontSizeToWidth(_textList.at(0), r.width(), fontSize, bold);
                painter.setFont(fontManager.textFont(fontSize, bold));
                painter.drawText(r, _alignFlag, _textList.at(0));
            }
            break;
        case ILABEL_SINGLE_PIC:
            _drawPic(painter, r, _alignFlag);
            break;
        case ILABEL_HORIZOL_TEXT_PIC:
            _drawHrizonTextPic(painter, r);
            break;
        case ILABEL_HORIZOL_PIC_TEXT:
            _drawHrizonPicText(painter, r);
            break;
        case ILABEL_HORIZOL_MULTI_TEXT:
            _drawHrizonText(painter, r);
            break;
        case ILABEL_VERTICAL_TEXT_PIC:
            _drawVerticalTextPic(painter, r);
            break;
        case ILABEL_VERTICAL_PIC_TEXT:
            _drawVerticalPicText(painter, r);
            break;
        case ILABEL_VERTICAL_MULTI_TEXT:
            _drawVerticalText(painter, r);
            break;
        default:
            break;
    }
}

/**************************************************************************************************
 * 改变文字大小
 * 参数:
 * txt:显示的文本
 * width:控件宽度
 * fontSize:原始字体大小
 * 返回:
 * 调整后的字体大小
 *************************************************************************************************/
int ILabel::_adjustFontSizeToWidth(const QString &txt, int width, int fontSize, bool bold)
{
    QFont font = fontManager.textFont(fontSize, bold);
    int strlen = fontManager.textWidthInPixels(txt, font);
    while (strlen > width && fontSize > 0)
    {
        --fontSize;
        font = fontManager.textFont(fontSize, bold);
        strlen = fontManager.textWidthInPixels(txt, font);
    }

    if (fontSize >= fontManager.getFontSize(0))
    {
        return fontSize;
    }
    else
    {
        return fontManager.getFontSize(0);
    }
}

/**************************************************************************************************
 * 改变文字大小
 * 参数:
 * txt:显示的文本
 * height:控件高度
 * fontSize:原始字体大小
 * 返回:
 * 调整后的字体大小
 *************************************************************************************************/
int ILabel::_adjustFontSizeToHeight(int height, int fontSize, bool bold)
{
    QFont font = fontManager.textFont(fontSize, bold);
    int fontH = fontManager.textHeightInPixels(font);
    while (fontH > height && fontSize > 0)
    {
        --fontSize;
        fontH = fontManager.textHeightInPixels(font);
    }

    if (fontSize >= fontManager.getFontSize(0))
    {
        return fontSize;
    }
    else
    {
        return fontManager.getFontSize(0);
    }
}

/**************************************************************************************************
 * 绘画图片
 *************************************************************************************************/
void ILabel::_drawPic(QPainter &painter, const QRect &r, unsigned alignFlag)
{
    if (_pic.isNull())
    {
        return;
    }

    QRect rscRect;
    QRect dstRect = r;
    rscRect.setWidth((_picWidth > r.width()) ? r.width() : _picWidth);
    rscRect.setHeight((_picHight > r.height()) ? r.height() : _picHight);

    if (alignFlag & Qt::AlignHCenter)
    {
        dstRect.setX(r.x() + (r.width() - rscRect.width()) / 2);
    }
    else if (alignFlag & Qt::AlignRight)
    {
        dstRect.setX(r.x() + r.width() - rscRect.width());
    }

    if (alignFlag & Qt::AlignVCenter)
    {
        dstRect.setY(r.y() + (r.height() - rscRect.height()) / 2);
    }
    else if (alignFlag & Qt::AlignBottom)
    {
        dstRect.setY(r.y() + r.height() - rscRect.height());
    }

    dstRect.setWidth(rscRect.width());
    dstRect.setHeight(rscRect.height());
    painter.drawPixmap(dstRect, _pic);
}

/**************************************************************************************************
 * 绘画水平图片及文本
 *************************************************************************************************/
void ILabel::_drawHrizonTextPic(QPainter &painter, const QRect &rect)
{
    if (_textList.isEmpty())
    {
        _drawPic(painter, rect, _alignFlag);
        return;
    }

    int fontSize = fontManager.getFontSize(4);
    bool bold = false;
    QString text = _textList.at(0);
    if (!_fontSize.isEmpty())
    {
        fontSize = _fontSize[0];
    }

    if (!_fontBold.isEmpty())
    {
        bold = _fontBold[0];
    }

    QFont font = fontManager.textFont(fontSize, bold);
    int fontH = fontManager.textHeightInPixels(font);
    painter.setFont(font);
    if (_pic.isNull())
    {
        painter.drawText(rect, _alignFlag, text);
        return;
    }

    int strLen = fontManager.textWidthInPixels(text, font);
    int contentLen = strLen + _spacing + _picWidth;
    int tX = 0, tY = 0, tW = 0, pX = 0, pY = 0, pW = 0, pH = rect.height();
    if (contentLen > rect.width())
    {
        if (strLen + _spacing > rect.width())
        {
            fontSize = _adjustFontSizeToWidth(text, rect.width(), fontSize, bold);
            painter.setFont(fontManager.textFont(fontSize, bold));
            strLen = fontManager.textWidthInPixels(text, font);
            tW = (strLen > rect.width()) ? rect.width() : strLen;
            pW = _picWidth;
            pX = rect.width() - pW;
        }
        else
        {
            tW = strLen;
            pW = rect.width() - _spacing - strLen;
            pX = tX + tW + _spacing;
        }
    }
    else
    {
        tW = strLen;
        pW = _picWidth;
        if (_alignFlag & Qt::AlignHCenter)
        {
            tX = (rect.width() - contentLen) / 2;
        }
        else if (_alignFlag & Qt::AlignRight)
        {
            tX = rect.width() - contentLen;
        }
        pX = tX + tW + _spacing;
    }

    if (_alignFlag & Qt::AlignVCenter)
    {
        tY = (rect.height() - fontH) / 2;
        if (rect.height() > _picHight)
        {
            pY = (rect.height() - _picHight) / 2;
            pH = _picHight;
        }
    }
    else if (_alignFlag & Qt::AlignBottom)
    {
        tY = rect.height() - fontH;
        if (rect.height() > _picHight)
        {
            pY = rect.height() - _picHight;
            pH = _picHight;
        }
    }
    else
    {
        if (rect.height() > _picHight)
        {
            pH = _picHight;
        }
    }

    painter.drawText(QRect(tX + rect.x(), tY + rect.y(), tW, fontH), text);
    painter.drawPixmap(QRect(pX + rect.x(), pY + rect.y(), pW, pH), _pic);
}

/**************************************************************************************************
 * 绘画水平图片及文本
 *************************************************************************************************/
void ILabel::_drawHrizonPicText(QPainter &painter, const QRect &rect)
{
    if (_textList.isEmpty())
    {
        _drawPic(painter, rect, _alignFlag);
        return;
    }

    int fontSize = fontManager.getFontSize(4);
    bool bold = false;
    QString text = _textList.at(0);
    if (!_fontSize.isEmpty())
    {
        fontSize = _fontSize[0];
    }

    if (!_fontBold.isEmpty())
    {
        bold = _fontBold[0];
    }

    QFont font = fontManager.textFont(fontSize, bold);
    int fontH = fontManager.textHeightInPixels(font);
    painter.setFont(font);
    if (_pic.isNull())
    {
        painter.drawText(rect, _alignFlag, text);
        return;
    }

    int strLen = fontManager.textWidthInPixels(text, font);
    int contentLen = strLen + _spacing + _picWidth;
    int tX = 0, tY = 0, tW = 0, pX = 0, pY = 0, pW = 0, pH = rect.height();
    if (contentLen > rect.width())
    {
        if (_picWidth + _spacing > rect.width())
        {
            pW = (_picWidth > rect.width()) ? rect.width() : _picWidth;
            fontSize = _adjustFontSizeToWidth(text, rect.width(), fontSize, bold);
            font = fontManager.textFont(fontSize, bold);
            strLen = fontManager.textWidthInPixels(text, font);
            tW = (strLen > rect.width()) ? rect.width() : strLen;
            tX = rect.width() - tW;
        }
        else
        {
            pW = _picWidth;
            tW = rect.width() - _spacing - _picWidth;
            tX = pX + pW + _spacing;
        }
    }
    else
    {
        tW = strLen;
        pW = _picWidth;
        if (_alignFlag & Qt::AlignHCenter)
        {
            pX = (rect.width() - contentLen) / 2;
        }
        else if (_alignFlag & Qt::AlignRight)
        {
            pX = rect.width() - contentLen;
        }
        tX = pX + pW + _spacing;
    }

    if (_alignFlag & Qt::AlignVCenter)
    {
        tY = (rect.height() - fontH) / 2;
        if (rect.height() > _picHight)
        {
            pY = (rect.height() - _picHight) / 2;
            pH = _picHight;
        }
    }
    else if (_alignFlag & Qt::AlignBottom)
    {
        tY = rect.height() - fontH;
        if (rect.height() > _picHight)
        {
            pY = rect.height() - _picHight;
            pH = _picHight;
        }
    }
    else
    {
        if (rect.height() > _picHight)
        {
            pH = _picHight;
        }
    }

    painter.drawPixmap(QRect(pX + rect.x(), pY + rect.y(), pW, pH), _pic);
    painter.drawText(QRect(tX + rect.x(), tY + rect.y(), tW, fontH), text);
}

/**************************************************************************************************
 * 绘画水平文本
 *************************************************************************************************/
void ILabel::_drawHrizonText(QPainter &painter, const QRect &rect)
{
    if (_textList.isEmpty())
    {
        return;
    }

    QFont font;
    int contentLen = 0;
    int count = _textList.count();
    int fontSize = 0;
    bool bold = false;
    QString text;
    for (int i = 0; i < count; ++i)
    {
        text = _textList.at(i);
        fontSize = fontManager.getFontSize(4);
        bold = false;
        if (i < _fontSize.size())
        {
            fontSize = _fontSize[i];
        }

        if (i < _fontBold.size())
        {
            bold = _fontBold[i];
        }

        font = fontManager.textFont(fontSize, bold);
        contentLen += fontManager.textWidthInPixels(text, font);
        contentLen += _spacing;
    }

    // 间隔比项数少一个
    contentLen -= _spacing;
    int startX = 0;
    if (contentLen < rect.width())
    {
        if (_alignFlag & Qt::AlignHCenter)
        {
            startX = (rect.width() - contentLen) / 2;
        }
        else if (_alignFlag & Qt::AlignRight)
        {
            startX = rect.width() - contentLen;
        }
    }
    startX += rect.x();

    contentLen = 0;
    int strlen = 0, fontH = 0, tY = 0;
    for (int i = 0; i < count; ++i)
    {
        text = _textList.at(i);
        fontSize = fontManager.getFontSize(4);
        bold = false;
        if (i < _fontSize.size())
        {
            fontSize = _fontSize[i];
        }

        if (i < _fontBold.size())
        {
            bold = _fontBold[i];
        }
        font = fontManager.textFont(fontSize, bold);
        strlen = fontManager.textWidthInPixels(text, font);
        fontH = fontManager.textHeightInPixels(font);

        tY = 0;
        if (_alignFlag & Qt::AlignVCenter)
        {
            if (rect.height() > fontH)
            {
                tY = (rect.height() - fontH) / 2;
            }
        }
        else if (_alignFlag & Qt::AlignBottom)
        {
            if (rect.height() > fontH)
            {
                tY = rect.height() - fontH;
            }
        }

        painter.setFont(font);
        painter.drawText(QRect(startX, tY + rect.y(), strlen, fontH), text);
        contentLen += strlen + _spacing;
        startX += strlen + _spacing;
        if (contentLen > rect.width())
        {
            break;
        }
    }
}

/**************************************************************************************************
 * 绘画垂直图片及文本
 *************************************************************************************************/
void ILabel::_drawVerticalTextPic(QPainter &painter, const QRect &rect)
{
    if (_textList.isEmpty())
    {
        _drawPic(painter, rect, Qt::AlignCenter);
        return;
    }
    int fontSize = fontManager.getFontSize(4);
    bool bold = false;
    QString text = _textList.at(0);
    if (!_fontSize.isEmpty())
    {
        fontSize = _fontSize[0];
    }

    if (!_fontBold.isEmpty())
    {
        bold = _fontBold[0];
    }
    QFont font = fontManager.textFont(fontSize, bold);
    int fontH = fontManager.textHeightInPixels(font);
    painter.setFont(font);
    if (_pic.isNull())
    {
        painter.drawText(rect, _alignFlag, text);
        return;
    }

    int strLen = fontManager.textWidthInPixels(text, font);
    int contentH = fontH + _spacing + _picHight;
    int tX = 0, tY = 0, tW = rect.width(), tH = 0, pX = 0, pY = 0, pW = rect.width(), pH = 0;
    if (contentH > rect.height())
    {
        if (fontH + _spacing > rect.height())
        {
            fontSize = _adjustFontSizeToHeight(rect.height(), fontSize, bold);
            font = fontManager.textFont(fontSize, bold);
            fontH = fontManager.textHeightInPixels(font);
            painter.setFont(font);
            tH = (fontH > rect.height()) ? rect.height() : fontH;
            pH = (_picHight > rect.height()) ? rect.height() : _picHight;
            pY = rect.height() - pH;
        }
        else
        {
            tH = fontH;
            pH = rect.height() - _spacing - tH;
            pY = tY + tH + _spacing;
        }
    }
    else
    {
        tH = fontH;
        pH = _picHight;
        if (_alignFlag & Qt::AlignVCenter)
        {
            tY = (rect.height() - contentH) / 2;
        }
        else if (_alignFlag & Qt::AlignBottom)
        {
            tY = rect.height() - contentH;
        }

        pY = tY + tH + _spacing;
    }

    if (_alignFlag & Qt::AlignHCenter)
    {
        if (strLen < rect.width())
        {
            tX = (rect.width() - strLen) / 2;
            tW = strLen;
        }

        if (_picWidth < rect.width())
        {
            pX = (rect.width() - _picWidth) / 2;
            pW = _picWidth;
        }
    }
    else if (_alignFlag & Qt::AlignRight)
    {
        if (strLen < rect.width())
        {
            tX = rect.width() - strLen;
            tW = strLen;
        }

        if (_picWidth < rect.width())
        {
            pX = rect.width() - _picWidth;
            pW = _picWidth;
        }
    }
    else
    {
        if (_picWidth < rect.width())
        {
            pW = _picWidth;
        }
    }

    painter.drawText(QRect(tX + rect.x(), tY + rect.y(), tW, tH), text);
    painter.drawPixmap(QRect(pX + rect.x(), pY + rect.y(), pW, pH), _pic);
}

/**************************************************************************************************
 * 绘画垂直图片及文本
 *************************************************************************************************/
void ILabel::_drawVerticalPicText(QPainter &painter, const QRect &rect)
{
    if (_textList.isEmpty())
    {
        _drawPic(painter, rect, _alignFlag);
        return;
    }

    int fontSize = fontManager.getFontSize(4);
    bool bold = false;
    QString text = _textList.at(0);
    if (!_fontSize.isEmpty())
    {
        fontSize = _fontSize[0];
    }

    if (!_fontBold.isEmpty())
    {
        bold = _fontBold[0];
    }
    QFont font = fontManager.textFont(fontSize, bold);
    int fontH = fontManager.textHeightInPixels(font);
    painter.setFont(font);
    if (_pic.isNull())
    {
        painter.drawText(rect, _alignFlag, text);
        debug("picture size is too big\n");
        return;
    }

    int strLen = fontManager.textWidthInPixels(text, font);
    int contentH = fontH + _spacing + _picHight;
    int tX = 0, tY = 0, tW = rect.width(), tH = 0, pX = 0, pY = 0, pW = rect.width(), pH = 0;
    if (contentH > rect.height())
    {
        if (_spacing + _picHight > rect.height())
        {
            pH = rect.height();
        }
        else
        {
            pH = _picHight;
        }

        if (fontH > rect.height())
        {
            fontSize = _adjustFontSizeToHeight(rect.height(), fontSize, bold);
            font = fontManager.textFont(fontSize, bold);
            fontH = fontManager.textHeightInPixels(font);
            painter.setFont(font);
        }

        tH = (fontH > rect.height()) ? rect.height() : fontH;
        tY = rect.height() - tH;
    }
    else
    {
        pH = _picHight;
        tH = fontH;
        if (_alignFlag & Qt::AlignVCenter)
        {
            pY = (rect.height() - contentH) / 2;
        }
        else if (_alignFlag & Qt::AlignBottom)
        {
            pY = rect.height() - contentH;
        }

        tY = pY + pH + _spacing;
    }

    if (_alignFlag & Qt::AlignHCenter)
    {
        if (strLen < rect.width())
        {
            tX = (rect.width() - strLen) / 2;
            tW = strLen;
        }

        if (_picWidth < rect.width())
        {
            pX = (rect.width() - _picWidth) / 2;
            pW = _picWidth;
        }
    }
    else if (_alignFlag & Qt::AlignRight)
    {
        if (strLen < rect.width())
        {
            tX = rect.width() - strLen;
            tW = strLen;
        }

        if (_picWidth < rect.width())
        {
            pX = rect.width() - _picWidth;
            pW = _picWidth;
        }
    }
    else
    {
        if (_picWidth < rect.width())
        {
            pW = _picWidth;
        }
    }

    painter.drawText(QRect(tX + rect.x(), tY + rect.y(), tW, tH), text);
    painter.drawPixmap(QRect(pX + rect.x(), pY + rect.y(), pW, pH), _pic);
}

/**************************************************************************************************
 * 垂直文本
 *************************************************************************************************/
void ILabel::_drawVerticalText(QPainter &painter, const QRect &rect)
{
    if (_textList.isEmpty())
    {
        return;
    }

    QFont font;
    int contentHeight = 0;
    int count = _textList.count();
    int fontSize = 0;
    bool bold = false;
    QString text;
    for (int i = 0; i < count; ++i)
    {
        text = _textList.at(i);
        fontSize = fontManager.getFontSize(4);
        bold = false;
        if (i < _fontSize.size())
        {
            fontSize = _fontSize[i];
        }

        if (i < _fontBold.size())
        {
            bold = _fontBold[i];
        }

        font = fontManager.textFont(fontSize, bold);
        contentHeight += fontManager.textHeightInPixels(font);
        contentHeight += _spacing;
    }

    // 间隔比项数少一个
    contentHeight -= _spacing;
    int startY = 0;
    if (contentHeight < rect.height())
    {
        if (_alignFlag & Qt::AlignVCenter)
        {
            startY = (rect.height() - contentHeight) / 2;
        }
        else if (_alignFlag & Qt::AlignBottom)
        {
            startY = rect.width() - contentHeight;
        }
    }
    startY += rect.y();

    int contentH = 0;
    int strlen = 0, fontH = 0, tx = 0;
    for (int i = 0; i < count; ++i)
    {
        text = _textList.at(i);
        fontSize = fontManager.getFontSize(4);
        bold = false;
        if (i < _fontSize.size())
        {
            fontSize = _fontSize[i];
        }

        if (i < _fontBold.size())
        {
            bold = _fontBold[i];
        }
        font = fontManager.textFont(fontSize, bold);
        strlen = fontManager.textWidthInPixels(text, font);
        fontH = fontManager.textHeightInPixels(font);
        if (strlen > rect.width())
        {
            fontSize = _adjustFontSizeToWidth(text, rect.width(), fontSize, bold);
            font = fontManager.textFont(fontSize, bold);
            if (contentHeight < rect.height())
            {
                fontH = fontManager.textHeightInPixels(font);
            }
            strlen = fontManager.textWidthInPixels(text, font);
        }

        tx = 0;
        if (_alignFlag & Qt::AlignHCenter)
        {
            if (strlen < rect.width())
            {
                tx = (rect.width() - strlen) / 2;
            }
        }
        else if (_alignFlag & Qt::AlignRight)
        {
            if (strlen < rect.width())
            {
                tx = rect.width() - strlen;
            }
        }

        painter.setFont(font);
        painter.drawText(QRect(tx + rect.x(), startY, strlen, fontH), text);
        contentH += fontH + _spacing;
        startY += fontH + _spacing;
        if (contentH > rect.height())
        {
            break;
        }
    }
}
