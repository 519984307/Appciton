/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/20
 **/


#include "SoftKeyManager.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QPainter>
#include "IConfig.h"
#include "SoftKeyWidget.h"
#include "MonitorSoftkeyAction.h"
#include "ECG12LeadSoftkeyAction.h"
#include "RescueDataSoftKeyAction.h"
#include "DelRescueDataSoftkeyAction.h"
#include "CalculateSoftkeyAction.h"
#include "ECGParam.h"
#include "WindowManager.h"

SoftKeyManager *SoftKeyManager::_selfObj = NULL;

#define SOFTKEY_WIDGET_NR  12      // 按钮显示的数量
#define SOFTKEY_HEAD_NR 2          // 开头固定软按键个数
#define SOFTKEY_TAIL_NR 1          // 结尾固定软按键个数

/***************************************************************************************************
 * 功能：前面板按键处理。
 * 参数:
 *     index:左侧第几个键按下
 **************************************************************************************************/
void SoftKeyManager::softKeyPress(int index)
{
    if (index < SOFTKEY_WIDGET_NR)
    {
        _keyWidgets[index]->pressedEffect();
    }

    if (_currentAction != NULL)
    {
        int i = _currentPage * SOFTKEY_WIDGET_NR + index;
        KeyActionDesc *desc = _currentAction->getActionDesc(i);
        if ((desc != NULL) && (desc->hook != NULL))
        {
            desc->hook(1);
        }
    }
}

/***************************************************************************************************
 * 功能：前面板按键处理。
 * 参数:
 *     index:左侧第几个键弹起。
 **************************************************************************************************/
void SoftKeyManager::softkeyReleased(int index)
{
    if (index < SOFTKEY_WIDGET_NR)
    {
        _keyWidgets[index]->normalEffect();
    }

    if (_currentAction != NULL)
    {
        int i = _currentPage * SOFTKEY_WIDGET_NR + index;
        KeyActionDesc *desc = _currentAction->getActionDesc(i);
        if ((desc != NULL) && (desc->hook != NULL))
        {
            desc->hook(0);
        }
    }
}

/***************************************************************************************************
 * 功能：切换到指定的动作组。
 **************************************************************************************************/
void SoftKeyManager::switchTo(SoftKeyActionType type)
{
    ActionMap::iterator it = _actions.find(type);
    if (it == _actions.end())
    {
        return;
    }

    if (type == _curSoftKeyType)
    {
        return;
    }

    _curSoftKeyType = type;
    _currentAction = it.value();
    _totalPages = _currentAction->getActionDescNR() / SOFTKEY_WIDGET_NR;
    _totalPages += (_currentAction->getActionDescNR() % SOFTKEY_WIDGET_NR) ? 1 : 0;
    _currentPage = 0;
    _layoutKeys();
}

/***************************************************************************************************
 * 功能：切换到上一个page。
 **************************************************************************************************/
void SoftKeyManager::previousPage()
{
    if (_currentAction == NULL)
    {
        return;
    }

    _currentPage--;
    _currentPage = (_currentPage <= 0) ? 0 : _currentPage;
    _layoutKeys();
}

/***************************************************************************************************
 * 功能：切换到下一个page。
 **************************************************************************************************/
void SoftKeyManager::nextPage(void)
{
    if (_currentAction == NULL)
    {
        return;
    }

    _currentPage++;
    _currentPage = (_currentPage >= _totalPages) ? (_totalPages - 1) : _currentPage;
    _layoutKeys();
}

bool SoftKeyManager::hasPreviousPage()
{
    return _currentPage > 0;
}

bool SoftKeyManager::hasNextPage()
{
    return _currentPage < _totalPages - 1;
}

/***************************************************************************************************
 * 功能：切换到首页。
 **************************************************************************************************/
bool SoftKeyManager::returnRootPage()
{
    switch (_curSoftKeyType)
    {
    case SOFTKEY_ACTION_RESCUE_DATA:
    case SOFTKEY_ACTION_DEL_RESCUE_DATA:
        switch (windowManager.getUFaceType())
        {
        case UFACE_MONITOR_STANDARD:
            switchTo(SOFTKEY_ACTION_STANDARD);
            break;
        default:
            break;
        }
        return true;
    default:
        break;
    }

    bool ret = false;
    if (0  != _currentPage)
    {
        _currentPage = 0;
        ret = true;
    }

    _layoutKeys();
    return ret;
}

/***************************************************************************************************
 * 功能：刷新当前页。
 **************************************************************************************************/
