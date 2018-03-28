#pragma once
#include "WidgetSelectMenu.h"

class IButton;
class IComboList;
class TrendWidgetSelectMenu : public WidgetSelectMenu
{
    Q_OBJECT

public:
    static TrendWidgetSelectMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new TrendWidgetSelectMenu();
        }
        return *_selfObj;
    }
    static TrendWidgetSelectMenu *_selfObj;
    ~TrendWidgetSelectMenu();

private slots:
    void _replaceListSlot(int index);
    void _insertListSlot(int index);
    void _removeButtonSlot(void);

private:
    TrendWidgetSelectMenu();
    void _loadWidget(void);
};
#define trendWidgetSelectMenu (TrendWidgetSelectMenu::construction())
#define deleteTrendWidgetSelectMenu() (delete TrendWidgetSelectMenu::_selfObj)
