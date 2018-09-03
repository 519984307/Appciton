/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/4
 **/

#include "ThemeManager.h"
#include <QMap>
#include <QIcon>

#define COLOR_KEY(type, elem, state) (((type) << 16) | (elem) << 8 | (state))

class ThemeManagerPrivate
{
public:
    ThemeManagerPrivate() {}
    void loadColorScheme();
    QMap<int, QColor> colorScheme;
    QMap<ThemeManager::IconType, QIcon> iconMap;
};

static const char *iconPaths[ThemeManager::IconNR] =
{
    ":/ui/close.svg",
    ":/ui/up.svg",
    ":/ui/down.svg",
    ":/ui/left.svg",
    ":/ui/right.svg",
    ":/ui/checked.svg",
};

void ThemeManagerPrivate::loadColorScheme()
{
    // initialize all the default color with black color
    for (int i = ThemeManager::ElementBorder; i < ThemeManager::ElementNR; i++)
    {
        for (int j = ThemeManager::StateInactive; j < ThemeManager::StateNR; j++)
        {
            colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, i, j), Qt::black);
        }
    }

    // default border color scheme
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementBorder,
                                 ThemeManager::StateInactive), QColor("#C0C6CE"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementBorder,
                                 ThemeManager::StateActive), QColor("#13CE66"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementBorder,
                                 ThemeManager::StateDisabled), QColor("#E6E8EB"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementBorder,
                                 ThemeManager::StateHighlight), QColor("#13CE66"));

    // default background color scheme
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                 ThemeManager::StateInactive), QColor("#EFF0F1"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                 ThemeManager::StateActive), QColor("#EFF0F1"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                 ThemeManager::StateDisabled), QColor("#E1E3E5"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                 ThemeManager::StateHighlight), QColor("#78A4FA"));

    // default text color scheme
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementText,
                                 ThemeManager::StateInactive), QColor("#2C405A"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementText,
                                 ThemeManager::StateActive), QColor("#2C405A"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementText,
                                 ThemeManager::StateDisabled), QColor("#E6E8EB"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementText,
                                 ThemeManager::StateHighlight), QColor("#2C405A"));

    // Popup List item
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlPopupListItem, ThemeManager::ElementBackgound,
                                 ThemeManager::StateActive), QColor("#BCD2FD"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlPopupListItem, ThemeManager::ElementBackgound,
                                 ThemeManager::StateInactive), QColor("#EEEE00"));

    // SpinBox
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlSpinBox, ThemeManager::ElementBackgound,
                                 ThemeManager::StateHighlight), QColor("#C0C6CE"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlSpinBox , ThemeManager::ElementText,
                                 ThemeManager::StateDisabled), QColor(186, 186, 186));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlSpinBox , ThemeManager::ElementBorder,
                                 ThemeManager::StateDisabled), QColor(225, 225, 225));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlSpinBox , ThemeManager::ElementBackgound,
                                 ThemeManager::StateDisabled), QColor("#EFF0F1"));

    // button
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlButton , ThemeManager::ElementText,
                                 ThemeManager::StateDisabled), QColor(186, 186, 186));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlButton , ThemeManager::ElementBorder,
                                 ThemeManager::StateDisabled), QColor(225, 225, 225));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlButton , ThemeManager::ElementBackgound,
                                 ThemeManager::StateDisabled), QColor("#EFF0F1"));

    // comboBox
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlComboBox, ThemeManager::ElementText,
                                 ThemeManager::StateDisabled), QColor(186, 186, 186));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlComboBox, ThemeManager::ElementBorder,
                                 ThemeManager::StateDisabled), QColor(225, 225, 225));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlComboBox, ThemeManager::ElementBackgound,
                                 ThemeManager::StateDisabled), QColor("#EFF0F1"));
}

ThemeManager &ThemeManager::getInstance()
{
    static ThemeManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new ThemeManager;
    }
    return *instance;
}

ThemeManager::~ThemeManager()
{
    if (d_ptr)
    {
        delete d_ptr;
    }
}

const QColor &ThemeManager::getColor(ThemeManager::ControlType type, ThemeManager::ControlElement elem,
                                     ThemeManager::ControlState state) const
{
    if (type > ControlTypeNR)
    {
        type = ControlTypeNR;
    }

    if (elem > ElementNR)
    {
        elem = ElementText;
    }

    if (state > StateNR)
    {
        state = StateInactive;
    }

    int key = COLOR_KEY(type, elem, state);

    QMap<int, QColor>::ConstIterator iter = d_ptr->colorScheme.find(key);

    if (iter != d_ptr->colorScheme.constEnd())
    {
        return iter.value();
    }

    // return default value
    key = COLOR_KEY(ControlTypeNR, elem, state);
    iter = d_ptr->colorScheme.find(key);
    Q_ASSERT(iter != d_ptr->colorScheme.constEnd());
    return iter.value();
}

void ThemeManager::setupPalette(ThemeManager::ControlType type, QPalette &pal)
{
    pal.setColor(QPalette::Inactive, QPalette::Shadow,
                 getColor(type, ElementBorder, StateInactive));
    pal.setColor(QPalette::Active, QPalette::Shadow,
                 getColor(type, ElementBorder, StateActive));
    pal.setColor(QPalette::Disabled, QPalette::Shadow,
                 getColor(type, ElementBorder, StateDisabled));
    pal.setColor(QPalette::Highlight,
                 getColor(type, ElementBorder, StateHighlight));

    pal.setColor(QPalette::Inactive, QPalette::WindowText,
                 getColor(type, ElementText, StateInactive));
    pal.setColor(QPalette::Active, QPalette::WindowText,
                 getColor(type, ElementText, StateActive));
    pal.setColor(QPalette::Disabled, QPalette::WindowText,
                 getColor(type, ElementText, StateDisabled));
    pal.setColor(QPalette::HighlightedText,
                 getColor(type, ElementText, StateHighlight));

    pal.setColor(QPalette::Inactive, QPalette::Window,
                 getColor(type, ElementBackgound, StateInactive));
    pal.setColor(QPalette::Active, QPalette::Window,
                 getColor(type, ElementBackgound, StateActive));
    pal.setColor(QPalette::Disabled, QPalette::Window,
                 getColor(type, ElementBackgound, StateDisabled));
    pal.setColor(QPalette::Highlight,
                 getColor(type, ElementBackgound, StateHighlight));
}

const QIcon &ThemeManager::getIcon(ThemeManager::IconType icon)
{
    QMap<IconType, QIcon>::ConstIterator iter;
    iter = d_ptr->iconMap.find(icon);
    if (iter != d_ptr->iconMap.constEnd())
    {
        return iter.value();
    }

    // load the icon from resourse
    QIcon ico;
    if (icon >= 0 && icon < IconNR)
    {
        ico = QIcon(iconPaths[icon]);
    }
    return *d_ptr->iconMap.insert(icon, ico);
}

ThemeManager::ThemeManager()
    : QObject(), d_ptr(new ThemeManagerPrivate)
{
    d_ptr->loadColorScheme();
}

