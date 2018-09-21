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
#include <QSignalMapper>
#include <QDebug>
#include <QResizeEvent>



#define PREFER_SOFTKEY_WIDTH 98
#define SOFTKEY_SPACING 2
#define SOFTKEY_MARGIN 2

typedef QMap<SoftKeyActionType, SoftkeyActionBase*> SoftKeyActionMap;

class SoftKeyManagerPrivate
{
public:
    SoftKeyManagerPrivate()
        : totalPages(1),
          curPage(0),
          currentAction(NULL),
          currentActionType(SOFTKEY_ACTION_NONE),
          mainMenuKeyWidget(NULL),
          leftPageKeyWidget(NULL),
          rightPageKeyWidget(NULL),
          emptyKeyDesc("", "", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
          signalMapper(NULL),
          dynamicKeyLayout(NULL)
    {}

    void resetPageInfo()
    {
        curPage = 0;
        if (dynamicKeyWidgets.count() == 0 || currentAction == NULL)
        {
            totalPages = 1;
        }
        else
        {
            int dynamicNum = dynamicKeyWidgets.count();
            totalPages = (currentAction->getActionDescNR()  + dynamicNum - 1) / dynamicNum;
        }
    }

    void layoutKeyDesc()
    {
        if (curPage == 0)
        {
            KeyActionDesc leftKeyDesc;
            leftKeyDesc.focus = false;
            leftPageKeyWidget->setContent(&leftKeyDesc);
        }
        else
        {
            KeyActionDesc leftKeyDesc;
            leftKeyDesc.iconPath = ICON_FILE_LEFT;
            leftPageKeyWidget->setContent(&leftKeyDesc);
        }


        if (curPage == totalPages - 1)
        {
            KeyActionDesc rightKeyDesc;
            rightKeyDesc.focus = false;
            rightPageKeyWidget->setContent(&rightKeyDesc);
        }
        else
        {
            KeyActionDesc rightKeyDesc;
            rightKeyDesc.iconPath = ICON_FILE_RIGHT;
            rightPageKeyWidget->setContent(&rightKeyDesc);
        }

        if (currentAction && dynamicKeyWidgets.count())
        {
            int startDescIndex = curPage * dynamicKeyWidgets.count();
            int keyIndex = 0;
            while (keyIndex < dynamicKeyWidgets.count())
            {
                KeyActionDesc *desc = currentAction->getActionDesc(startDescIndex);
                if (desc)
                {
                    dynamicKeyWidgets.at(keyIndex)->setContent(desc);
                }
                else
                {
                    dynamicKeyWidgets.at(keyIndex)->setContent(&emptyKeyDesc);
                }
                keyIndex++;
                startDescIndex++;
            }
        }
    }

    int totalPages;     // total pages
    int curPage;        // current page index
    SoftkeyActionBase *currentAction;
    SoftKeyActionType currentActionType;
    SoftkeyWidget *mainMenuKeyWidget;
    SoftkeyWidget *leftPageKeyWidget;
    SoftkeyWidget *rightPageKeyWidget;
    QList<SoftkeyWidget*> dynamicKeyWidgets;
    KeyActionDesc emptyKeyDesc; // empty key description, use for the empty soft key
    SoftKeyActionMap actionMaps;
    QSignalMapper *signalMapper;
    QHBoxLayout *dynamicKeyLayout;
};

/***************************************************************************************************
 * 功能：获取软按键动作对象。
 * 参数：
 *      t： 指定动作对象的类型。
 **************************************************************************************************/
SoftkeyActionBase *SoftKeyManager::getAction(SoftKeyActionType t)
{
    SoftKeyActionMap::iterator it = d_ptr->actionMaps.find(t);
    if (it == d_ptr->actionMaps.end())
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
    subWidgets.append(d_ptr->mainMenuKeyWidget);
    subWidgets.append(d_ptr->leftPageKeyWidget);
    for (int i = 0; i < d_ptr->dynamicKeyWidgets.count(); i++ )
    {
        subWidgets.append(d_ptr->dynamicKeyWidgets.at(i));
    }
    subWidgets.append(d_ptr->rightPageKeyWidget);
}

void SoftKeyManager::_dynamicKeyClicked(int index)
{
    int descIndex = d_ptr->curPage * d_ptr->dynamicKeyWidgets.count() + index;
    if (d_ptr->currentAction)
    {
        KeyActionDesc *desc = d_ptr->currentAction->getActionDesc(descIndex);
        if (desc != NULL && desc->hook != NULL)
        {
            desc->hook(0);
        }
    }
}

void SoftKeyManager::_fixedKeyClicked()
{
    SoftkeyWidget *w = qobject_cast<SoftkeyWidget*>(sender());
    if (d_ptr->mainMenuKeyWidget == w)
    {
        KeyActionDesc *desc = SoftkeyActionBase::getBaseActionDesc(SOFT_BASE_KEY_MAIN_SETUP);
        if (desc != NULL && desc->hook != NULL)
        {
            desc->hook(0);
        }
    }
    else if (d_ptr->leftPageKeyWidget == w)
    {
        if (d_ptr->curPage > 0)
        {
            d_ptr->curPage--;
        }
        d_ptr->layoutKeyDesc();
    }
    else if (d_ptr->rightPageKeyWidget == w)
    {
        if (d_ptr->curPage < d_ptr->totalPages - 1)
        {
            d_ptr->curPage++;
        }
        d_ptr->layoutKeyDesc();
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

void SoftKeyManager::resizeEvent(QResizeEvent *e)
{
    // calculate the avaliable dynamic softkey number

    int w = e->size().width();
    // minus main menu width , left/right page key width, spacing and margin
    int avaliableWidth = w - (PREFER_SOFTKEY_WIDTH + SOFTKEY_SPACING)
            - (PREFER_SOFTKEY_WIDTH / 2) - SOFTKEY_SPACING - 2 * SOFTKEY_MARGIN;

    int dynamicKeyNum = avaliableWidth / (PREFER_SOFTKEY_WIDTH + SOFTKEY_SPACING);

    while (d_ptr->dynamicKeyWidgets.count() < dynamicKeyNum)
    {
        SoftkeyWidget *k = new SoftkeyWidget();
        k->setFixedWidth(PREFER_SOFTKEY_WIDTH);
        d_ptr->dynamicKeyLayout->addWidget(k);
        connect(k, SIGNAL(released()), d_ptr->signalMapper, SLOT(map()));
        d_ptr->signalMapper->setMapping(k, d_ptr->dynamicKeyWidgets.count());
        d_ptr->dynamicKeyWidgets.append(k);
    }

    d_ptr->resetPageInfo();
    d_ptr->layoutKeyDesc();
    this->layout()->activate();
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
void SoftKeyManager::setContent(SoftKeyActionType type)
{
    if (type == d_ptr->currentActionType)
    {
        return;
    }

    SoftKeyActionMap::iterator iter = d_ptr->actionMaps.find(type);
    if (iter == d_ptr->actionMaps.end())
    {
        return;
    }


    d_ptr->currentActionType = type;
    d_ptr->currentAction = iter.value();

    d_ptr->resetPageInfo();

    d_ptr->layoutKeyDesc();
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
SoftKeyManager::SoftKeyManager() : IWidget("SoftKeyManager"),
    d_ptr(new SoftKeyManagerPrivate())
{
    setFocusPolicy(Qt::NoFocus);
    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->setMargin(SOFTKEY_MARGIN);
    hbox->setSpacing(SOFTKEY_SPACING);

    // main menu
    d_ptr->mainMenuKeyWidget = new SoftkeyWidget();
    d_ptr->mainMenuKeyWidget->setContent(SoftkeyActionBase::getBaseActionDesc(SOFT_BASE_KEY_MAIN_SETUP));
    d_ptr->mainMenuKeyWidget->setFixedWidth(PREFER_SOFTKEY_WIDTH);
    connect(d_ptr->mainMenuKeyWidget, SIGNAL(released()), this, SLOT(_fixedKeyClicked()));
    hbox->addWidget(d_ptr->mainMenuKeyWidget);

    // left page
    d_ptr->leftPageKeyWidget = new SoftkeyWidget();
    d_ptr->leftPageKeyWidget->setMinimumWidth(PREFER_SOFTKEY_WIDTH / 2);
    d_ptr->leftPageKeyWidget->setMaximumWidth(PREFER_SOFTKEY_WIDTH);
    connect(d_ptr->leftPageKeyWidget, SIGNAL(released()), this, SLOT(_fixedKeyClicked()));
    hbox->addWidget(d_ptr->leftPageKeyWidget, 1);

    // dynamic layout
    QWidget *placeHolder =  new QWidget();
    placeHolder->setAttribute(Qt::WA_NoSystemBackground);
    hbox->addWidget(placeHolder, 1);
    d_ptr->dynamicKeyLayout = new QHBoxLayout(placeHolder);
    d_ptr->dynamicKeyLayout->setSpacing(SOFTKEY_SPACING);
    d_ptr->dynamicKeyLayout->setMargin(0);

    // right page
    d_ptr->rightPageKeyWidget = new SoftkeyWidget();
    d_ptr->rightPageKeyWidget->setMinimumWidth(PREFER_SOFTKEY_WIDTH / 2);
    d_ptr->rightPageKeyWidget->setMaximumWidth(PREFER_SOFTKEY_WIDTH);
    connect(d_ptr->rightPageKeyWidget, SIGNAL(released()), this, SLOT(_fixedKeyClicked()));
    hbox->addWidget(d_ptr->rightPageKeyWidget, 1);

    d_ptr->signalMapper = new QSignalMapper(this);
    connect(d_ptr->signalMapper, SIGNAL(mapped(int)), this, SLOT(_dynamicKeyClicked(int)));

    setLayout(hbox);

    // new出SoftkeyAction对象。
    SoftkeyActionBase *action = new MonitorSoftkeyAction();
    d_ptr->actionMaps.insert(action->getType(), action);
}

/***************************************************************************************************
 * 析构。
 **************************************************************************************************/
SoftKeyManager &SoftKeyManager::getInstance()
{
    static SoftKeyManager *instance;
    if (instance == NULL)
    {
        instance = new SoftKeyManager();
    }
    return *instance;
}

SoftKeyManager::~SoftKeyManager()
{
    qDeleteAll(d_ptr->actionMaps);
}
