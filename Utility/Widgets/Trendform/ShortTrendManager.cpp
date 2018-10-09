/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/9
 **/

#include "ShortTrendManager.h"
#include "ShortTrendWidgetLabel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Debug.h"
#include <QTimer>
#include "IConfig.h"
#include "ParamInfo.h"
#include "ColorManager.h"
#include "WindowManager.h"

ShortTrendManager *ShortTrendManager::_selfObj = NULL;

/**************************************************************************************************
 * 构造函数。
 *************************************************************************************************/
ShortTrendManager::ShortTrendManager() : IWidget("ShortTrendManager"), _shortTrend(SHORT_TREND_NR)
{
    setFocusPolicy(Qt::NoFocus);
//    setValueRange(64, 192);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    _shortTrendWidget0 = new ShortTrendWidget();
    _shortTrendWidget1 = new ShortTrendWidget();

    _shortTrendItems.insert(SHORT_TREND_0, _shortTrendWidget0);
    _shortTrendItems.insert(SHORT_TREND_1, _shortTrendWidget1);

    mainLayout->addWidget(_shortTrendWidget0);
    mainLayout->addWidget(_shortTrendWidget1);

    setLayout(mainLayout);

    _timer = new QTimer();
    _timer->setInterval(1000);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_timeOutSlot()));
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
ShortTrendManager::~ShortTrendManager()
{
    if (NULL != _timer)
    {
        delete _timer;
        _timer = NULL;
    }

    // 清除窗体
    QList<ShortTrendWidget *> winList = _shortTrendItems.values();
    foreach(IWidget *w, winList)
    {
        if (w)
        {
            delete w;
            w = NULL;
        }
    }
    _shortTrendItems.clear();

    _shortTrendSubParamID0.clear();
    _shortTrendSubParamID1.clear();
}

/**************************************************************************************************
 * 添加子项。
 *************************************************************************************************/
void ShortTrendManager::_addItem(void)
{
    QString path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorTrend|ShortTrendOrder0";

    if (!path.isEmpty())
    {
        QString widgets;
        systemConfig.getStrValue(path, widgets);
        QStringList waveformNames = widgets.split(",");

//        for (int i = 0; i < waveformNames.size(); i++)
//        {
//            _shortTrendWidget0->addItem(paramInfo.getSubParamID(waveformNames[i]));
//        }
    }

    path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorTrend|ShortTrendOrder1";

    if (!path.isEmpty())
    {
        QString widgets;
        systemConfig.getStrValue(path, widgets);
        QStringList waveformNames = widgets.split(",");

//        for (int i = 0; i < waveformNames.size(); i++)
//        {
//            _shortTrendWidget1->addItem(paramInfo.getSubParamID(waveformNames[i]));
//        }
    }
}

void ShortTrendManager::getSubFocusWidget(QList<QWidget *> &subWidget) const
{
    subWidget.clear();
    QList<QWidget *> widgetItem;
//    widgetItem.clear();
//    _shortTrendWidget0->getFocusWidgetItem(widgetItem);
//    subWidget += widgetItem;

//    widgetItem.clear();
//    _shortTrendWidget1->getFocusWidgetItem(widgetItem);
//    subWidget += widgetItem;


    QList<ShortTrendWidget *> _labelItems = _shortTrendItems.values();
    int count = _labelItems.count();
    if (0 == count)
    {
        return;
    }

    for (int i = count - 1; i >= 0; --i)
    {
        QWidget *widget = static_cast<QWidget *>(_labelItems.at(i));
        if (NULL != widget && (Qt::NoFocus != _labelItems.at(i)->focusPolicy()))
        {
            subWidget.append(widget);
        }
        if (NULL != _labelItems.at(i))
        {
            widgetItem.clear();
            _labelItems.at(i)->getFocusWidgetItem(widgetItem);
            subWidget += widgetItem;
        }
    }
}

void ShortTrendManager::_focusTrendformWidget(void)
{
    QMap<ShortTrend, ShortTrendWidget *>::Iterator iter = _shortTrendItems.find(_shortTrend);
    if (iter != _shortTrendItems.end())
    {
        iter.value()->setFocus();
    }
}


/**************************************************************************************************
 * 获取当前显示的波形窗体内容。
 *************************************************************************************************/
