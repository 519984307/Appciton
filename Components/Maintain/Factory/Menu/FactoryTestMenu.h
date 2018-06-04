#pragma once
#include "MenuWidget.h"
#include "LabelButton.h"
#include "ISpinBox.h"
#include "SubMenu.h"
//#define OUTPUT_TESTPACKET_THROUGH_USB
#define TEST_REFRESH_RATE

enum FactoryTestType
{
    FACTORY_TEST_LIGHT,
    FACTORY_TEST_SOUND,
    FACTORY_TEST_KEY,
    FACTORY_TEST_PRINT,
#ifdef OUTPUT_TESTPACKET_THROUGH_USB
    FACTORY_TEST_USB_HS,
    FACTORY_TEST_USB_FS,
#endif
    FACTORY_TEST_WIFI,
    FACTORY_TEST_ENTHERNET,
    FACTORY_TEST_BATTERY,
    FACTORY_TEST_NR
};

class QLabel;
class LButtonEn;
class FactoryTestMenu : public SubMenu
{
    Q_OBJECT

public:
    static FactoryTestMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new FactoryTestMenu();
        }

        return *_selfObj;
    }
    static FactoryTestMenu *_selfObj;

    ~FactoryTestMenu();

    virtual void layoutExec();

    virtual void readyShow();

private slots:
    void _btnReleased(int);

#ifdef TEST_REFRESH_RATE
private slots:
    void _onFreshRateChanged(QString valStr);
private:
    ISpinBox *_freshRateSpinBox;
#endif

private:
    FactoryTestMenu();

private:
    static QString _btnStr[FACTORY_TEST_NR];
    LabelButton *lbtn[FACTORY_TEST_NR];
    LabelButton *_return;
};
#define factoryTestMenu (FactoryTestMenu::construction())



