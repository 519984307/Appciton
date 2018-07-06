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

#define COLOR_KEY(type, elem, state) (((type) << 16) | (elem) << 8 | (state))

class ThemeManagerPrivate
{
public:
    ThemeManagerPrivate() {}
    void loadColorScheme();
    QMap<int, QColor> colorScheme;
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
                                 ThemeManager::StateHightlight), QColor("#13CE66"));

    // default background color scheme
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                 ThemeManager::StateInactive), QColor("#EFF0F1"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                 ThemeManager::StateActive), QColor("#EFF0F1"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                 ThemeManager::StateDisabled), QColor("#E1E3E5"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementBackgound,
                                 ThemeManager::StateHightlight), QColor("#78A4FA"));

    // default text color scheme
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementText,
                                 ThemeManager::StateInactive), QColor("#2C405A"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementText,
                                 ThemeManager::StateActive), QColor("#2C405A"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementText,
                                 ThemeManager::StateDisabled), QColor("#E6E8EB"));
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlTypeNR, ThemeManager::ElementText,
                                 ThemeManager::StateHightlight), QColor("#2C405A"));

    // Popup List item
    colorScheme.insert(COLOR_KEY(ThemeManager::ControlPopupListItem, ThemeManager::ElementBackgound,
                                 ThemeManager::StateActive), QColor("#BCD2FD"));
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

    pal.setColor(QPalette::Inactive, QPalette::WindowText,
                 getColor(type, ElementText, StateInactive));
    pal.setColor(QPalette::Active, QPalette::WindowText,
                 getColor(type, ElementText, StateActive));
    pal.setColor(QPalette::Disabled, QPalette::WindowText,
                 getColor(type, ElementText, StateDisabled));

    pal.setColor(QPalette::Inactive, QPalette::Window,
                 getColor(type, ElementBackgound, StateInactive));
    pal.setColor(QPalette::Active, QPalette::Window,
                 getColor(type, ElementBackgound, StateActive));
    pal.setColor(QPalette::Disabled, QPalette::Window,
                 getColor(type, ElementBackgound, StateDisabled));
}

ThemeManager::ThemeManager()
    : QObject(), d_ptr(new ThemeManagerPrivate)
{
    d_ptr->loadColorScheme();
}