void ShortTrendManager::_getCurrentshortTrend(QStringList &shortTrend)
{
//    shortTrend.clear();

//    QString path;
//    switch (_shortTrend)
//    {
//    case SHORT_TREND_0:
//        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorTrend|ShortTrendOrder0";
//        break;
//    case SHORT_TREND_1:
//        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorTrend|ShortTrendOrder1";
//        break;
//    default:
//        break;
//    }

//    QString trendNames;
//    systemConfig.getStrValue(path, trendNames);
//    shortTrend = trendNames.split(",");

    QList<SubParamID> subIDlist;
    subIDlist.clear();

    QMap<ShortTrend, ShortTrendWidget *>::Iterator iter;

    switch (_shortTrend)
    {
    case SHORT_TREND_0:
        _shortTrendSubParamID0.clear();
        iter = _shortTrendItems.find(_shortTrend);
        if (iter != _shortTrendItems.end())
        {
            iter.value()->getSubParamList(_shortTrendSubParamID0);
        }
        subIDlist += _shortTrendSubParamID0;
        break;
    case SHORT_TREND_1:
        _shortTrendSubParamID1.clear();
        iter = _shortTrendItems.find(_shortTrend);
        if (iter != _shortTrendItems.end())
        {
            iter.value()->getSubParamList(_shortTrendSubParamID1);
        }
        subIDlist += _shortTrendSubParamID1;
        break;
    default:
        break;
    }
    for (int i = 0; i < subIDlist.size(); i++)
    {
        shortTrend += paramInfo.getSubParamName(subIDlist[i]);
    }
}

/**************************************************************************************************
 * 设置当前显示的波形窗体内容。
 *************************************************************************************************/
void ShortTrendManager::_setCurrentshortTrend(const QStringList &shortTrend)
{
    QString path;
    switch (_shortTrend)
    {
    case SHORT_TREND_0:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorTrend|ShortTrendOrder0";
        break;
    case SHORT_TREND_1:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorTrend|ShortTrendOrder1";
        break;
    default:
        break;
    }

    if (!path.isEmpty())
    {
        QString trend = shortTrend.join(",");
        systemConfig.setStrValue(path, trend);
    }
}

/**************************************************************************************************
 * 设置当前波形窗体的控件。
 *************************************************************************************************/
void ShortTrendManager::_setCurrentshortTrendWidget(const QString &shortTrend)
{
    for (int i = 0; i < _shortTrendSubParamID0.count(); i++)
    {
        if (paramInfo.getSubParamName(_shortTrendSubParamID0[i]) == shortTrend)
        {
            _shortTrend = SHORT_TREND_0;
            return;
        }
    }
    for (int i = 0; i < _shortTrendSubParamID1.count(); i++)
    {
        if (paramInfo.getSubParamName(_shortTrendSubParamID1[i]) == shortTrend)
        {
            _shortTrend = SHORT_TREND_1;
            return;
        }
    }
    _shortTrend = SHORT_TREND_NR;
}

/**************************************************************************************************
 * 当前显示的波形窗体的SubParamID列表。
 *************************************************************************************************/
void ShortTrendManager::getSubParamList(QList<SubParamID> &subIDlist)
{
    subIDlist.clear();

    _shortTrendSubParamID0.clear();
    _shortTrendWidget0->getSubParamList(_shortTrendSubParamID0);
    subIDlist += _shortTrendSubParamID0;

    _shortTrendSubParamID1.clear();
    _shortTrendWidget1->getSubParamList(_shortTrendSubParamID1);
    subIDlist += _shortTrendSubParamID1;
}

/**************************************************************************************************
 * 当前显示的波形窗体的QString列表。
 *************************************************************************************************/
void ShortTrendManager::getSubParamList(QStringList &trendList)
{
    trendList.clear();

    QList<SubParamID> subIDlist;
    getSubParamList(subIDlist);
    for (int i = 0; i < subIDlist.size(); i++)
    {
        trendList += paramInfo.getSubParamName(subIDlist[i]);
    }
}

/***************************************************************************************************
 * 功能：获取当前界面还能新增几道波形。。
 * 返回：剩余的波形道数。
 **************************************************************************************************/
int ShortTrendManager::getTrendNR(const QString &trendform)
{
    int currentTrend;
    int totalTrend = 4;
    _setCurrentshortTrendWidget(trendform);
    if (_shortTrend == SHORT_TREND_0)
    {
        currentTrend = _shortTrendSubParamID0.count();
    }
    else if (_shortTrend == SHORT_TREND_1)
    {
        currentTrend = _shortTrendSubParamID1.count();
    }
    else
    {
        currentTrend = totalTrend;
    }

    return ((totalTrend - currentTrend) < 0) ? 0 : (totalTrend - currentTrend);
}

/***************************************************************************************************
 * 功能：替换波形。
 * 参数：
 *      oldTrendform：被替换的波形。
 *      newTrendform：新波形。
 *      order:设置焦点顺序
 **************************************************************************************************/
