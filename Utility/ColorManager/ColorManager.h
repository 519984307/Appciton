/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/8
 **/


#pragma once
#include <QString>
#include <QMap>
#include <QPalette>

class ColorManager
{
public:
    static ColorManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ColorManager();
        }
        return *_selfObj;
    }
    static ColorManager *_selfObj;

    // 获得参数的调色板。
    QPalette &getPalette(const QString &param);
    QColor getColor(const QString &param);

    // 获取高亮色。
    QColor getHighlight(void);
    QColor getShadow(void);

    // 获取菜单标题栏颜色。
    QColor getBarBkColor(void);

    ~ColorManager();

private:
    ColorManager();

    typedef QMap<QString, QPalette*> ColorMap;
    ColorMap _colorMap;
};
#define colorManager (ColorManager::construction())
#define deleteColorManager() (delete ColorManager::_selfObj)
