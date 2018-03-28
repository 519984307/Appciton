#include "NIBPRepair.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "LabelButton.h"
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QPainter>
#include <QBitmap>
#include <QScrollArea>
#include <QScrollBar>
#include "ColorManager.h"
#include "SubMenu.h"
#include "Debug.h"
#include "FontManager.h"

#include "NIBPCalibrate.h"
#include "NIBPZeroPoint.h"
#include "NIBPManometer.h"
#include "NIBPPressureControl.h"



NIBPRepair *NIBPRepair::_selfObj = NULL;

/**************************************************************************************************
 * 功能：初始化。
 *************************************************************************************************/
void NIBPRepair::init()
{
    _repairError = false;
    // 增加一个列表项。
    _itemList->setFocus();
    QListWidgetItem *listItem = _itemList->item(0);
    _itemList->setCurrentItem(listItem);

    if (nibpParamService.isErrorDisable())
    {
        warnShow(true);
    }
    else
    {
        if (nibpParamService.connectWrongFlag)
        {
            warnShow(true);
        }
        else
        {
            warnShow(false);
        }
    }

    _replyFlag = false;

    // 转换到状态。
    nibpParamService.switchState(NIBP_Service_REPAIR);
    nibpParamService.serviceEnter(true);
}

/**************************************************************************************************
 * 进入维护模式的应答。
 *************************************************************************************************/
void NIBPRepair::unPacket(bool flag)
{
    _replyFlag = flag;
}

/**************************************************************************************************
 * 返回列表。
 *************************************************************************************************/
void NIBPRepair::returnMenu()
{
    nibpcalibrate.init();
    nibpmanometer.init();
    nibppressurecontrol.init();
    nibpzeropoint.init();
    _itemList->setFocusPolicy(Qt::StrongFocus);
    _itemList->setFocus();
}

/**************************************************************************************************
 * 提示框显示。
 *************************************************************************************************/
void NIBPRepair::messageBox(void)
{
    messageBoxWait->exec();
}

/**************************************************************************************************
 * 警告框显示。
 *************************************************************************************************/
void NIBPRepair::warnShow(bool enable)
{
    // 模块硬件出错
    if (nibpParamService.isErrorDisable())
    {
        _warn->setText(trs("NIBPModuleError"));
    }
    // 模块通信错误
    else
    {
        _warn->setText(trs("NIBPServiceModuleErrorQuitTryAgain"));
        if(enable)
        {
            if (messageBoxWait != NULL && messageBoxWait->isVisible())
            {
                messageBoxWait->close();
            }
            messageBoxError->exec();
        }
        else
        {
            if (messageBoxError != NULL && messageBoxError->isVisible())
            {
                messageBoxError->close();
            }
        }
    }

    _warn->setVisible(enable);
    _repairError = enable;
}

/**************************************************************************************************
 * 重新聚焦菜单列表。
 *************************************************************************************************/
void NIBPRepair::_returnMenuList()
{
    nibpParamService.serviceReturn();
    setFocusOrder(true);
    _itemList->setFocus();
}

/**************************************************************************************************
 * 需要更新标题。
 *************************************************************************************************/
void NIBPRepair::_titleChanged(void)
{
    SubMenu *m = (SubMenu*)_subMenu->currentWidget();
    _title->setText(m->desc());
}

/**************************************************************************************************
 * 子菜单页聚焦
 *************************************************************************************************/
void NIBPRepair::_itemClicked()
{
    if (_replyFlag && !getRepairError())
    {
        setFocusOrder(false);
        SubMenu *m = (SubMenu*)_scorllArea->widget();
        m->focusFirstItem();
    }
    else
    {
        if (getRepairError())
        {
            warnShow(true);
            return;
        }
        if (!_replyFlag)
        {
            messageBox();
        }
    }
}

/**************************************************************************************************
 * 关闭窗口
 *************************************************************************************************/
void NIBPRepair::_closeMenu()
{
    // 转换到状态，发送退出服务模式
    nibpParamService.switchState(NIBP_Service_REPAIR);
    nibpParamService.serviceReturn();

    // 转换到状态，关闭窗口
    nibpParamService.switchState(NIBP_Service_NULL);
    close();
}

/**************************************************************************************************
 * 关闭按钮聚焦
 *************************************************************************************************/
void NIBPRepair::_closeBtnSetFoucs()
{
    _closeBtn->setFocus();
}

/**************************************************************************************************
 * 改变滚动区域滚动条的值
 *************************************************************************************************/
void NIBPRepair::_changeScrollValue(int value)
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
 * 重绘制。
 *************************************************************************************************/
