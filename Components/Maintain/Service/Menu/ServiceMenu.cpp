#include "ServiceMenu.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QBitmap>
#include <QScrollArea>
#include <QScrollBar>
#include "ColorManager.h"
#include "SubMenu.h"
#include "Debug.h"
#include "LabelButton.h"
#include "MenuManager.h"
#include "NIBPRepair.h"
#include "NIBPCalibrate.h"
#include "NIBPManometer.h"
#include "NIBPZeroPoint.h"
#include "NIBPPressureControl.h"
#include "ServiceUpgrade.h"
#include "ServiceVersion.h"
#include "ServiceCPUTemp.h"
#include "ServiceErrorLogMenu.h"

ServiceMenu *ServiceMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ServiceMenu::ServiceMenu() : QWidget()
{
    int submenuW = menuManager.getSubmenuWidth();
//    int submenuH = menuManager.getSubmenuHeight();
//    setFixedSize(submenuW, submenuH);

    int fontSize = fontManager.getFontSize(3);
    //定义按钮，并以序号命名，方便后面使用
    _nibpButton = new IButton("_nibpButton");
    _nibpButton->setFont(fontManager.textFont(fontSize));
    _nibpButton->setFixedSize(submenuW / 2, 30);
    connect(_nibpButton, SIGNAL(realReleased()), this,
            SLOT(_nibpButtonSlot()));

    _upgradeButton = new IButton("_upgradeButton");
    _upgradeButton->setFont(fontManager.textFont(fontSize));
    _upgradeButton->setFixedSize(submenuW / 2, 30);
    connect(_upgradeButton, SIGNAL(realReleased()), this,
            SLOT(_upgradeButtonSlot()));

    _versionButton = new IButton("_versionButton");
    _versionButton->setFont(fontManager.textFont(fontSize));
    _versionButton->setFixedSize(submenuW / 2, 30);
    connect(_versionButton, SIGNAL(realReleased()), this,
            SLOT(_versionButtonSlot()));

    _tempButton = new IButton("_tempButton");
    _tempButton->setFont(fontManager.textFont(fontSize));
    _tempButton->setFixedSize(submenuW / 2, 30);
    connect(_tempButton, SIGNAL(realReleased()), this,
            SLOT(_tempButtonSlot()));

    _errorLogButton = new IButton("_errorLogButton");
    _errorLogButton->setFont(fontManager.textFont(fontSize));
    _errorLogButton->setFixedSize(submenuW / 2, 30);
    connect(_errorLogButton, SIGNAL(realReleased()), this,
            SLOT(_errorLogButtonSlot()));

    _returnButton = new IButton("_returnButton");
    _returnButton->setFont(fontManager.textFont(fontSize));
    _returnButton->setFixedSize(submenuW / 2, 30);
    connect(_returnButton, SIGNAL(realReleased()), this,
            SLOT(_returnButtonSlot()));

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout = new QVBoxLayout();
    vLayout->setMargin(10);
    vLayout->setSpacing(20);

    vLayout->addStretch();
    vLayout->addWidget(_nibpButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_upgradeButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_versionButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_tempButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_errorLogButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_returnButton, 0, Qt::AlignCenter);
    vLayout->addStretch();

    setLayout(vLayout);

    nibprepair.Construation();
    nibpcalibrate.construction();
    nibprepair.addSubMenu(&nibpcalibrate);
    nibpmanometer.construction();
    nibprepair.addSubMenu(&nibpmanometer);
    nibpzeropoint.construction();
    nibprepair.addSubMenu(&nibpzeropoint);
    nibppressurecontrol.construction();
    nibprepair.addSubMenu(&nibppressurecontrol);
    nibprepair.addReturnMenu();

    serviceUpgrade.Construation();
    serviceVersion.Construation();
    serviceCPUTemp.Construation();
    serviceErrorLogMenu.construction();
}

/**************************************************************************************************
 * 功能：初始化。
 *************************************************************************************************/
void ServiceMenu::init()
{
    // 增加一个列表项。
    _nibpButton->setFocus();
}

/**************************************************************************************************
 * 功能:绘画
 *************************************************************************************************/
void ServiceMenu::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setBrush(palette().background());
    painter.drawRoundedRect(rect(), 5, 5);

    QWidget::paintEvent(e);
}

void ServiceMenu::_nibpButtonSlot()
{
    menuManager.openMenuGroup(&nibprepair);
}

void ServiceMenu::_upgradeButtonSlot()
{
    menuManager.openWidget(&serviceUpgrade);
}

void ServiceMenu::_versionButtonSlot()
{
    menuManager.openWidget(&serviceVersion);
}

void ServiceMenu::_tempButtonSlot()
{
    menuManager.openWidget(&serviceCPUTemp);
}

void ServiceMenu::_errorLogButtonSlot()
{
    menuManager.openWidget(&serviceErrorLogMenu);
}

void ServiceMenu::_returnButtonSlot()
{
    menuManager.returnPrevious();
}

/**************************************************************************************************
 * 重新聚焦菜单列表。
 *************************************************************************************************/
void ServiceMenu::_returnMenuList(SubMenu */*m*/)
{
}

/**************************************************************************************************
 * 需要更新标题。
 *************************************************************************************************/
void ServiceMenu::_titleChanged(void)
{
}

/**************************************************************************************************
 * 子菜单页聚焦
 *************************************************************************************************/
void ServiceMenu::_itemClicked()
{
    SubMenu *m = (SubMenu*)_scorllArea->widget();
    m->focusFirstItem();
}

/**************************************************************************************************
 * 改变滚动区域滚动条的值
 *************************************************************************************************/
void ServiceMenu::_changeScrollValue(int value)
{
    QScrollBar *bar = _scorllArea->verticalScrollBar();
    if (NULL != bar)
    {
        if (1== value)
        {
            bar->setValue(bar->maximum());
        }
        else
        {
            bar->setValue(0);
        }
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ServiceMenu::~ServiceMenu()
{

}
