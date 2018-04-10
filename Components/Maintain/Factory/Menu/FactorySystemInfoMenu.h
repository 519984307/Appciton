#pragma once
#include "MenuWidget.h"

class LabelButton;
class LButtonEn;
class QLabel;
class FactorySystemInfoMenu : public MenuWidget
{
    Q_OBJECT

public:
    static FactorySystemInfoMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new FactorySystemInfoMenu();
        }

        return *_selfObj;
    }
    static FactorySystemInfoMenu *_selfObj;

    ~FactorySystemInfoMenu();
    void readyShow();

private slots:
    void _serialNumReleased();

private:
    FactorySystemInfoMenu();

    LabelButton *_serialNum;       //序列号
    LabelButton *_return;
};
#define factorySystemInfoMenu (FactorySystemInfoMenu::construction())