void NIBPRepair::paintEvent(QPaintEvent *e)
{
    // 绘制圆角背景。
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.setBrush(palette().background());
//    painter.drawRoundedRect(rect(), 5, 5);


    QPainterPath clipPath;
    clipPath.addRoundedRect(this->rect(), 5, 5);

    QPainter barPainter(this);
    barPainter.setRenderHint(QPainter::Antialiasing);
    barPainter.setClipPath(clipPath);

    // 绘制边框。
    QPen pen;
    pen.setColor(colorManager.getBarBkColor());
    pen.setWidth(_borderWidth * 2);
    barPainter.setPen(pen);
    barPainter.setBrush(palette().background());
    barPainter.drawRect(rect());

    QRect r = rect();
    r.setBottom(_titleBarHeight);
    barPainter.fillRect(r, colorManager.getBarBkColor());

    QWidget::paintEvent(e);
}

/**************************************************************************************************
 * 按键事件。
 *************************************************************************************************/
void NIBPRepair::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            if (Qt::StrongFocus == _itemList->focusPolicy() && _closeBtn->hasFocus())
            {
                _itemList->setCurrentRow(_itemList->count() - 1);
                _itemList->setFocus();
            }
            else
            {
                if (_return->hasFocus())
                {
                    _changeScrollValue(1);
                }

                focusPreviousChild();
            }
            return;
        case Qt::Key_Down:
        case Qt::Key_Right:
            if (Qt::StrongFocus == _itemList->focusPolicy() && _closeBtn->hasFocus())
            {
                _itemList->setCurrentRow(0);
                _itemList->setFocus();
            }
            else
            {
                if (_closeBtn->hasFocus())
                {
                    _changeScrollValue(0);
                }

                focusNextChild();
            }
            return;
        default:
            break;
    }

    QWidget::keyPressEvent(event);
}

/**************************************************************************************************
 * 功能： 改变当前页。
 * 参数：
 *      current: 当前页对象。
 *      previous: 上个页对象。
 *************************************************************************************************/
void NIBPRepair::_changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
    {
        current = previous;
    }

    //重新添加submenu，滚动区域设置新的widget时，原来的会被删除
    QWidget *win = _scorllArea->takeWidget();
    if (NULL != win)
    {
        int index = _itemList->row(previous);
        _subMenu->insertWidget(index, win);
    }

    _subMenu->setCurrentIndex(_itemList->row(current));
    SubMenu *m = (SubMenu*)_subMenu->currentWidget();

    _scorllArea->setWidget(m);

    _title->setText(m->desc());
}

/**************************************************************************************************
 * 功能：反向操作显现或隐藏。
 *************************************************************************************************/
void NIBPRepair::popup(int x, int y)
{
    setFocusOrder(true);

    move(x, y);

    show();
}

/**************************************************************************************************
 * 功能：添加子菜单。
 * 参数：
 *      SubMenu: 菜单对象。
 *************************************************************************************************/
void NIBPRepair::addSubmenu(SubMenu *subMenu)
{
    // 增加一个列表项。
    QListWidgetItem *listItem = new QListWidgetItem(_itemList);
    listItem->setText(subMenu->name());
    listItem->setTextAlignment(Qt::AlignCenter);
    listItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    listItem->setFont(fontManager.textFont(16));
    listItem->setSizeHint(QSize(0, _listItemHeight));

    // 添加子菜单。
    _subMenu->addWidget(subMenu);

    // 连接菜单关闭信息。
    connect(subMenu, SIGNAL(closeMenu()), this, SLOT(close()));
    connect(subMenu, SIGNAL(titleChanged()), this, SLOT(_titleChanged()));
    connect(subMenu, SIGNAL(scrollAreaChange(int)), this, SLOT(_changeScrollValue(int)));
}


/**************************************************************************************************
 * 设置焦点顺序。
 *************************************************************************************************/
void NIBPRepair::setFocusOrder(bool flag)
{
    QList<QWidget *> allWidget;
    allWidget.clear();

    if (flag)
    {
        _itemList->setFocusPolicy(Qt::StrongFocus);
        allWidget.append(_itemList);
        allWidget.append(_closeBtn);
    }
    else
    {
        _itemList->setFocusPolicy(Qt::NoFocus);
        allWidget.append(_scorllArea);
        allWidget.append(_return);
        allWidget.append(_closeBtn);
    }

    int count = allWidget.count();
    for (int i = 0; i < count - 1; ++i)
    {
        setTabOrder(allWidget.at(i), allWidget.at(i + 1));
    }

    allWidget.clear();
}

bool NIBPRepair::getRepairError(void)
{
    return _repairError;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPRepair::NIBPRepair() : MenuGroup("SupervisorMenuManager")
{
    _repairError = false;
    _replyFlag = false;
    messageBoxWait = new IMessageBox(trs("Warn"), trs("NIBPServiceWaitAgain"), QStringList(trs("EnglishYESChineseSURE")));
    messageBoxError = new IMessageBox(trs("Warn"), trs("NIBPServiceModuleErrorQuitTryAgain"), QStringList(trs("EnglishYESChineseSURE")));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPRepair::~NIBPRepair()
{
    delete messageBoxWait;
    delete messageBoxError;
}
