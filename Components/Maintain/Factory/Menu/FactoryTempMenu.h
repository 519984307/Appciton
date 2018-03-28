#pragma once
#include "PopupWidget.h"
#include <QStackedWidget>
#include <QGroupBox>

class IComboList;
class QLabel;
class LabelButton;
class FactoryTempMenu : public PopupWidget
{
    Q_OBJECT

public:
    static FactoryTempMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new FactoryTempMenu();
        }

        return *_selfObj;
    }
    static FactoryTempMenu *_selfObj;

    ~FactoryTempMenu();

    void readyShow();

    void getResult(int channel, int value,bool flag);

    void setTEMPValue(short t1, short t2);

    void showError(QString str);

protected:
    virtual void layoutExec(void);
    void keyPressEvent(QKeyEvent *e);
    void hideEvent(QHideEvent *e);

private slots:
    void _channelReleased(int);
    void _btnReleased(int);
    void _timeOut();

private:
    FactoryTempMenu();

private:
    static QString _btnStr[10];
    static QString _labelStr[10];

    QLabel *_tempChannel;
    QLabel *_tempValue;

    QLabel *_tempError;
    QStackedWidget *_stackedwidget;
    IComboList *_channel;

    LabelButton *lbtn[10];
    QLabel *_calibrateResult[10];
    LabelButton *_return;

    QImage _success;
    QImage _fault;

    QTimer *_timer;

private:
    bool _isFlushCalibrateStart;

    int _calibrateChannel;
    int _calibrateValue;
};
#define factoryTempMenu (FactoryTempMenu::construction())

