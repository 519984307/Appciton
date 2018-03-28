#include "ServiceCPUTemp.h"
#include <QVBoxLayout>
#include "SystemBoardProvider.h"
#include "SystemManager.h"
#include <QTextTable>
#include <QScrollBar>
#include "Debug.h"
#include "ParamManager.h"
#include "MenuManager.h"
#include "FontManager.h"

ServiceCPUTemp *ServiceCPUTemp::_selfObj = NULL;

/**************************************************************************************************
 * 功能：初始化。
 *************************************************************************************************/
void ServiceCPUTemp::init()
{
   _coreVersion();
   _Timer->start(1 * 1000);
}

/**************************************************************************************************
 * 功能:绘画
 *************************************************************************************************/
void ServiceCPUTemp::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setBrush(palette().background());
    painter.drawRoundedRect(rect(), 5, 5);

    QWidget::paintEvent(e);
}

/**************************************************************************************************
 * 返回。
 *************************************************************************************************/
void ServiceCPUTemp::_returnBtnReleased()
{
    _Timer->stop();
    menuManager.returnPrevious();

}

void ServiceCPUTemp::_TimeOut()
{
    _coreVersion();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ServiceCPUTemp::ServiceCPUTemp() : QWidget()
{
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = 500;

    QVBoxLayout *labelLayout = new QVBoxLayout();
    labelLayout->setContentsMargins(50, 0, 50, 20);
    labelLayout->setSpacing(2);
    labelLayout->setAlignment(Qt::AlignTop);

    // 标题栏。
    _title = new QLabel(trs("ServiceCPUTemp"));
    _title->setAlignment(Qt::AlignCenter);
    _title->setFixedHeight(TITLE_H + 10);
    _title->setFont(fontManager.textFont(fontManager.getFontSize(4)));
    QPalette p;
    p.setColor(QPalette::Foreground, Qt::black);
    _title->setPalette(p);
    labelLayout->addWidget(_title);

    _CORE = new LabeledLabel(trs("TEMP") + ":  ", "");
    _CORE->setFont(fontManager.textFont(fontSize));
    _CORE->setFixedSize(btnWidth, ITEM_H);
    labelLayout->addWidget(_CORE, 0, Qt::AlignCenter);

    _btnReturn = new LButtonEn();
    _btnReturn->setText(trs("VersionReturn"));
    _btnReturn->setFont(fontManager.textFont(fontSize));
    _btnReturn->setFixedSize(150, ITEM_H);
    connect(_btnReturn, SIGNAL(realReleased()), this, SLOT(_returnBtnReleased()));
    labelLayout->addStretch();
    labelLayout->addWidget(_btnReturn, 0, Qt::AlignRight);
//    labelLayout->addStretch();

    setLayout(labelLayout);

    p.setColor(QPalette::Window, QColor(209, 203, 183));
    setPalette(p);

//    setFixedSize(serviceWindowManager.getSubmenuWidth(), serviceWindowManager.getSubmenuHeight());

    _Timer = new QTimer(this);
    connect(_Timer, SIGNAL(timeout()), this, SLOT(_TimeOut()));
}

void ServiceCPUTemp::_coreVersion(void)
{
    QFile file("/sys/class/thermal/thermal_zone1/temp");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QString fileStr = file.readAll();
    file.close();

    int temp = fileStr.toInt();

    QString Str;
    Str.sprintf("%.3f", temp / 1000.000);
    Str += "\r\r";
    Str += Unit::localeSymbol(UNIT_TC);

    _CORE->setText(Str);

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ServiceCPUTemp::~ServiceCPUTemp()
{

}
