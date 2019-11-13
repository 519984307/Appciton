/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/11/29
 **/


#pragma once
#include "FontManagerInterface.h"

class FontManager : public FontManagerInterface
{
public:
    static FontManager &getInstance(void);

    ~FontManager();

    // 构造文本字体
    QFont textFont(int fontSize = -1, bool isBold = false);

    // 构造数值字体
    QFont numFont(int fontSize = -1, bool isBold = false);

    // 构造打印字体
    QFont recordFont(int fontSize = -1, bool isBold = false);

    // 计算特定字体下文本行的高度，单位:像素。
    int textHeightInPixels(const QFont &font = QFont());

    // 计算特定字体下文本字符串长度，单位:像素。
    int textWidthInPixels(const QString &text, const QFont &font = QFont());

    // 根据布局大小确定字体对应XML配置文件中的字号
    int adjustTextFontSizeXML(const QRect r);

    // 根据布局大小确定字体大小
    int adjustTextFontSize(const QRect r, bool bold = false, int fontSize = -1);

    /**
     * @brief textFontSize 根据空间和字符串计算字体大小
     * @param r 字体要显示的空间
     * @param text 要显示的字符串
     * @param bold 字体是否加粗
     * @param fontSize 默认字体大小（非超出显示区域时显示的字体大小），-1：默认字体大小为填满整个空间的最大字体大小
     * @return
     */
    int textFontSize(const QRect r, QString text, bool bold = false, int fontSize = -1);

    // 根据布局大小确定数字对应XML配置文件中的字号
    int adjustNumFontSizeXML(const QRect r, QString num = "999");

    // 根据布局大小确定数字对应的大小
    int adjustNumFontSize(const QRect r, bool bold = false, QString num = "999", int fontSize = -1);

    // 字体对应的字号返回字体大小
    int getFontSizeXML(int index);
    int getFontSize(int index);

    // 默认文本字体
    QFont defaultTextFont()
    {
        return _textFont;
    }

    // 默认数值字体
    QFont defaultNumFont()
    {
        return _numFont;
    }

    // 默认记录字体
    QFont defaultRecordFont()
    {
        return _recordFont;
    }

private:
    FontManager();
    QList<int> PARAM_FONT_NUM;

private:
    QFont _textFont;        // 文本字体(显示)
    QFont _numFont;         // 数值字体(显示)
    QFont _recordFont;      // 记录字体(打印)
    float _prop;            // 不同屏幕的字体显示比例（以800*600为基准）
    int _fontSizeNum0;
    int _fontSizeNum1;
    int _fontSizeNum2;
    int _fontSizeNum3;
    int _fontSizeNum4;
    int _fontSizeNum5;
    int _fontSizeNum6;
    int _fontTrendText;

    static const int _defaultFontSize = 10;
};

#define fontManager (FontManager::getInstance())
