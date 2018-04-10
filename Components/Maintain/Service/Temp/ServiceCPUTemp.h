#pragma once
#include "MenuWidget.h"
#include <QPushButton>
#include "LabelButton.h"
#include <QTextEdit>
#include "LabeledLabel.h"
#include "QTimer"

class ServiceCPUTemp : public MenuWidget
{
    Q_OBJECT

public:
    static ServiceCPUTemp &Construation()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ServiceCPUTemp();
        }

        return *_selfObj;
    }
    static ServiceCPUTemp *_selfObj;
    ~ServiceCPUTemp();

    //初始化
    void init();

private slots:
    void _returnBtnReleased(void);
    void _TimeOut();

private:
    ServiceCPUTemp();

    QTimer *_Timer;

    static const int minLen = 65;

    LabeledLabel *_CORE;

    LButtonEn *_btnReturn;               //返回按钮

    void _coreVersion(void);
};
#define serviceCPUTemp (ServiceCPUTemp::Construation())