void SoftKeyManager::refresh(bool toFirstPage)
{
    if (toFirstPage)
    {
        returnRootPage();
    }
    else
    {
        _layoutKeys();
    }
}

/***************************************************************************************************
 * 功能：获取软按键动作对象。
 * 参数：
 *      t： 指定动作对象的类型。
 **************************************************************************************************/
SoftkeyActionBase *SoftKeyManager::getAction(SoftKeyActionType t)
{
    ActionMap::iterator it = _actions.find(t);
    if (it == _actions.end())
    {
        return NULL;
    }

    return it.value();
}

/**************************************************************************************************
 * 函数说明:
 *         获取焦点子控件。
 *************************************************************************************************/
void SoftKeyManager::getSubFocusWidget(QList<QWidget *> &subWidgets) const
{
    subWidgets.clear();

    for (int i = (SOFTKEY_WIDGET_NR - 1); i >= 0; i--)
    {
        IWidget *subWidget = _keyWidgets[i];
        if (NULL != subWidget)
        {
            subWidgets.append(subWidget);
        }
    }
}

/***************************************************************************************************
 * 功能：子控件点击回调。
 **************************************************************************************************/
void SoftKeyManager::_clickKey(IWidget *w)
{
    int index = 0;
    int actionSize = _currentAction->getActionDescNR();
    for (; index < _keyWidgets.size() ; index++)
    {
        if (_keyWidgets[index] == w)
        {
            break;
        }
    }
    // menu tail
    if (index >= SOFTKEY_WIDGET_NR - SOFTKEY_TAIL_NR)
    {
        index = actionSize - (SOFTKEY_WIDGET_NR - index);
    }
    // menu body
    else if (index < SOFTKEY_WIDGET_NR - SOFTKEY_TAIL_NR && index > SOFTKEY_HEAD_NR - 1)
    {
        index = index +
                (SOFTKEY_WIDGET_NR - SOFTKEY_TAIL_NR - SOFTKEY_HEAD_NR) * _currentPage;
    }

    if (_currentAction != NULL)
    {
        KeyActionDesc *desc = _currentAction->getActionDesc(index);
        if ((desc != NULL) && (desc->hook != NULL))
        {
            desc->hook(0);
        }
    }
}

/***************************************************************************************************
 * 功能：将当前页的图标布局出来。
 **************************************************************************************************/
void SoftKeyManager::_layoutKeys(void)
{
    if (_currentAction == NULL)
    {
        return;
    }

    int startIndex = 0;
    // menu head
    for (int i = 0; i <= SOFTKEY_HEAD_NR - 1; i++)
    {
        KeyActionDesc *desc = _currentAction->getActionDesc(startIndex++);
        if (desc != NULL)
        {
            _keyWidgets[i]->setContent(*desc);
        }
    }

    // menu body
    if (_currentPage)
    {
        startIndex = startIndex +
                (SOFTKEY_WIDGET_NR - SOFTKEY_TAIL_NR - SOFTKEY_HEAD_NR)* _currentPage;
    }
    for (int i = SOFTKEY_HEAD_NR;
         i < SOFTKEY_WIDGET_NR - SOFTKEY_TAIL_NR; i++)
    {
        KeyActionDesc *desc = _currentAction->getActionDesc(startIndex++);
        if (desc != NULL)
        {
            _keyWidgets[i]->setContent(*desc);
        }
    }

    // menu tail
    int actionSize = _currentAction->getActionDescNR() - SOFTKEY_TAIL_NR;
    for (int i = SOFTKEY_WIDGET_NR - SOFTKEY_TAIL_NR; i < SOFTKEY_WIDGET_NR; i++)
    {
        KeyActionDesc *desc = _currentAction->getActionDesc(actionSize++);
        if (desc != NULL)
        {
            _keyWidgets[i]->setContent(*desc);
        }
    }
}

/***************************************************************************************************
 * 功能：重绘事件。
 * 参数：
 *      e：事件。
 **************************************************************************************************/
void SoftKeyManager::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);

    QPainter painter(this);
    painter.fillRect(this->rect(), SoftkeyWidget::backgroundColor());
}

/***************************************************************************************************
 * 功能：设置当前的界面类型，以便布局不同的按键。
 * 参数：
 *      type：界面类型。
 **************************************************************************************************/
