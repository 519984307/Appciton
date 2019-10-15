/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/11/29
 **/

#include "FontManager.h"
#include "Debug.h"
#include <QFontMetrics>
#include <QFontDatabase>
#include <QStringList>
#include "IConfig.h"

/**************************************************************************************************
 * 构造函数。
 *************************************************************************************************/
FontManager::FontManager()
{
    QFontDatabase::addApplicationFont("/usr/local/nPM/fonts/DroidSans.ttf");
    QFontDatabase::addApplicationFont("/usr/local/nPM/fonts/DroidSansFallbackFull.ttf");
    // QFontDatabase::addApplicationFont("/usr/local/nPM/fonts/HelveticaNeue-Bold.otf");
    QFontDatabase::addApplicationFont("/usr/local/nPM/fonts/HelveticaNeueLTPro-Bd.otf");
    QFontDatabase::addApplicationFont("/usr/local/nPM/fonts/FandolHei-Regular-BLM.otf");
    _textFont.setFamily("Droid Sans Fallback");
    // _numFont.setFamily("Helvetica Neue");
    _numFont.setFamily("Helvetica Neue LT Pro");
    // _recordFont.setFamily("Droid Sans Fallback");
    _recordFont.setFamily("FandolHei");

#if 0
    debug("---------------\n");
    QFontDatabase fdb;
    QStringList flist = fdb.families();
    for (int i = 0; i < flist.size(); i++)
    {
        printf("%s:\t", qPrintable(flist[i]));
        QStringList slist = fdb.styles(flist[i]);
        for (int i = 0; i < slist.size(); i++)
        {
            printf("%s,", qPrintable(slist[i]));
        }
        printf("\n");
    }
    debug("---------------\n\n");
#endif

    _textFont.setBold(false);
    _textFont.setPixelSize(_defaultFontSize);

    _numFont.setBold(false);
    _numFont.setPixelSize(_defaultFontSize);

    _recordFont.setBold(false);
    _recordFont.setPixelSize(_defaultFontSize);

    _prop = 1.0;
    _fontSizeNum0 = 12;
    _fontSizeNum1 = 15;
    _fontSizeNum2 = 16;
    _fontSizeNum3 = 18;
    _fontSizeNum4 = 20;
    _fontSizeNum5 = 25;
    _fontSizeNum6 = 30;
    _fontTrendText = 16;

    QStringList FontSize;
    QString prefix = "PrimaryCfg|UILayout|WidgetsOrder|FontSize";
    FontSize = systemConfig.getChildNodeNameList(prefix);

    for (int i = 0; i < FontSize.size(); i++)
    {
        QString string = prefix + "|" + FontSize[i];
        int index = 1;
        systemConfig.getNumValue(string, index);
        PARAM_FONT_NUM.append(index);
    }
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
FontManager &FontManager::getInstance()
{
    static FontManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new FontManager();
        FontMangerInterface *old = registerFontManager(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

FontManager::~FontManager()
{
    QFontDatabase::removeAllApplicationFonts();
}

/**************************************************************************************************
 * 构造文本字体
 *************************************************************************************************/
QFont FontManager::textFont(int fontSize, bool isBold)
{
    QFont font(_textFont);
    if (fontSize > 0)
    {
        font.setPixelSize(fontSize);
    }

    font.setBold(isBold);
    return font;
}

/**************************************************************************************************
 * 构造数值字体
 *************************************************************************************************/
QFont FontManager::numFont(int fontSize, bool isBold)
{
    QFont font(_numFont);
    if (fontSize > 0)
    {
        font.setPixelSize(fontSize);
    }

    font.setBold(isBold);
    return font;
}

/**************************************************************************************************
 * 构造记录字体
 *************************************************************************************************/
QFont FontManager::recordFont(int fontSize, bool isBold)
{
    QFont font(_recordFont);
    if (fontSize > 0)
    {
        font.setPixelSize(fontSize);
    }

    font.setBold(isBold);
    return font;
}

/**************************************************************************************************
 * 计算特定字体下文本行的高度，单位:像素。
 *************************************************************************************************/
int FontManager::textHeightInPixels(const QFont &font)
{
    QFontMetrics fm(font);
    return fm.height();
}

/**************************************************************************************************
 * 计算特定字体下文本字符串长度，单位:像素。
 *************************************************************************************************/
int FontManager::textWidthInPixels(const QString &text, const QFont &font)
{
    QFontMetrics fm(font);
    return fm.width(text);
}

/**************************************************************************************************
 * 功能： 根据布局大小确定Text字符串对应XML配置文件中的字号。
 * 参数：
 *      r: 布局参数。
 *************************************************************************************************/
int FontManager::adjustTextFontSizeXML(const QRect r)
{
    int fontSize_sure = 3;
    int fontSize_new = fontSize_sure;
    int Height = r.height();
    QFont font;

    font = textFont(PARAM_FONT_NUM[fontSize_new], true);
    int txtHeight = textHeightInPixels(font);

    if (Height >= txtHeight)
    {
        for (int i = fontSize_sure; i < (PARAM_FONT_NUM.count() - 1); i++)
        {
            font = textFont(PARAM_FONT_NUM[fontSize_new], true);
            txtHeight = textHeightInPixels(font);
            if (Height >= txtHeight)
            {
                fontSize_new++;
            }
            else
            {
                break;
            }
        }
        fontSize_sure = fontSize_new;
    }
    else
    {
        for (int i = fontSize_sure; i > 0; i--)
        {
            font = textFont(PARAM_FONT_NUM[fontSize_new], true);
            txtHeight = textHeightInPixels(font);
            if (Height <= txtHeight)
            {
                fontSize_new--;
            }
            else
            {
                break;
            }
        }
        fontSize_sure = fontSize_new;
    }

    return fontSize_sure;
}

/**************************************************************************************************
 * 功能： 根据布局大小确定Text字符串大小。
 * 参数：
 *      r: 布局参数。
 *************************************************************************************************/
int FontManager::adjustTextFontSize(const QRect r, bool bold, int fontSize)
{
    if (fontSize == -1)
    {
        fontSize = getFontSize(1);
    }
    int height = r.height();
    QFont font = textFont(fontSize, bold);
    int fontH = textHeightInPixels(font);
    if (fontH > height)
    {
        while (fontH > height && fontSize > 0)
        {
            --fontSize;
            font = textFont(fontSize, bold);
            fontH = textHeightInPixels(font);
        }
    }
    else
    {
        while (fontH < height && fontSize < 100)
        {
            ++fontSize;
            font = textFont(fontSize, bold);
            fontH = textHeightInPixels(font);
        }
    }

    return fontSize;
}

int FontManager::textFontSize(const QRect r, QString text, bool bold, int fontSize)
{
    if (fontSize == -1)
    {
        fontSize = adjustTextFontSize(r, bold);
    }
    QFont font = textFont(fontSize, bold);
    int fontW = textWidthInPixels(text, font);
    int fontH = textHeightInPixels(font);
    int width = r.width();
    int height = r.height();

    while (fontW > width)
    {
        fontSize--;
        fontW = textWidthInPixels(text, textFont(fontSize, bold));
    }
    fontH = textHeightInPixels(textFont(fontSize, bold));
    while (fontH > height)
    {
        fontSize--;
        fontH = textHeightInPixels(textFont(fontSize, bold));
    }
    return fontSize - 1;
}

/**************************************************************************************************
 * 功能： 设置单位区的数字大小。
 * 参数：
 *      r: 布局参数。
 *      num: 字符串
 *************************************************************************************************/
int FontManager::adjustNumFontSizeXML(const QRect r, QString num)
{
    int fontSize_sure = 3;
    int fontSize_new = fontSize_sure;
    int Height = r.height();
    int width = r.width();
    QFont font;

    font = numFont(PARAM_FONT_NUM[fontSize_new], true);
    int numWidth = textWidthInPixels(num, font);

    if (width >= numWidth)
    {
        for (int i = fontSize_sure; i < (PARAM_FONT_NUM.count() - 1); i++)
        {
            font = numFont(PARAM_FONT_NUM[fontSize_new], true);
            numWidth = textWidthInPixels(num, font);
            if (width >= numWidth)
            {
                fontSize_new++;
            }
            else
            {
                break;
            }
        }
        fontSize_sure = fontSize_new;
    }
    else
    {
        for (int i = fontSize_sure; i > 0; i--)
        {
            font = numFont(PARAM_FONT_NUM[fontSize_new], true);
            numWidth = textWidthInPixels(num, font);
            if (width <= numWidth)
            {
                fontSize_new--;
            }
            else
            {
                break;
            }
        }
        fontSize_sure = fontSize_new;
    }

    int fontSize_num = fontSize_sure;
    font = numFont(PARAM_FONT_NUM[fontSize_sure], true);
    int numHeight = textHeightInPixels(font);
    if (Height < numHeight)
    {
        for (int i = fontSize_num; i > 0; i--)
        {
            font = numFont(PARAM_FONT_NUM[fontSize_new], true);
            numHeight = textHeightInPixels(font);
            if (Height < numHeight)
            {
                fontSize_new--;
            }
            else
            {
                break;
            }
        }
        fontSize_sure = fontSize_new;
    }

    return fontSize_sure;
}

/**************************************************************************************************
 * 功能： 设置单位区的数字大小。
 * 参数：
 *      r: 布局参数。
 *      num: 字符串
 *************************************************************************************************/
int FontManager::adjustNumFontSize(const QRect r, bool bold, QString num, int fontSize)
{
    int height = r.height() - 1;
    int width = r.width() - 1;

    if (fontSize == -1)
    {
        if (height > 12)
        {
            fontSize = static_cast<int>(height / 1.2);
        }
        else
        {
            fontSize = 5;
        }
    }
    QFont font = textFont(fontSize, bold);
    int fontW = textWidthInPixels(num, font);
    if (fontW > width)
    {
        while (fontW > width && fontSize > 0)
        {
            --fontSize;
            font = textFont(fontSize, bold);
            fontW = textWidthInPixels(num, font);
        }
    }
    else
    {
        while (fontW < width && fontSize < 100)
        {
            ++fontSize;
            font = textFont(fontSize, bold);
            fontW = textWidthInPixels(num, font);
        }
    }

    if (height > 0)
    {
        font = textFont(fontSize, bold);
        int fontH = textHeightInPixels(font);

        while (fontH > height && fontSize > 0)
        {
            --fontSize;
            font = textFont(fontSize, bold);
            fontH = textHeightInPixels(font);
        }
    }

    return fontSize;
}

/**************************************************************************************************
 * 功能： 更具字体对应的数组返回字体大小
 * 参数：
 *      index: 字号。
 *************************************************************************************************/
int FontManager::getFontSizeXML(int index)
{
    if (index < 0)
    {
        index = 0;
    }
    return PARAM_FONT_NUM[index];
}

/**************************************************************************************************
 * 功能： 返回不同的字体大小
 * 参数：
 *      index: 字号。
 *************************************************************************************************/
int FontManager::getFontSize(int index)
{
    int size;

    switch (index)
    {
    case 0:
        size = _fontSizeNum0;
        break;
    case 1:
        size = _fontSizeNum1;
        break;
    case 2:
        size = _fontSizeNum2;
        break;
    case 3:
        size = _fontSizeNum3;
        break;
    case 4:
        size = _fontSizeNum4;
        break;
    case 5:
        size = _fontSizeNum5;
        break;
    case 6:
        size = _fontSizeNum6;
        break;
    case 7:
        size = _fontTrendText;
        break;
    default:
        size = 15;
        break;
    }
    return size;
}


