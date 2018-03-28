#include "MenuManager.h"
#include "WindowManager.h"
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QPainter>
#include "ColorManager.h"
#include "SystemMenu.h"
#include "SupervisorEntrance.h"
#include "PublicMenuManager.h"
#include "SupervisorMenuManager.h"
#include "MenuGroup.h"
#include "NIBPRepair.h"

MenuManager *MenuManager::_selfObj = NULL;

/**************************************************************************************************
 * 设置文本。
 *************************************************************************************************/
void MenuManager::setCloseBtnTxt(const QString &txt)
{
    closeBtn->setText(txt);
}

/**************************************************************************************************
 * 设置pic。
 *************************************************************************************************/
void MenuManager::setCloseBtnPic(const QImage &pic)
{
    closeBtn->setPicture(pic);
}

/**************************************************************************************************
 * 设置background Color。
 *************************************************************************************************/
void MenuManager::setCloseBtnColor(QColor color)
{
    closeBtn->setBackgroundColor(color);
}

/**************************************************************************************************
 * 添加一个子菜单。
 *************************************************************************************************/
void MenuManager::addMenuGroup(MenuGroup *menuGroup)
{
    // 添加子菜单。
    _subMenus->addWidget(menuGroup);
    _subMenus->setCurrentWidget(menuGroup);
    menuGroup->listTableOrder(false);
}

void MenuManager::openMenuGroup(MenuGroup *menuGroup)
{
    _subMenus->addWidget(menuGroup);
    _subMenus->setCurrentWidget(menuGroup);
    menuGroup->_initMenuList();
}

void MenuManager::openWidget(QWidget *menuGroup)
{
    _subMenus->addWidget(menuGroup);
    _subMenus->setCurrentWidget(menuGroup);
}

void MenuManager::returnPrevious()
{
    _subMenus->removeWidget(_subMenus->currentWidget());
}

void MenuManager::closeBtnSetFoucs()
{
    closeBtn->setFocus();
}

/***************************************************************************************************
 * 功能：NIBP服务模式警告
 **************************************************************************************************/
void MenuManager::NIBPWarn(bool enable)
{
    if (_subMenus->currentWidget() == &nibprepair)
    {
        nibprepair.warnShow(enable);
    }
}

/**************************************************************************************************
 * 重绘制。
 *************************************************************************************************/
void MenuManager::paintEvent(QPaintEvent */*event*/)
{
    QPainterPath clipPath;
    clipPath.addRoundedRect(this->rect(), 5, 5);

    QPainter barPainter(this);
    barPainter.setRenderHint(QPainter::Antialiasing);
    barPainter.setClipPath(clipPath);

    // 绘制边框。
    QPen pen;
    pen.setColor(colorManager.getBarBkColor());
    pen.setWidth(BORDER_W * 2);
    barPainter.setPen(pen);
    barPainter.setBrush(palette().window());
    barPainter.drawRect(rect());

    QRect r = rect();
    r.setBottom(TITLE_H);
    barPainter.fillRect(r, colorManager.getBarBkColor());
}

/**************************************************************************************************
 * 隐藏事件。
 *************************************************************************************************/
void MenuManager::hideEvent(QHideEvent *event)
{
    QDialog::hideEvent(event);
}

/**************************************************************************************************
 * 按键事件。
 *************************************************************************************************/
void MenuManager::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
        if (_menuGroup != NULL)
        {
            if (_menuGroup->listTableFocus())
            {
                _menuGroup->listTableOrder(true);
                break;
            }
        }

        focusPreviousChild();
        break;
        case Qt::Key_Down:
        case Qt::Key_Right:
        if (_menuGroup != NULL)
        {
            if (_menuGroup->listTableFocus())
            {
                _menuGroup->listTableOrder(false);
                break;
            }
        }

        focusNextChild();
        break;
        default:
            break;
    }

    QDialog::keyPressEvent(event);
}

