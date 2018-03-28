#pragma once
#include "WidgetSelectMenu.h"

class IButton;
class IComboList;
class ShortTrendWidget;
class ShortTrendWidgetSelectMenu : public WidgetSelectMenu
{
    Q_OBJECT

public:
    static ShortTrendWidgetSelectMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ShortTrendWidgetSelectMenu();
        }
        return *_selfObj;
    }
    static ShortTrendWidgetSelectMenu *_selfObj;
    ~ShortTrendWidgetSelectMenu();

private slots:
    void _replaceListSlot(int index);
    void _insertListSlot(int index);
    void _removeButtonSlot(void);

private:
    ShortTrendWidgetSelectMenu();
    void _loadWidget(void);
};
#define shortTrendWidgetSelectMenu (ShortTrendWidgetSelectMenu::construction())
#define deleteShortTrendWidgetSelectMenu() (delete ShortTrendWidgetSelectMenu::_selfObj)