SoftKeyActionType SoftKeyManager::uFaceTypeToSoftKeyType(UserFaceType type)
{
    SoftKeyActionType actionType = SOFTKEY_ACTION_NONE;
    switch (type)
    {
    case UFACE_MONITOR_STANDARD:
    case UFACE_MONITOR_OXYCRG:
    case UFACE_MONITOR_TREND:
    case UFACE_MONITOR_BIGFONT:
    case UFACE_MONITOR_12LEAD:
    case UFACE_MONITOR_CUSTOM:
        actionType = SOFTKEY_ACTION_STANDARD;
        break;


    default:
        break;
    }

    return actionType;
}

/***************************************************************************************************
 * 功能：设置当前的界面类型，以便布局不同的按键。
 * 参数：
 *      type：界面类型。
 **************************************************************************************************/
void SoftKeyManager::setContent(SoftKeyActionType type, bool reload)
{
    ActionMap::iterator it = _actions.find(type);
    if (it == _actions.end())
    {
        return;
    }

    if (type == _curSoftKeyType)
    {
        return;
    }

    // 计算新的Actions有多少页。
    _curSoftKeyType = type;
    _currentAction = it.value();
    _totalPages = _currentAction->getActionDescNR() / SOFTKEY_WIDGET_NR;
    _totalPages += (_currentAction->getActionDescNR() % SOFTKEY_WIDGET_NR) ? 1 : 0;

    if (!reload)
    {
        _currentPage = 0;
    }
    else
    {
        _currentPage = _totalPages - 1;
    }

    _layoutKeys();
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
SoftKeyManager::SoftKeyManager() : IWidget("SoftKeyManager")
{
    _currentAction = NULL;
    _totalPages = 0;
    _currentPage = 0;
    _curSoftKeyType = SOFTKEY_ACTION_NONE;

    setFocusPolicy(Qt::NoFocus);
    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->setMargin(2);
    hbox->setSpacing(2);
    int screenWidth = 0;
    int screenHeight = 0;


    machineConfig.getNumValue("ScreenWidth", screenWidth);
    machineConfig.getNumValue("ScreenHeight", screenHeight);
    int sum = 0;
    QStringList screen;
    QString prefix = "PrimaryCfg|UILayout|WidgetsOrder|ScreenVLayoutStretch";
    screen = systemConfig.getChildNodeNameList(prefix);
    if (screen.size() > 0)
    {
        for (int i = 0; i < screen.size(); i++)
        {
            QString string = prefix + "|" + screen[i];
            int index = 0;
            systemConfig.getNumValue(string, index);
            sum += index;
        }
    }
    else
    {
        return;
    }
    int w = 1;
    systemConfig.getNumValue("PrimaryCfg|UILayout|WidgetsOrder|ScreenVLayoutStretch|softkeyRow", w);
    //获取配置文件中软按键栏的高度
    int keySizeH = (w * screenHeight) / sum;
    keySizeH = keySizeH - 4;
    systemConfig.getNumValue("PrimaryCfg|UILayout|WidgetsOrder|softkeyAreaRowOrder|SoftkeyArea", w);
    int keySizeW = w / SOFTKEY_WIDGET_NR;

    for (int i = 0; i < SOFTKEY_WIDGET_NR; i++)
    {
        SoftkeyWidget *widget;
        if (SOFTKEY_HEAD_NR - 1 == i || i == SOFTKEY_WIDGET_NR - SOFTKEY_TAIL_NR) // 设置翻页按键大小
        {
            widget = new SoftkeyWidget(this);
            widget->setFixedSize(keySizeW / 2, keySizeH);
        }
        else
        {
            widget = new SoftkeyWidget(this);
            widget->setFixedSize(keySizeW, keySizeH);
        }
        connect(widget, SIGNAL(released(IWidget *)), this, SLOT(_clickKey(IWidget *)));
        if (i < SOFTKEY_WIDGET_NR - SOFTKEY_TAIL_NR)
        {
            hbox->addWidget(widget, 0, Qt::AlignHCenter | Qt::AlignLeft);
        }
        else
        {
            if (i == SOFTKEY_WIDGET_NR - SOFTKEY_TAIL_NR)
            {
                hbox->addStretch();
            }
            hbox->addWidget(widget, 0, Qt::AlignHCenter | Qt::AlignRight);
        }

        _keyWidgets.append(widget);
    }
    setLayout(hbox);

    // new出SoftkeyAction对象。
    SoftkeyActionBase *action = new MonitorSoftkeyAction();
    _actions.insert(action->getType(), action);
}

/***************************************************************************************************
 * 析构。
 **************************************************************************************************/
SoftKeyManager::~SoftKeyManager()
{
    ActionMap::iterator it = _actions.begin();
    for (; it != _actions.end(); ++it)
    {
        delete it.value();
    }
    _actions.clear();
}
