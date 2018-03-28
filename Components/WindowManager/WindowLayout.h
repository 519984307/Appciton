#pragma once
#include "PopupWidget.h"
#include "PatientDefine.h"
#include "IComboList.h"
#include "WindowWidget.h"

class LabelButton;
class ISpinBox;
class IButton;
class WindowLayout : public PopupWidget
{
    Q_OBJECT

public:
    static WindowLayout &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new WindowLayout();
        }
        return *_selfObj;
    }
    static WindowLayout *_selfObj;
    ~WindowLayout();

    void layoutExec(void);

protected:
    void showEvent(QShowEvent *e);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void _resetReleased(void);
    void _saveReleased(void);

private:
    WindowLayout();

private:
    static const int _itemH = 30;     // 子项高度
    WindowWidget *_windowWidget;           //
    IButton *_resetLayout;            // 恢复默认界面
    IButton *_saveLayout;             // 保存界面
};
#define windowLayout (WindowLayout::construction())
#define deletewindowLayout() (delete WindowLayout::_selfObj)