void MenuManager::_titleChanged()
{
    MenuGroup *m = (MenuGroup*)_subMenus->currentWidget();
    _titleLabel->setText(m->desc());
}

void MenuManager::_closeSlot(void)
{
    int i = _subMenus->count();
    while (i > 1)
    {
        for (int j = 0; j < i; j++)
        {
            if (_subMenus->widget(j) != &publicMenuManager)
            {
                _subMenus->removeWidget(_subMenus->widget(j));
            }
        }
        i = _subMenus->count();
    }
//    _subMenus->setCurrentWidget(&publicMenuManager);
    close();
}

void MenuManager::_currentMenuGroup(int /*index*/)
{
//    _menuGroup = (MenuGroup*)_subMenus->currentWidget();
    _menuGroup = dynamic_cast<MenuGroup *>(_subMenus->currentWidget());
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
MenuManager::MenuManager() : QDialog(0, Qt::FramelessWindowHint)
{
    // 设定为模态窗口。
    setModal(true);

    setFixedSize(windowManager.getPopMenuWidth(), windowManager.getPopMenuHeight());
    _listWidth = windowManager.getPopMenuWidth() / 4;
    _submenuWidth = windowManager.getPopMenuWidth() - _listWidth - 20;
    _submenuHeight = windowManager.getPopMenuHeight() - 30 - 30 - 20;

    // 创建资源。
    QPalette p;
    p.setColor(QPalette::Foreground, Qt::black);
    setAttribute(Qt::WA_NoSystemBackground); //draw background in paintEvent

    int fontSize = fontManager.getFontSize(1);
    // 标题栏。
    _titleLabel = new QLabel("");
    _titleLabel->setAlignment(Qt::AlignCenter);
    _titleLabel->setFixedHeight(TITLE_H);
    _titleLabel->setFont(fontManager.textFont(fontManager.getFontSize(3)));
    _titleLabel->setPalette(p);

    //关闭按钮
    closeBtn = new IButton("X");
    closeBtn->setEnableKeyAction(false);
    closeBtn->setFixedSize(TITLE_H - 1, TITLE_H - 1);
    closeBtn->setFont(fontManager.textFont(fontSize));
    connect(closeBtn, SIGNAL(realReleased()), this, SLOT(_closeSlot()));

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(0, 1, 0, 0);
    titleLayout->addWidget(_titleLabel, 1);
    titleLayout->addWidget(closeBtn, 1, Qt::AlignVCenter);

    // 警告栏。
    _warn = new QLabel("");
    _warn->setAlignment(Qt::AlignCenter);
    _warn->setFixedHeight(30);
    _warn->setFont(fontManager.textFont(fontManager.getFontSize(3)));
//    QPalette warn_p;
//    warn_p.setColor(QPalette::Foreground, Qt::red);
//    _warn->setPalette(warn_p);
    _warn->setStyleSheet("color:White;background:red");
    _warn->setText(trs("NIBPServiceError"));
    _warn->setVisible(false);

    _subMenus = new QStackedWidget();
    connect(_subMenus, SIGNAL(currentChanged(int)), this, SLOT(_currentMenuGroup(int)));

    _mainLayout = new QVBoxLayout();
    _mainLayout->setContentsMargins(BORDER_W, 0, BORDER_W, BORDER_W);
    _mainLayout->setSpacing(0);
    _mainLayout->addLayout(titleLayout);
    _mainLayout->addWidget(_warn);
    _mainLayout->addWidget(_subMenus);

    setLayout(_mainLayout);

    //supervisorMenuManager
    _subMenus->addWidget(&publicMenuManager);

    //supervisorMenuManager
//    _subMenus->addWidget(&supervisorMenuManager);

    _subMenus->setCurrentWidget(&publicMenuManager);
}

/**************************************************************************************************
* 功能: 添加子菜单
*************************************************************************************************/
void MenuManager::_addSubMenu(void)
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
MenuManager::~MenuManager()
{

}

