/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/4
 **/

#pragma once

#include <QObject>
#include <QPalette>

class QIcon;
class ThemeManagerPrivate;
class ThemeManager : public QObject
{
    Q_OBJECT
public:
    enum ControlState
    {
        StateInactive,
        StateActive,
        StateDisabled,
        StateHighlight,
        StateNR,
    };

    enum ControlType
    {
        ControlWindow,
        ControlButton,
        ControlSpinBox,
        ControlComboBox,
        ControlPopupList,
        ControlPopupListItem,
        ControlMenuSideBarItem,
        ControlTypeNR,
    };

    enum ControlElement
    {
        ElementBorder,
        ElementBackgound,
        ElementText,
        ElementNR,
    };

    enum IconType {
        IconClose,
        IconUp,
        IconDown,
        IconLeft,
        IconRight,
        IconChecked,
        IconNR
    };

    enum ShadowElementType
    {
        ShadowElementLeft,
        ShadowElementLeftTop,
        ShadowElementTop,
        ShadowElementRightTop,
        ShadowElementRight,
        ShadowElementRightBottom,
        ShadowElementBottom,
        ShadowElementLeftBottom,
        ShadowElementNR,
    };

    static ThemeManager &getInstance();

    ~ThemeManager();

    /**
     * @brief getColor get the contorl color base on the control type, elem and state
     * @param type control type
     * @param elem elem of the control
     * @param state the control state
     * @return color scheme
     */
    const QColor &getColor(ControlType type, ControlElement elem, ControlState state) const;

    /**
     * @brief setupPalette setup the palette base on the control type
     * @param type control type
     * @param pal reference of the palette
     */
    void setupPalette(ControlType type, QPalette &pal);

    /**
     * @brief getBorderWidth get the border width of the contorl
     * @return border width
     */
    int getBorderWidth() const
    {
        return 2;
    }

    /**
     * @brief getBorderRadius get the border radius of the contorl
     * @return border radius
     */
    int getBorderRadius() const
    {
        return 4;
    }

    /**
     * @brief getIcon get a icon
     * @param icon icon type
     * @return reference to the icon
     */
    const QIcon &getIcon(IconType icon);

    /**
     * @brief getAcceptableControlHeight get the acceptable control height,
     *        all controls should use this value as it's default height
     * @return height;
     */
    int getAcceptableControlHeight() const
    {
        return 48;
    }

    /**
     * @brief defaultFontPixSize get the default font pixsize
     * @return the font pix size
     */
    int defaultFontPixSize() const
    {
        return 20;
    }

    /**
     * @brief getShadownPixmapElement get the shadow element
     * @type the type of the shadow element
     * @size size of the shadow element
     * @return the pixmap
     */
    QPixmap getShadowElement(ShadowElementType type, const QSize &size);

private:
    ThemeManager();
    ThemeManagerPrivate *const d_ptr;
};

#define themeManger (ThemeManager::getInstance())
