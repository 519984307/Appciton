#pragma once
#include "PopupWidget.h"


class QLabel;
class IComboList;
class LButtonEn;
class LabelButton;
class QTimer;
class FactoryDataRecord : public PopupWidget
{
    Q_OBJECT

public:
    static FactoryDataRecord &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new FactoryDataRecord();
        }

        return *_selfObj;
    }
    static FactoryDataRecord *_selfObj;

    ~FactoryDataRecord();

    void readyShow();

protected:
    virtual void layoutExec(void);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void _ECGReleased(int);
    void _TEMPReleased(int);
    void _SPO2Released(int);
    void _NIBPReleased(int);
    void _batteryReleased(int);
    void _pdCommLog(int);

private:
    FactoryDataRecord();

    IComboList *_ECG;
    IComboList *_SPO2;
    IComboList *_NIBP;
    IComboList *_TEMP;
    IComboList *_battery;
    IComboList *_PDCommLog;
    LabelButton *_return;

    QStringList _spo2ParamList;
};
#define factoryDataRecord (FactoryDataRecord::construction())