void ShortTrendManager::replaceTrendform(const QString &oldTrendform,
        const QString &newTrendform, bool setFocus, bool /*order*/)
{
    _setCurrentshortTrendWidget(oldTrendform);

    QStringList currentTrend;
    _getCurrentshortTrend(currentTrend);

    int i = 0;
    for (; i < currentTrend.size(); i++)
    {
        if (currentTrend[i] == oldTrendform)
        {
            currentTrend[i] = newTrendform;
            _setCurrentshortTrend(currentTrend);
            break;
        }
    }

    if (i == currentTrend.size())
    {
        return;
    }

    QMap<ShortTrend, ShortTrendWidget *>::iterator it;
    for (it = _shortTrendItems.begin(); it != _shortTrendItems.end(); ++it)
    {
        if (NULL != it.value())
        {
            it.value()->removeLabel();
        }
    }

    _addItem();

    for (it = _shortTrendItems.begin(); it != _shortTrendItems.end(); ++it)
    {
        if (NULL != it.value())
        {
            it.value()->addWidget();
        }
    }

    if (setFocus)
    {
        _focusTrendformWidget();
    }
}

/***************************************************************************************************
 * 功能：插入波形。
 * 参数：
 *      frontTrendform 前面的波形。
 *      insertedTrendform 插入的波形。
 **************************************************************************************************/
void ShortTrendManager::insertTrendform(const QString &frontTrendform,
                                        const QString &insertedTrendform, bool setFocus, bool /*order*/)
{
    _setCurrentshortTrendWidget(frontTrendform);

    QStringList currentTrend;
    _getCurrentshortTrend(currentTrend);

    if (-1 != currentTrend.indexOf(insertedTrendform))
    {
        return;
    }

    int i = 0;
    int size = currentTrend.size();
    for (; i < size; i++)
    {
        if (currentTrend[i] == frontTrendform)
        {
            currentTrend.insert(i + 1, insertedTrendform);
            _setCurrentshortTrend(currentTrend);
            break;
        }
    }

    if (i == size)
    {
        return;
    }

    QMap<ShortTrend, ShortTrendWidget *>::iterator it;
    for (it = _shortTrendItems.begin(); it != _shortTrendItems.end(); ++it)
    {
        if (NULL != it.value())
        {
            it.value()->removeLabel();
        }
    }

    _addItem();

    for (it = _shortTrendItems.begin(); it != _shortTrendItems.end(); ++it)
    {
        if (NULL != it.value())
        {
            it.value()->addWidget();
        }
    }

    if (setFocus)
    {
        _focusTrendformWidget();
    }
}

/***************************************************************************************************
 * 功能：移除波形。
 * 参数：
 *      waveform：待移除波形的名称。
 **************************************************************************************************/
void ShortTrendManager::removeTrendform(const QString &trendform, bool setFocus)
{
    _setCurrentshortTrendWidget(trendform);

    QStringList currentTrend;
    _getCurrentshortTrend(currentTrend);
    if (-1 == currentTrend.indexOf(trendform))
    {
        return;
    }

    currentTrend.removeOne(trendform);
    _setCurrentshortTrend(currentTrend);

    QMap<ShortTrend, ShortTrendWidget *>::iterator it;
    for (it = _shortTrendItems.begin(); it != _shortTrendItems.end(); ++it)
    {
        if (NULL != it.value())
        {
            it.value()->removeLabel();
        }
    }

    _addItem();

    for (it = _shortTrendItems.begin(); it != _shortTrendItems.end(); ++it)
    {
        if (NULL != it.value())
        {
            it.value()->addWidget();
        }
    }

    if (setFocus)
    {
        _focusTrendformWidget();
    }
}

void ShortTrendManager::hideEvent(QHideEvent *e)
{
    IWidget::hideEvent(e);
    QMap<ShortTrend, ShortTrendWidget *>::iterator it;
    for (it = _shortTrendItems.begin(); it != _shortTrendItems.end(); ++it)
    {
        if (NULL != it.value())
        {
            it.value()->removeLabel();
        }
    }
    _timer->stop();
}

void ShortTrendManager::showEvent(QShowEvent *e)
{
    IWidget::showEvent(e);

    _addItem();

    QMap<ShortTrend, ShortTrendWidget *>::iterator it;
    for (it = _shortTrendItems.begin(); it != _shortTrendItems.end(); ++it)
    {
        if (NULL != it.value())
        {
            it.value()->addWidget();
        }
    }

    _timer->start();
}

void ShortTrendManager::_timeOutSlot()
{
    QMap<ShortTrend, ShortTrendWidget *>::iterator it;
    for (it = _shortTrendItems.begin(); it != _shortTrendItems.end(); ++it)
    {
        if (NULL != it.value())
        {
            it.value()->refresh();
        }
    }
}
