#pragma once
#include "PopupWidget.h"

class LabelButton;
class LButtonEn;
class QLabel;
class FactorySystemInfoMenu : public PopupWidget
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

protected:
    virtual void layoutExec(void);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void _serialNumReleased();

private:
    FactorySystemInfoMenu();

    LabelButton *_serialNum;       //序列号
    LabelButton *_return;
};
#define factorySystemInfoMenu (FactorySystemInfoMenu::construction())
