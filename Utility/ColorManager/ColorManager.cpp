#include "ColorManager.h"
#include "IConfig.h"

ColorManager *ColorManager::_selfObj = NULL;

/**************************************************************************************************
 * 得到参数的QColor对象。
 *************************************************************************************************/
static QColor _loadColor(const QString &param)
{
    QString colorName;
    currentConfig.getStrValue("Display|" + param + "Color", colorName);

    int r = 0;
    int g = 0;
    int b = 0;
    currentConfig.getNumValue("Display|" + colorName + "|r", r);
    currentConfig.getNumValue("Display|" + colorName + "|g", g);
    currentConfig.getNumValue("Display|" + colorName + "|b", b);

    return QColor(r, g, b);
}

/**************************************************************************************************
 * 功能：根据颜色创建调色板。
 * 参数：
 *      color: 颜色
 * 返回：
 *      调色板对象。
 *************************************************************************************************/
static QPalette *_newPalette(const QColor &color)
{
    QPalette *palette = new QPalette();
    palette->setColor(QPalette::WindowText, color);
    palette->setColor(QPalette::Window, Qt::black);
    return palette;
}

/**************************************************************************************************
 * 功能：获取调色板。
 * 参数：
 *      param: 参数。
 * 返回：
 *      调色板对象。
 *************************************************************************************************/
QPalette &ColorManager::getPalette(const QString &param)
{
    ColorMap::iterator it = _colorMap.find(param);
    if (_colorMap.end() == it)
    {
        QPalette *palette = _newPalette(_loadColor(param));
        it = _colorMap.insert(param, palette);
//        debug("palette = %p, it.value = %p\n", palette, it.value());
    }

    return *it.value();
}

QColor ColorManager::getColor(const QString &param)
{
    return _loadColor(param);
}

/**************************************************************************************************
 * 功能：获取高亮色。
 * 返回：
 *      颜色对象。
 *************************************************************************************************/
QColor ColorManager::getHighlight(void)
{
    return QColor(227, 89, 42);//(98, 177, 213);
}

/**************************************************************************************************
 * 功能：获取隐藏色。
 * 返回：
 *      颜色对象。
 *************************************************************************************************/
QColor ColorManager::getShadow(void)
{
    return QColor(220, 220, 220);
}

/**************************************************************************************************
 * 功能：获取菜单标题栏颜色。
 * 返回：
 *      颜色对象。
 *************************************************************************************************/
QColor ColorManager::getBarBkColor(void)
{
    return QColor(98, 177, 213);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ColorManager::ColorManager()
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ColorManager::~ColorManager()
{
    ColorMap::iterator it = _colorMap.begin();
    for (; it != _colorMap.end(); ++it)
    {
        delete it.value();
    }
}
