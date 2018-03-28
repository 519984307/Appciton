#pragma once
#include <QWidget>
#include <QPushButton>
#include "LabelButton.h"
#include <QTextEdit>
#include "LabeledLabel.h"
#include "QTimer"

class ServiceCPUTemp : public QWidget
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

protected:
    void paintEvent(QPaintEvent *e);

private slots:
    void _returnBtnReleased(void);
    void _TimeOut();

private:
    ServiceCPUTemp();

    QLabel *_title;                      //标题

    QTimer *_Timer;

    static const int minLen = 65;

    LabeledLabel *_CORE;

    LButtonEn *_btnReturn;               //返回按钮

    void _coreVersion(void);
};
#define serviceCPUTemp (ServiceCPUTemp::Construation())
