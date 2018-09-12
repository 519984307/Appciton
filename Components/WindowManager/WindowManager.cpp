/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/12
 **/

#include "Debug.h"
#include <QList>
#include <QMap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include "IWidget.h"
#include "WindowManager.h"
#include "SoftKeyManager.h"
#include "IConfig.h"
#include "SystemModeBarWidget.h"
#include "TrendWidget.h"
#include "CO2Param.h"
#include "RESPParam.h"
#include "ECGParam.h"
#include "ECGSymbol.h"
#include "PatientBarWidget.h"
#include "WaveWidgetSelectMenu.h"
#include "MainMenuWindow.h"
#include "ConfigManagerMenuWindow.h"
#include "UserMaintainMenuWindow.h"
#include "FactoryMaintainMenuWindow.h"
#include "ConfigEditMenuWindow.h"
#include "NIBPRepairMenuWindow.h"
#include <QApplication>
#include "Window.h"
#include "FontManager.h"
#include <QTimer>
#include "TopBarWidget.h"

struct NodeDesc
{
    NodeDesc(int xx, int yy, int ww, int hh)
    {
        x = xx;
        y = yy;
        w = ww;
        h = hh;
    }

    int x;
    int y;
    int w;
    int h;
};

typedef QMap<QString, QList<NodeDesc> > LayoutStyleMap;
static LayoutStyleMap _layoutStyle;

WindowManager *WindowManager::_selfObj = NULL;

/***************************************************************************************************
 * 功能：获取当前显示的波形窗体内容。
 **************************************************************************************************/
void WindowManager::getCurrentWaveforms(QStringList &waveformNames)
{
    waveformNames.clear();
    QString path;
    switch (_currenUserFaceType)
    {
    case UFACE_MONITOR_STANDARD:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorStandard|WaveOrder";
        break;
    case UFACE_MONITOR_12LEAD:
        path = "PrimaryCfg|UILayout|WidgetsOrder|Monitor12Lead|WaveOrder|";
        path += ECGSymbol::convert(ecgParam.get12LDisplayFormat());
        break;
    case UFACE_MONITOR_OXYCRG:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorOxyCRG|WaveOrder";
        break;
    case UFACE_MONITOR_TREND:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorTrend|WaveOrder";
        break;
    case UFACE_MONITOR_BIGFONT:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorBigFont|WaveOrder";
        break;
    case UFACE_MONITOR_CUSTOM:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorCustom|WaveOrder";
        break;
    default:
        break;
    }

    if (!path.isEmpty())
    {
        QString widgets;
        systemConfig.getStrValue(path, widgets);
        waveformNames = widgets.split(",");
    }
}

bool WindowManager::isLastWaveForm(WaveWidget *w)
{
    int count = _waveformBox->count();
    for (int i = count - 1; i >= 0; i--)
    {
        WaveWidget *lastW = qobject_cast<WaveWidget *>(_waveformBox->itemAt(i)->widget());
        if (!lastW)
        {
            continue;
        }

        return lastW == w;
    }

    return false;
}

WaveWidget *WindowManager::getWaveWidget(int id)
{
    int count = _waveformBox->count();
    for (int i = count - 1; i >= 0; i--)
    {
        WaveWidget *wave = qobject_cast<WaveWidget *>(_waveformBox->itemAt(i)->widget());

        if (wave && wave->getID() == id)
        {
            if (wave->isVisible())
            {
                return wave;
            }
            else
            {
                return NULL;
            }
        }
    }

    return NULL;
}

/***************************************************************************************************
 * 功能：设置当前显示的波形窗体内容。
 **************************************************************************************************/
void WindowManager::_setCurrentWaveforms(const QStringList &waveformNames)
{
    QString path;
    switch (_currenUserFaceType)
    {
    case UFACE_MONITOR_STANDARD:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorStandard|WaveOrder";
        break;
    case UFACE_MONITOR_12LEAD:
        path = "PrimaryCfg|UILayout|WidgetsOrder|Monitor12Lead|WaveOrder|";
        path += ECGSymbol::convert(ecgParam.get12LDisplayFormat());
        break;
    case UFACE_MONITOR_OXYCRG:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorOxyCRG|WaveOrder";
        break;
    case UFACE_MONITOR_TREND:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorTrend|WaveOrder";
        break;
    case UFACE_MONITOR_BIGFONT:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorBigFont|WaveOrder";
        break;
    case UFACE_MONITOR_CUSTOM:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorCustom|WaveOrder";
        break;
    default:
        break;
    }

    if (!path.isEmpty())
    {
        QString widgets = waveformNames.join(",");
        systemConfig.setStrValue(path, widgets);
    }
}

/***************************************************************************************************
 * 功能： 获取当前显示的趋势窗体内容。
 **************************************************************************************************/
void WindowManager::_getCurrentDisplayTrendWindow(QStringList &names)
{
    QString path;
    switch (_currenUserFaceType)
    {
    case UFACE_MONITOR_STANDARD:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorStandard|TrendOrder";
        break;
    case UFACE_MONITOR_12LEAD:
        path = "PrimaryCfg|UILayout|WidgetsOrder|Monitor12Lead|TrendOrder";
        break;
    case UFACE_MONITOR_OXYCRG:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorOxyCRG|TrendOrder";
        break;
    case UFACE_MONITOR_TREND:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorTrend|TrendOrder";
        break;
    case UFACE_MONITOR_BIGFONT:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorBigFont|TrendOrder";
        break;
    case UFACE_MONITOR_CUSTOM:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorCustom|TrendOrder";
        break;
    default:
        break;
    }

    if (!path.isEmpty())
    {
        QString widgets;
        systemConfig.getStrValue(path, widgets);
        names = widgets.split(",");

//        if (!respParam.isEnabled())
//        {
//            respParam.getTrendWindow(widgets);
//            names.removeOne(widgets);
//        }

//        if (!co2Param.isEnabled())
//        {
//            co2Param.getTrendWindow(widgets);
//            names.removeOne(widgets);
//        }
    }
}

/***************************************************************************************************
 * 功能： 设置当前显示的趋势窗体内容。
 **************************************************************************************************/
void WindowManager::_setCurrentTrendWindow(const QStringList &trendNames)
{
    QString path;
    switch (_currenUserFaceType)
    {
    case UFACE_MONITOR_STANDARD:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorStandard|TrendOrder";
        break;
    case UFACE_MONITOR_12LEAD:
        path = "PrimaryCfg|UILayout|WidgetsOrder|Monitor12Lead|TrendOrder";
        break;
    case UFACE_MONITOR_OXYCRG:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorOxyCRG|TrendOrder";
        break;
    case UFACE_MONITOR_TREND:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorTrend|TrendOrder";
        break;
    case UFACE_MONITOR_BIGFONT:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorBigFont|TrendOrder";
        break;
    case UFACE_MONITOR_CUSTOM:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorCustom|TrendOrder";
        break;
    default:
        break;
    }

    if (!path.isEmpty())
    {
        QString widgets = trendNames.join(",");
        systemConfig.setStrValue(path, widgets);
    }
}

/***************************************************************************************************
 * 功能： 获取当前显示的波形区的趋势窗体内容。
 **************************************************************************************************/
void WindowManager::_getDisplayedWaveTrendWidget(QStringList &names)
{
    QString path;
    switch (_currenUserFaceType)
    {
    case UFACE_MONITOR_STANDARD:
    case UFACE_MONITOR_12LEAD:
    case UFACE_MONITOR_OXYCRG:
    case UFACE_MONITOR_TREND:
    case UFACE_MONITOR_BIGFONT:
    case UFACE_MONITOR_CUSTOM:
        path = "PrimaryCfg|UILayout|WidgetsOrder|WaveTrendOrder";
        break;
    default:
        break;
    }

    if (!path.isEmpty())
    {
        QString widgets;
        systemConfig.getStrValue(path, widgets);
        names = widgets.split(",");
    }
}

/***************************************************************************************************
 * 功能： 根据名称获取波形窗体。
 **************************************************************************************************/
void WindowManager::_focusWaveformWidget(const QString &name)
{
    QMap<QString, IWidget *>::Iterator it = _waveformMap.find(name);
    if (it == _waveformMap.end())
    {
        return;
    }

    IWidget *widget = dynamic_cast<IWidget *>(it.value());
    if (NULL != widget)
    {
        if (!widget->isVisible())
        {
            widget->setVisible(true);
        }
        widget->setFocus();
    }
}

/***************************************************************************************************
 * 功能： 获取当前显示的波形。
 **************************************************************************************************/
void WindowManager::_getDisplayedWaveform(QList<WaveWidget *> &widgets)
{
    widgets.clear();

    QString path;
    switch (_currenUserFaceType)
    {
    case UFACE_MONITOR_STANDARD:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorStandard|WaveOrder";
        break;
    case UFACE_MONITOR_12LEAD:
        path = "PrimaryCfg|UILayout|WidgetsOrder|Monitor12Lead|WaveOrder|";
        path += ECGSymbol::convert(ecgParam.get12LDisplayFormat());
        break;
    case UFACE_MONITOR_OXYCRG:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorOxyCRG|WaveOrder";
        break;
    case UFACE_MONITOR_TREND:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorTrend|WaveOrder";
        break;
    case UFACE_MONITOR_BIGFONT:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorBigFont|WaveOrder";
        break;
    case UFACE_MONITOR_CUSTOM:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorCustom|WaveOrder";
        break;
    default:
        break;
    }

    QList<IWidget *> waveWidgetList;
    _getWaveFormBoxWidget(waveWidgetList);
    if (waveWidgetList.isEmpty())
    {
        return;
    }

    QString widgetNames;
    systemConfig.getStrValue(path, widgetNames);
    QStringList nodeWidgets = widgetNames.split(",");

    QMap<QString, IWidget *>::Iterator it;
    for (int i = 0; i < nodeWidgets.size(); i++)
    {
        it = _waveformMap.find(nodeWidgets[i]);
        if (it != _waveformMap.end())
        {
            WaveWidget *widget = qobject_cast<WaveWidget *>(it.value());
            if (NULL != widget && (-1 != waveWidgetList.indexOf(it.value())))
            {
                widgets += widget;
            }
        }
    }
}

/***************************************************************************************************
 * 功能： 获取当前显示的波形。
 **************************************************************************************************/
void WindowManager::_getDisplayedWaveWidget(QList<IWidget *> &widgets)
{
    widgets.clear();

    QString path;
    switch (_currenUserFaceType)
    {
    case UFACE_MONITOR_STANDARD:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorStandard|WaveOrder";
        break;
    case UFACE_MONITOR_12LEAD:
        path = "PrimaryCfg|UILayout|WidgetsOrder|Monitor12Lead|WaveOrder|";
        path += ECGSymbol::convert(ecgParam.get12LDisplayFormat());
        break;
    case UFACE_MONITOR_OXYCRG:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorOxyCRG|WaveOrder";
        break;
    case UFACE_MONITOR_TREND:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorTrend|WaveOrder";
        break;
    case UFACE_MONITOR_BIGFONT:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorBigFont|WaveOrder";
        break;
    case UFACE_MONITOR_CUSTOM:
        path = "PrimaryCfg|UILayout|WidgetsOrder|MonitorCustom|WaveOrder";
        break;
    default:
        break;
    }

    QList<IWidget *> waveWidgetList;
    _getWaveFormBoxWidget(waveWidgetList);
    if (waveWidgetList.isEmpty())
    {
        return;
    }

    QString widgetNames;
    systemConfig.getStrValue(path, widgetNames);
    QStringList nodeWidgets = widgetNames.split(",");
    int size = nodeWidgets.size();
    QMap<QString, IWidget *>::Iterator it;
    for (int i = 0; i < size; i++)
    {
        it = _waveformMap.find(nodeWidgets[i]);
        if (it != _waveformMap.end())
        {
            WaveWidget *waveWidget = qobject_cast<WaveWidget *>(it.value());
            if (NULL != waveWidget && (-1 != waveWidgetList.indexOf(it.value())))   // wave widget
            {
                widgets += it.value();
            }
            else if (NULL == waveWidget && NULL != it.value())   // dash board
            {
                widgets += it.value();
            }
        }
    }
}

/***************************************************************************************************
 * 功能：设置聚焦顺序。
 **************************************************************************************************/
void WindowManager::setFocusOrder(void)
{
    QList<QWidget *> allWidgets;

    // 获取当前显示的波形。
    QList<IWidget *> waveWidgets;
    waveWidgets.clear();
    _getDisplayedWaveWidget(waveWidgets);

    // 获取当前显示的趋势窗体。
    QStringList trendWidgetNames;
    _getCurrentDisplayTrendWindow(trendWidgetNames);
    QList<QWidget *> trendWidgets;
    for (int i = 0; i < trendWidgetNames.size(); i++)
    {
        trendWidgets += getWidget(trendWidgetNames[i]);
    }

    QStringList waveTrendWidgetNames;
    _getDisplayedWaveTrendWidget(waveTrendWidgetNames);
    QList<QWidget *> waveTrendWidgets;
    for (int i = 0; i < waveTrendWidgetNames.size(); i++)
    {
        waveTrendWidgets += getWidget(waveTrendWidgetNames[i]);
    }

    // 先获取固定的QWidget。
    QString name;
    systemConfig.getStrValue("PrimaryCfg|UILayout|WidgetsOrder|FocusOrder", name);
    QStringList names = name.split(",");
    QList<QWidget *> subList;
    for (int i = 0; i < names.size(); i++)
    {
        if (names[i] == "WaveGroup")
        {
            for (int i = (waveTrendWidgets.size() - 1); i >= 0; i--)
            {
                allWidgets += waveTrendWidgets[i];
            }

            int waveWidgetSize = waveWidgets.size() - 1;
            for (int i = waveWidgetSize - 1; i >= 0; --i)
            {
                subList.clear();
                waveWidgets[i]->getSubFocusWidget(subList);
                if (!subList.isEmpty())
                {
                    allWidgets.append(subList);
                }
            }
        }
        else if (names[i] == "TrendGroup")
        {
            for (int i = 0; i < trendWidgets.size(); i++)
            {
                allWidgets += trendWidgets[i];
            }
        }
        else if (names[i] == "SystemStatusBarWidget")
        {
            IWidget *w = getWidget(names[i]);
            if (NULL != w)
            {
                subList.clear();
                w->getSubFocusWidget(subList);
                if (!subList.isEmpty())
                {
                    allWidgets.append(subList);
                }
            }
        }
        else if (names[i] == "SoftKeyManager")
        {
            IWidget *w = getWidget(names[i]);
            if (NULL != w)
            {
                subList.clear();
                w->getSubFocusWidget(subList);
                if (!subList.isEmpty())
                {
                    allWidgets.append(subList);
                }
            }
        }
        else
        {
            allWidgets += getWidget(names[i]);
        }
    }

    // 开始排列聚焦顺序。
    for (int i = 0; i < allWidgets.size() - 1; i++)
    {
        setTabOrder(allWidgets[i], allWidgets[i + 1]);
    }
}

/***************************************************************************************************
 * 功能：重画波形。
 **************************************************************************************************/
void WindowManager::resetWave(void)
{
    QMap<QString, IWidget *>::iterator it = _waveformMap.begin();
    for (; it != _waveformMap.end(); ++it)
    {
        if (NULL != it.value())
        {
            it.value()->resetWaveWidget();
        }
    }
}

/***************************************************************************************************
 * 功能：生成布局容器。
 **************************************************************************************************/
void WindowManager::_newLayoutStyle(void)
{
    _topBarRow = new QHBoxLayout();
    _topBarRow->setMargin(0);
    _topBarRow->setSpacing(3);

    _alarmRow = new QHBoxLayout();
    _alarmRow->setMargin(0);
    _alarmRow->setSpacing(3);

    // 构造波形易变区。
    _waveformBox = new QVBoxLayout();
    _waveformBox->setMargin(0);
    _waveformBox->setSpacing(0);
    _12LeadBox = new QGridLayout();
    _12LeadBox->setContentsMargins(2, 0, 2, 0);
    _12LeadBox->setHorizontalSpacing(5);

    // 构造波形可变区窗体。
    _paramLayout = new QHBoxLayout();
    _paramLayout->setMargin(0);
    _paramLayout->setSpacing(0);

    _volatileLayout = new QVBoxLayout();
    _volatileLayout->setMargin(0);
    _volatileLayout->setSpacing(0);

    _trendRowLayout = new QVBoxLayout();
    _trendRowLayout->setMargin(0);
    _trendRowLayout->setSpacing(0);

    _paramBox = new QGridLayout();
    _paramBox->setContentsMargins(2, 0, 2, 0);
    _paramBox->setHorizontalSpacing(5);

    _softkeyRow = new QHBoxLayout();
    _softkeyRow->setMargin(0);
    _softkeyRow->setSpacing(0);

    // 界面顶层布局器。
    QStringList factors;
    factors = systemConfig.getChildNodeNameList("PrimaryCfg|UILayout|WidgetsOrder|ScreenVLayoutStretch");
    if (factors.size() < 3)
    {
        debug("size of ScreenVLayoutStretch is wrong \n");
        return;
    }


    _mainLayout = new QVBoxLayout();
    _mainLayout->setMargin(0);
    _mainLayout->setSpacing(0);

    int index = 0;
    systemConfig.getNumValue("PrimaryCfg|UILayout|WidgetsOrder|ScreenVLayoutStretch|topBarRow", index);
    _mainLayout->addLayout(_topBarRow, index);

//    systemConfig.getNumValue("PrimaryCfg|UILayout|WidgetsOrder|ScreenVLayoutStretch|alarmRow", index);
//    _mainLayout->addLayout(_alarmRow, index);

    systemConfig.getNumValue("PrimaryCfg|UILayout|WidgetsOrder|ScreenVLayoutStretch|paramLayout", index);
    _mainLayout->addLayout(_paramLayout, index);

    systemConfig.getNumValue("PrimaryCfg|UILayout|WidgetsOrder|ScreenVLayoutStretch|softkeyRow", index);
    _mainLayout->addLayout(_softkeyRow, index);

    // 设置布局。
    setLayout(_mainLayout);
}

void WindowManager::_paramLayoutStyle(UserFaceType type)
{
    // 布局可变窗体。
    _trendLayout(type);
    _doVolatileLayout(type);
}

/***************************************************************************************************
 * 功能：布局固定区域。
 **************************************************************************************************/
void WindowManager::_fixedLayout(void)
{
    if (_doesFixedLayout)
    {
        return;
    }
    _doesFixedLayout = true;

    QMap<QString, IWidget *>::iterator it;
    QString prefix;

//    // 顶部信息栏。
//    QHBoxLayout *hLayoutTopBarRow = new QHBoxLayout();
//    hLayoutTopBarRow->setMargin(0);
//    hLayoutTopBarRow->setSpacing(0);

//    QStringList topBarRow;
//    prefix = "PrimaryCfg|UILayout|WidgetsOrder|topBarRowOrder";
//    topBarRow = systemConfig.getChildNodeNameList(prefix);

//    if (topBarRow.size() > 0)
//    {
//        for (int i = 0; i < topBarRow.size(); i++)
//        {
//            it = _winMap.find(topBarRow[i]);
//            if (it == _winMap.end())
//            {
//                continue;
//            }

//            w = it.value();
//            w->setParent(this);            // 设置父窗体。
//            w->setVisible(true);           // 可见。
//            QString string = prefix + "|" + topBarRow[i];
//            int index = 1;
//            systemConfig.getNumValue(string, index);
//            hLayoutTopBarRow->addWidget(w, index);
//        }
//    }
//    else
//    {
//        debug("topBarRow is null \n");
//        return;
//    }

    // 报警信息栏。
//    QHBoxLayout *hLayoutAlarmRow = new QHBoxLayout();
//    hLayoutAlarmRow->setMargin(0);
//    hLayoutAlarmRow->setSpacing(0);

//    QStringList alarmRow;
//    prefix = "PrimaryCfg|UILayout|WidgetsOrder|alarmRowOrder";
//    alarmRow = systemConfig.getChildNodeNameList(prefix);
//    if (alarmRow.size() > 0)
//    {
//        IWidget *w = NULL;
//        for (int i = 0; i < alarmRow.size(); i++)
//        {
//            it = _winMap.find(alarmRow[i]);
//            if (it == _winMap.end())
//            {
//                continue;
//            }

//            w = it.value();
//            w->setParent(this); // 设置父窗体
//            w->setVisible(true);
//            QString string = prefix + "|" + alarmRow[i];
//            int index = 1;
//            systemConfig.getNumValue(string, index);
//            hLayoutAlarmRow->addWidget(w, index);
//        }
//    }
//    else
//    {
//        debug("topBarRow is null \n");
//        return;
//    }
//    _topBarRow->addLayout(hLayoutTopBarRow);
    _topBarRow->addWidget(&topBarWidget);
//    _alarmRow->addLayout(hLayoutAlarmRow);

    // 软按键区。
    QHBoxLayout *softkeyLayout = new QHBoxLayout;
    softkeyLayout->setSpacing(0);
    softkeyLayout->setMargin(0);
    QStringList softkeyRow;
    prefix = "PrimaryCfg|UILayout|WidgetsOrder|softkeyAreaRowOrder";
    softkeyRow = systemConfig.getChildNodeNameList(prefix);
    if (softkeyRow.size() > 0)
    {
        IWidget *w = NULL;
        for (int i = 0; i < softkeyRow.size(); i++)
        {
            it = _winMap.find(softkeyRow[i]);
            if (it == _winMap.end())
            {
                continue;
            }
            w = it.value();
            w->setParent(this); // 设置父窗体
            w->setVisible(true);
            QString string = prefix + "|" + softkeyRow[i];
            int index = 1;
            systemConfig.getNumValue(string, index);
            softkeyLayout->addWidget(w, index);
        }
    }

    _softkeyRow->addWidget(&softkeyManager);
    _softkeyRow->addLayout(softkeyLayout);
    softkeyManager.setParent(this);
    softkeyManager.setVisible(true);
}

/***************************************************************************************************
 * 功能：布局趋势窗体。
 **************************************************************************************************/
void WindowManager::_trendLayout(UserFaceType type)
{
    // 移除_trendRowLayout之前的窗体。
    int trendcount = _trendRowLayout->count();
    for (int i = 0; i < trendcount; i++)
    {
        QLayoutItem *hlayoutitem = _trendRowLayout->takeAt(0);
        QBoxLayout *lable = qobject_cast<QBoxLayout *>(hlayoutitem->layout());
        if (lable != NULL)
        {
            int lablecount = lable->count();
            for (int j = 0; j < lablecount; j++)
            {
                QLayoutItem *item = lable->takeAt(0);
                IWidget *widget = qobject_cast<IWidget *>(item->widget());
                if (widget != NULL)
                {
                    widget->setVisible(false);
                    widget->setParent(NULL);
                }
            }
        }
    }

    // 移除_paramBox之前的窗体。
    trendcount = _paramBox->count();
    for (int i = 0; i < trendcount; i++)
    {
        QLayoutItem *hlayoutitem = _paramBox->takeAt(0);
        QBoxLayout *lable = qobject_cast<QBoxLayout *>(hlayoutitem->layout());
        if (lable != NULL)
        {
            int lablecount = lable->count();
            for (int j = 0; j < lablecount; j++)
            {
                QLayoutItem *item = lable->takeAt(0);
                IWidget *widget = qobject_cast<IWidget *>(item->widget());
                if (widget != NULL)
                {
                    widget->setVisible(false);
                    widget->setParent(NULL);
                }
            }
        }
    }



    if (type == UFACE_MONITOR_BIGFONT)
    {
        return;
    }

    QMap<QString, IWidget *>::iterator it;
    QStringList nodeWidgets;
    QHBoxLayout *hlayout = new QHBoxLayout();
    IWidget *w = NULL;

    //获取趋势窗体内容。
    _getCurrentDisplayTrendWindow(nodeWidgets);
    for (int i = 0; i < nodeWidgets.size(); i++)
    {
        hlayout = new QHBoxLayout();
        hlayout->setMargin(0);
        hlayout->setSpacing(0);

        it = _winMap.find(nodeWidgets[i]);
        if (it == _winMap.end())
        {
            continue;
        }
        w = it.value();
        w->setParent(this); // 设置父窗体
        hlayout->addWidget(w);

        if (i < nodeWidgets.size() - 1)
        {
            i++;
            it = _winMap.find(nodeWidgets[i]);
            if (it == _winMap.end())
            {
                _trendRowLayout->addLayout(hlayout, 1);
                continue;
            }
            //相同的情况
            if (w == it.value())
            {
                _trendRowLayout->addLayout(hlayout, 1);
                continue;
            }
            w = it.value();
            w->setParent(this); // 设置父窗体
            hlayout->addWidget(w);
        }

        _trendRowLayout->addLayout(hlayout, 1);
    }

    for (int i = 0; i < nodeWidgets.size(); i++)
    {
        it = _winMap.find(nodeWidgets[i]);
        if (it == _winMap.end())
        {
            continue;
        }

        w = it.value();
        w -> setVisible(true);
    }
}

/***************************************************************************************************
 * 功能：布局监护普通界面。
 **************************************************************************************************/
void WindowManager::_volatileStandardLayout(void)
{
    QStringList nodeWidgets;
    getCurrentWaveforms(nodeWidgets);

    _execVolitileLayout(nodeWidgets);
    _setCurrentWaveforms(nodeWidgets);

    _volatileLayout->addLayout(_waveformBox);
}

/***************************************************************************************************
 * 功能：布局12 Lead界面。
 **************************************************************************************************/
void WindowManager::_volatile12LeadLayout(void)
{
    QStringList nodeWidgets;
    getCurrentWaveforms(nodeWidgets);

    QMap<QString, IWidget *>::iterator it;
    IWidget *w = NULL;

    // 布局
    for (int i = 0; i < nodeWidgets.size(); i++)
    {
        it = _winMap.find(nodeWidgets[i]);
        if (it == _winMap.end())
        {
            continue;
        }

        _waveformMap.insert(it.key(), it.value());
        w = it.value();
        w->setParent(this);
        w->setVisible(true);

        int row = (i / 2);
        int col = i % 2;
        _12LeadBox->addWidget(w, row, col);
        _12LeadBox->setRowStretch(row, 1);
    }

    _volatileLayout->addLayout(_12LeadBox);
}

void WindowManager::_volatileOxyCRGLayout()
{
    QStringList nodeWidgets;
    getCurrentWaveforms(nodeWidgets);

    _execVolitileLayout(nodeWidgets);
    _setCurrentWaveforms(nodeWidgets);
    _volatileLayout->addLayout(_waveformBox);
}

void WindowManager::_volatileTrendLayout()
{
    QStringList nodeWidgets;
    getCurrentWaveforms(nodeWidgets);

    _execVolitileLayout(nodeWidgets);
    _setCurrentWaveforms(nodeWidgets);
    _volatileLayout->addLayout(_waveformBox);
}

void WindowManager::_volatileBigFontLayout()
{
    QMap<QString, IWidget *>::iterator trendItem;
    QMap<QString, IWidget *>::iterator waveItem;
    QStringList nodeWidgets;
    QStringList tmpWidgets;
    IWidget *trendWidgetItem = NULL;
    IWidget *waveWidgetItem = NULL;

    //获取趋势窗体内容。
    _getCurrentDisplayTrendWindow(nodeWidgets);
    getCurrentWaveforms(tmpWidgets);
    if (nodeWidgets.size() != tmpWidgets.size())
    {
        return;
    }

    for (int i = 0; i < nodeWidgets.size(); i++)
    {
        QVBoxLayout *vlayout;
        vlayout = new QVBoxLayout();
        vlayout->setMargin(0);
        vlayout->setSpacing(0);


        // 添加trend窗体
        trendItem = _winMap.find(nodeWidgets[i]);
        if (trendItem == _winMap.end())
        {
            continue;
        }
        trendWidgetItem = trendItem.value();
        trendWidgetItem->setParent(this); // 设置父窗体
        trendWidgetItem->setVisible(true);
        vlayout->addWidget(trendWidgetItem, 2);

        // 添加波形
        if (tmpWidgets[i] != "NULL")
        {
            // 添加心电的计算导联
            if (nodeWidgets[i] == "ECGTrendWidget")
            {
                if (tmpWidgets[i] != ecgParam.getCalcLeadWaveformName())
                {
                    tmpWidgets[i] = ecgParam.getCalcLeadWaveformName();
                    _setCurrentWaveforms(tmpWidgets);
                }
            }

            waveItem = _winMap.find(tmpWidgets[i]);

            if (waveItem == _winMap.end())
            {
                continue;
            }

            WaveWidget *waveWidget = dynamic_cast<WaveWidget *>(waveItem.value());
            if (NULL != waveWidget)
            {
                if (!waveWidget->waveEnable())
                {
                    tmpWidgets.removeOne(tmpWidgets[i]);
                    continue;
                }
            }

            if (_waveformMap.end() == _waveformMap.find(waveItem.key()))
            {
                _waveformMap.insert(waveItem.key(), waveItem.value());

                waveWidgetItem = waveItem.value();
                waveWidgetItem->setParent(this);
                waveWidgetItem -> setVisible(true);
                vlayout->addWidget(waveWidgetItem, 1);
            }
        }

        int row = (i / 2);
        int col = i % 2;
        _bigformMap.insert(trendItem.key(), vlayout);
        _paramBox->addLayout(vlayout, row, col);
        _paramBox->setRowStretch(row, 1);
    }
    _paramLayout->addLayout(_paramBox);
}

void WindowManager::_volatileCustomLayout()
{
    QStringList nodeWidgets;
    getCurrentWaveforms(nodeWidgets);

    _execVolitileLayout(nodeWidgets);
    _setCurrentWaveforms(nodeWidgets);
    _volatileLayout->addLayout(_waveformBox);
}

/***************************************************************************************************
 * 功能：获取波形布局器中的控件。
 **************************************************************************************************/
void WindowManager::_getWaveFormBoxWidget(QList<IWidget *> &list)
{
    list.clear();
    int count = _waveformBox->count();
    for (int i = 0; i < count; ++i)
    {
        IWidget *w = qobject_cast<IWidget *>(_waveformBox->itemAt(i)->widget());
        if (NULL != w)
        {
            list.append(w);
        }
    }
}

/***************************************************************************************************
 * 功能：计算波形比例因子。
 **************************************************************************************************/
void WindowManager::_calcWaveFactor(const QStringList &waveForms, int &normalFactor,
                                    int &specialFactor, int &trendFactor)
{
    int count = waveForms.count();

    normalFactor = 1;
    specialFactor = 1;
    trendFactor = 1;
    // 调整比例使面板区占整个波形区域的1/3
    if (count == 2)
    {
        if (_currenUserFaceType == UFACE_MONITOR_OXYCRG)
        {
            normalFactor = 3;
            specialFactor = 3;
            trendFactor = 1;
        }
        else
        {
            normalFactor = 4;
            specialFactor = 1;
            trendFactor = 1;
        }
    }
    else if (count == 3)
    {
        if (_currenUserFaceType == UFACE_MONITOR_OXYCRG)
        {
            normalFactor = 1;
            specialFactor = 2;
            trendFactor = 1;
        }
        else if (_currenUserFaceType == UFACE_MONITOR_TREND)
        {
            normalFactor = 2;
            specialFactor = 2;
            trendFactor = 1;
        }
        else
        {
            normalFactor = 2;
            specialFactor = 1;
            trendFactor = 1;
        }
    }
    else if (count == 4)
    {
        if (_currenUserFaceType == UFACE_MONITOR_OXYCRG)
        {
            normalFactor = 1;
            specialFactor = 3;
            trendFactor = 1;
        }
        else if (_currenUserFaceType == UFACE_MONITOR_TREND)
        {
            normalFactor = 1;
            specialFactor = 2;
            trendFactor = 1;
        }
        else
        {
            normalFactor = 4;
            specialFactor = 1;
            trendFactor = 3;
        }
    }
}

/***************************************************************************************************
 * 功能：将nodeWidgets的控件布局到波形区。
 **************************************************************************************************/
void WindowManager::_execVolitileLayout(QStringList &nodeWidgets)
{
    QStringList tmpWidgets;
    tmpWidgets.clear();
    for (int i = 0; i < nodeWidgets.count(); ++i)
    {
        if (-1 == tmpWidgets.indexOf(nodeWidgets.at(i)))
        {
            tmpWidgets << nodeWidgets.at(i);
        }
    }

    nodeWidgets = tmpWidgets;
    IWidget *w = NULL;
    QMap<QString, IWidget *>::iterator it;
    QStringList currentWaveforms;
    for (int i = 0; i < tmpWidgets.size(); i++)
    {
        it = _winMap.find(tmpWidgets[i]);
        if (it == _winMap.end())
        {
            continue;
        }

        WaveWidget *waveWidget = dynamic_cast<WaveWidget *>(it.value());
        if (NULL != waveWidget)
        {
            if (!waveWidget->waveEnable())
            {
                nodeWidgets.removeOne(tmpWidgets[i]);
                continue;
            }
        }

        if (_waveformMap.end() == _waveformMap.find(it.key()))
        {
            _waveformMap.insert(it.key(), it.value());

            w = it.value();
            w->setParent(this);
            // w->setVisible(true);
            // 此处执行setVisible(true)会触发ECGWaveWidget的show执行_loadConfig方法，
            // 导致布局没布完就进行完界面上WaveWidgetLabel控件是否显示的判断。
            _waveformBox->addWidget(w, 1);
            currentWaveforms << tmpWidgets[i];
        }
    }

    int normalF, specialFactor, trendF;
    _calcWaveFactor(currentWaveforms, normalF, specialFactor, trendF);

    it = _waveformMap.begin();
    for (it = _waveformMap.begin(); it != _waveformMap.end(); ++it)
    {
        if (NULL != it.value())
        {
            it.value()->setVisible(true);
            QString name = it.value()->name();

            if (-1 != currentWaveforms.indexOf(name))
            {
                if (name == "WaveTrendWidget")
                {
                    _waveformBox->setStretchFactor(it.value(), trendF);
                }
                else if (name == "ShortTrendManager" || name == "OxyCRGWidget")
                {
                    _waveformBox->setStretchFactor(it.value(), specialFactor);
                }
                else
                {
                    if (name == "ShortTrendManager")
                    {
                        _waveformBox->setStretchFactor(it.value(), normalF + normalF);
                    }
                    else
                    {
                        _waveformBox->setStretchFactor(it.value(), normalF);
                    }
                }
            }
        }
    }
}

/***************************************************************************************************
 * 功能：布局易变区域，也即波形区。
 **************************************************************************************************/
void WindowManager::_doVolatileLayout(UserFaceType type)
{
    // 移除_waveLayout中之前的窗体。
    QMap<QString, IWidget *>::iterator it = _waveformMap.begin();
    for (; it != _waveformMap.end(); ++it)
    {
        it.value()->setVisible(false);
        it.value()->setParent(NULL);

        _waveformBox->removeWidget(it.value());
        _12LeadBox->removeWidget(it.value());
    }
    _waveformMap.clear();
    _bigformMap.clear();
    _volatileLayout->removeItem(_waveformBox);
    _volatileLayout->removeItem(_12LeadBox);

    _paramLayout->removeItem(_volatileLayout);
    _paramLayout->removeItem(_trendRowLayout);
    _paramLayout->removeItem(_paramBox);

    if (type != UFACE_MONITOR_BIGFONT)
    {
        _paramLayout->addLayout(_volatileLayout, _volatileLayoutStretch);
        _paramLayout->addLayout(_trendRowLayout, _trendRowLayoutStretch);
    }

    // 获取界面布局。
    if (type == UFACE_MONITOR_STANDARD)
    {
        _volatileStandardLayout();
    }
    else if (type == UFACE_MONITOR_12LEAD)
    {
        _volatile12LeadLayout();
    }
    else if (type == UFACE_MONITOR_OXYCRG)
    {
        _volatileOxyCRGLayout();
    }
    else if (type == UFACE_MONITOR_TREND)
    {
        _volatileTrendLayout();
    }
    else if (type == UFACE_MONITOR_BIGFONT)
    {
        _volatileBigFontLayout();
    }
    else if (type == UFACE_MONITOR_CUSTOM)
    {
        _volatileCustomLayout();
    }

    it = _waveformMap.begin();
    for (; it != _waveformMap.end(); ++it)
    {
        it.value()->resetWaveWidget();
    }
}

/***************************************************************************************************
 * 功能：设置用户界面。
 * 参数：
 *      type：指定的界面类型。
 **************************************************************************************************/
void WindowManager::_setUFaceType(UserFaceType type)
{
    // 设置TOPwaveform。
    if (type != UFACE_MONITOR_12LEAD && type != UFACE_MONITOR_BIGFONT)
    {
        windowManager.setTopWaveform(ecgParam.getCalcLeadWaveformName());
    }

    // 布局固定的窗体。
    _fixedLayout();

    //参数区布局
    _paramLayoutStyle(type);

    // 聚焦控制。
    setFocusOrder();

    // 改变软按键的内容。
    softkeyManager.setContent(softkeyManager.uFaceTypeToSoftKeyType(type));

    //界面模式显示修改
    IWidget* widget =  topBarWidget.findWidget("SystemModeBarWidget");
    if (widget)
    {

        SystemModeBarWidget *modeWidget = qobject_cast<SystemModeBarWidget *>(widget);
        modeWidget->setMode(_currenUserFaceType);
    }

    if (_demoWidget && _demoWidget->isVisible())
    {
        _demoWidget->raise();
    }
}

/***************************************************************************************************
 * 功能：设置自定义界面。
 **************************************************************************************************/
void WindowManager::_setCustomFaceType(void)
{
    if (_currenUserFaceType == UFACE_MONITOR_CUSTOM)
    {
        return;
    }
    _currenUserFaceType = UFACE_MONITOR_CUSTOM;
    //界面模式显示修改
    QMap<QString, IWidget *>::Iterator iter = _winMap.find("SystemModeBarWidget");
    if (iter != _winMap.end())
    {
        SystemModeBarWidget *widget = qobject_cast<SystemModeBarWidget *>(iter.value());
        widget->setMode(_currenUserFaceType);
    }
}

/***************************************************************************************************
 * 功能：设置Top Waveform。
 * 参数：
 *      type：指定的界面类型。
 **************************************************************************************************/
void WindowManager::setTopWaveform(const QString &waveformName, bool needRefresh)
{
    QStringList nodeWidgets;
    getCurrentWaveforms(nodeWidgets);

    // 如果top波形已经在界面上其他位置显示了，则交换两道波形的位置。
    int pos = nodeWidgets.indexOf(waveformName);
    if (pos != -1)
    {
        QString str = nodeWidgets[0];
        nodeWidgets[0] = nodeWidgets[pos];
        nodeWidgets[pos] = str;
    }
    else
    {
        if (0 == nodeWidgets.count())
        {
            nodeWidgets << waveformName;
        }
        else
        {
            int lead = ECG_LEAD_I;
            for (; lead <= ECG_LEAD_V6; ++lead)
            {
                if (ecgParam.getWaveWidgetName((ECGLead)lead) == nodeWidgets.first())
                {
                    nodeWidgets[0] = waveformName;
                    break;
                }
            }
        }
    }

    _setCurrentWaveforms(nodeWidgets);

    QMap<QString, IWidget *>::iterator it = _waveformMap.begin();
    for (; it != _waveformMap.end(); ++it)
    {
        it.value()->resetWaveWidget();
    }

    if (needRefresh)
    {
        _doVolatileLayout(_currenUserFaceType);
        _focusWaveformWidget(waveformName);
    }
}

/***************************************************************************************************
 * 功能： 排除给定的波形。
 * 参数：
 *      waveformName： 指定被排除的波形。
 **************************************************************************************************/
void WindowManager::setExcludeWaveforms(const QStringList &waveformName)
{
    if (waveformName.isEmpty())
    {
        return;
    }



    QStringList nodeWidgets;
    getCurrentWaveforms(nodeWidgets);

    for (int j = 0; j < waveformName.size(); j++)
    {
        nodeWidgets.removeOne(waveformName[j]);
    }

    // 去掉重复的
    QStringList tmpList;
    tmpList.clear();
    for (int index = 0; index < nodeWidgets.count(); ++index)
    {
        if (-1 == tmpList.indexOf(nodeWidgets.at(index)))
        {
            tmpList << nodeWidgets.at(index);
        }
    }

    _setCurrentWaveforms(tmpList);

    setFocusOrder();
}

/***************************************************************************************************
 * 功能： ecg改为3导联。
 * 参数：
 *      waveformName： 指定被排除的波形。
 **************************************************************************************************/
void WindowManager::changeToECGLead3(const QStringList &waveformName)
{
    if (waveformName.isEmpty())
    {
        return;
    }

    QStringList nodeWidgets;
    getCurrentWaveforms(nodeWidgets);

    for (int j = 0; j < waveformName.size(); j++)
    {
        nodeWidgets.removeOne(waveformName[j]);
        removeWaveform(waveformName[j], false);
    }

    // 去掉重复的
    QStringList tmpList;
    tmpList.clear();
    for (int index = 0; index < nodeWidgets.count(); ++index)
    {
        if (-1 == tmpList.indexOf(nodeWidgets.at(index)))
        {
            tmpList << nodeWidgets.at(index);
        }
    }

    _setCurrentWaveforms(tmpList);

    // 布局可变窗体。
    setFocusOrder();
}

/***************************************************************************************************
 * 功能：设置界面布局样式
 * 参数：
 *      type：指定的界面类型。
 **************************************************************************************************/
void WindowManager::setUFaceType(UserFaceType type)
{
    if (_currenUserFaceType == type)
    {
        return;
    }
    _currenUserFaceType = type;
    _setUFaceType(_currenUserFaceType);
}

/***************************************************************************************************
 * 功能：设置界面布局样式,轮询切换
 **************************************************************************************************/
void WindowManager::setUFaceType(void)
{
    int currenType = _currenUserFaceType;
    currenType++;
    if ((UserFaceType)currenType > UFACE_MONITOR_CUSTOM)
    {
        _currenUserFaceType = UFACE_MONITOR_STANDARD;
    }
    else
    {
        _currenUserFaceType = (UserFaceType)currenType;
    }
    _setUFaceType(_currenUserFaceType);
}

/***************************************************************************************************
 * 功能：获取菜单区域的Rect，实际上是波形区域。
 * 返回：rect对象。
 **************************************************************************************************/
QRect WindowManager::getMenuArea(void)
{
    QRect r = _volatileLayout->geometry();
    r.adjust(x(), y(), x(), y());
    return r;
}

/***************************************************************************************************
 * 功能： 获取弹出菜单宽度
 **************************************************************************************************/
int WindowManager::getPopMenuWidth()
{
    return _subMenuWidth;
}

/***************************************************************************************************
 * 功能： 获取弹出菜单高度
 **************************************************************************************************/
int WindowManager::getPopMenuHeight()
{
    return _subMenuHeight;
}

/***************************************************************************************************
 * 功能： 设置弹出菜单大小
 * 参数：
 *      w：宽度
 *      h：高度
 **************************************************************************************************/
void WindowManager::setPopMenuSize(int w, int h)
{
    int total = _volatileLayoutStretch + _trendRowLayoutStretch;
    _subMenuWidth = w * _volatileLayoutStretch / total - 10; // 微调宽度10个像素点。
    _subMenuHeight = (h - 40) * 3 / 4;
}

/***************************************************************************************************
 * 功能： 获取已显示的波形窗体名称。
 * 参数：
 *      waveformName：带回波形控件的名称。
 **************************************************************************************************/
void WindowManager::getDisplayedWaveform(QStringList &waveformName)
{
    waveformName.clear();

    QList<WaveWidget *> waves;
    _getDisplayedWaveform(waves);
    for (int i = 0; i < waves.size(); i++)
    {
        waveformName += waves[i]->name();
    }
}

/***************************************************************************************************
 * 功能： 获取已显示的波形窗体ID。
 * 参数：
 *      waveformID：带回波形控件的ID。
 **************************************************************************************************/
void WindowManager::getDisplayedWaveform(QList<int> &id)
{
    id.clear();

    QList<WaveWidget *> waves;
    _getDisplayedWaveform(waves);
    for (int i = 0; i < waves.size(); i++)
    {
        id += waves[i]->getID();
    }
}

/**
 * @brief WindowManager::getDisplayedWaveformLabels get the display waves labels
 * @param waveLabels string list of the wave labels
 */
void WindowManager::getDisplayedWaveformIDsAndLabels(QList<int> &id, QStringList &waveLabels)
{
    waveLabels.clear();
    QList<WaveWidget *> waves;
    _getDisplayedWaveform(waves);
    for (int i = 0; i < waves.size(); i++)
    {
        waveLabels += waves[i]->waveLabel();
        id.append(waves[i]->getID());
    }
}

/***************************************************************************************************
 * 功能： 获取已显示的趋势窗体名称。
 * 参数：
 *      trendName：带回波形控件的ID。
 **************************************************************************************************/
void WindowManager::getDisplayTrendWindow(QStringList &trendName)
{
    trendName.clear();
    _getCurrentDisplayTrendWindow(trendName);
}

/***************************************************************************************************
 * 功能：获取当前界面还能新增几道波形。。
 * 返回：剩余的波形道数。
 **************************************************************************************************/
int WindowManager::getLeftWaveformChannelNR(void)
{
    int currentChannel = _waveformMap.size();
    int totalChannel = currentChannel;

    switch (_currenUserFaceType)
    {
    case UFACE_MONITOR_STANDARD:
    case UFACE_MONITOR_CUSTOM:
        totalChannel = 5;
        break;
    case UFACE_MONITOR_OXYCRG:
    case UFACE_MONITOR_TREND:
        totalChannel = 4;
        break;

    default:
        break;
    }

    return ((totalChannel - currentChannel) < 0) ? 0 : (totalChannel - currentChannel);
}

/***************************************************************************************************
 * 功能：替换趋势窗体。
 * 参数：
 *      oldWin： 被替换的窗体。
 *      newWin： 新窗体。
 **************************************************************************************************/
void WindowManager::replaceTrendWindow(const QString &oldWin, const QString &newWin)
{
    if (oldWin == newWin)
    {
        return;
    }

    QMap<QString, IWidget *>::iterator it = _winMap.find(newWin);
    if (it == _winMap.end())
    {
        return;
    }

    // 移除之前的窗体。
    bool isFocus = false;
    int count = _trendRowLayout->count();
    int index = 0;
    for (int i = 1; i < count; i++)
    {
        QLayoutItem *item = _trendRowLayout->itemAt(i);
        IWidget *widget = dynamic_cast<IWidget *>(item->widget());
        if (widget != NULL)
        {
            if (widget->name() == oldWin)
            {
                isFocus = widget->hasFocus();
                index = i;
                widget->setParent(NULL);
                widget->setVisible(false);
                _trendRowLayout->removeWidget(widget);
                break;
            }
        }
    }

    //未找到替换的控件，重新插入
    if (0 == index)
    {
        insertTrendWindow(newWin);
        return;
    }

    IWidget *w = it.value();
    w->setParent(this); // 设置父窗体
    w->setVisible(true);
    _trendRowLayout->insertWidget(index, w, 1);
    if (isFocus)
    {
        w->setFocus();
    }

    // 更新聚焦顺序。
    setFocusOrder();
}

/***************************************************************************************************
 * 功能：替换趋势窗体。
 * 参数：
 *      oldWaveform：被替换的窗体。
 *      newWaveform：新窗体。
 **************************************************************************************************/
void WindowManager::replaceTrendWindow(const QStringList &oldWin, const QStringList &newWin)
{
    if (oldWin.size() != newWin.size())
    {
        return;
    }

    for (int i = 0; i < oldWin.size(); i++)
    {
        replaceTrendWindow(oldWin[i], newWin[i]);
    }
}

/***************************************************************************************************
 * 功能：替换波形。
 * 参数：
 *      oldWaveform：被替换的波形。
 *      newWaveform：新波形。
 *      order:设置焦点顺序
 **************************************************************************************************/
void WindowManager::replaceWaveform(const QString &oldWaveform, const QString &newWaveform,
                                    bool setFocus, bool order)
{
    QStringList currentWaveforms;
    getCurrentWaveforms(currentWaveforms);

    //将标准界面修改为自定义界面
    if (_currenUserFaceType == UFACE_MONITOR_STANDARD && ecgParam.getCalcLeadWaveformName() != newWaveform)
    {
        _setCustomFaceType();
    }

    int i = 0;
    for (; i < currentWaveforms.size(); i++)
    {
        if (currentWaveforms[i] == oldWaveform)
        {
            currentWaveforms[i] = newWaveform;
            _setCurrentWaveforms(currentWaveforms);
            break;
        }
    }

    if (i == currentWaveforms.size())
    {
        return;
    }

    QMap<QString, IWidget *>::iterator it = _winMap.find(newWaveform);
    if (it != _winMap.end())
    {
        it.value()->setVisible(true);
        it.value()->setParent(this);
        _waveformMap.insert(it.key(), it.value());
        _waveformBox->insertWidget(i + 1, it.value(), 1);
    }

    bool isFocus = false;
    it = _waveformMap.find(oldWaveform);
    if (it != _waveformMap.end())
    {
        WaveWidget *wave = qobject_cast<WaveWidget *>(it.value());
        if (NULL != wave)
        {
            isFocus = wave->isFocus();
        }
        it.value()->setVisible(false);
        it.value()->setParent(NULL);
        IWidget *w = it.value();
        _waveformMap.erase(it);
        _waveformBox->removeWidget(w);
    }

    int normalF, specialFactor, trendF;
    _calcWaveFactor(currentWaveforms, normalF, specialFactor, trendF);

    it = _waveformMap.begin();
    for (; it != _waveformMap.end(); ++it)
    {
        if (NULL != it.value())
        {
            QString name = it.value()->name();
            if (-1 != currentWaveforms.indexOf(name))
            {
                if (name == "WaveTrendWidget")
                {
                    _waveformBox->setStretchFactor(it.value(), trendF);
                }
                else if (name == "ShortTrendManager" || name == "OxyCRGWidget")
                {
                    _waveformBox->setStretchFactor(it.value(), specialFactor);
                }
                else
                {
                    if (name == "ShortTrendManager")
                    {
                        _waveformBox->setStretchFactor(it.value(), normalF + normalF);
                    }
                    else
                    {
                        _waveformBox->setStretchFactor(it.value(), normalF);
                    }
                }
            }
            else
            {
                continue;
            }
        }
        it.value()->resetWaveWidget();
    }

    if (isFocus || setFocus)
    {
        _focusWaveformWidget(newWaveform);
    }

    // 更新聚焦顺序。
    if (order)
    {
        setFocusOrder();
    }
}

/***************************************************************************************************
 * 功能：替换波形。
 * 参数：
 *      oldWaveform：被替换的波形。
 *      newWaveform：新波形。
 **************************************************************************************************/
void WindowManager::replaceWaveform(const QStringList &oldWaveform, const QStringList &newWaveform)
{
    if (oldWaveform.size() != newWaveform.size())
    {
        return;
    }

    for (int i = 0; i < oldWaveform.size(); i++)
    {
        replaceWaveform(oldWaveform[i], newWaveform[i]);
    }
}

/***************************************************************************************************
 * 功能：大字体界面替换。
 * 参数：
 *      oldWidget：被替换的窗体。
 *      newWidget：新窗体。
 **************************************************************************************************/
void WindowManager::replacebigWidgetform(const QString &oldWidget, const QString &newWidget,
        bool setFocus, bool order)
{
    QStringList currentWidget;
    QStringList currentTrend;
    _getCurrentDisplayTrendWindow(currentTrend);
    getCurrentWaveforms(currentWidget);

    int i = 0;
    for (; i < currentTrend.size(); i++)
    {
        if (currentTrend[i] == oldWidget)
        {
            currentTrend[i] = newWidget;
            _setCurrentTrendWindow(currentTrend);
            break;
        }
    }

    if (i == currentTrend.size())
    {
        return;
    }

    QMap<QString, QVBoxLayout *>::iterator bigform = _bigformMap.find(oldWidget);
    if (bigform != _bigformMap.end())
    {
        QBoxLayout *lable = qobject_cast<QBoxLayout *>(bigform.value()->layout());
        if (lable != NULL)
        {
            int lablecount = lable->count();
            for (int j = 0; j < lablecount; j++)
            {
                QLayoutItem *item = lable->takeAt(0);
                IWidget *widget = qobject_cast<IWidget *>(item->widget());
                if (widget != NULL)
                {
                    widget->setVisible(false);
                    widget->setParent(NULL);
                }
            }
        }
        QVBoxLayout *vl = bigform.value();
        _bigformMap.erase(bigform);
        _paramBox->removeItem(vl);
    }

    QMap<QString, IWidget *>::iterator trendMap = _winMap.find(newWidget);
    if (trendMap != _winMap.end())
    {
        QVBoxLayout *vlayout = new QVBoxLayout();
        vlayout->setMargin(0);
        vlayout->setSpacing(0);

        IWidget *trendWidget = trendMap.value();
        trendWidget->setVisible(true);
        trendWidget->setParent(this);
        vlayout->addWidget(trendWidget, 2);

        QList<IWidget *> waveWidgetList;
        QString waveWidgetName;
        if (_trendWave.end() != _trendWave.find(trendWidget))
        {
            waveWidgetList = _trendWave.values(trendWidget);
        }
        if (waveWidgetList.count() > 0)
        {
            IWidget *waveWidget = NULL;
            if (trendWidget->name() == "ECGTrendWidget")
            {
                waveWidgetName = ecgParam.getCalcLeadWaveformName();
                QMap<QString, IWidget *>::iterator waveMap = _winMap.find(waveWidgetName);
                if (waveMap != _winMap.end())
                {
                    waveWidget = waveMap.value();
                    waveWidget->setVisible(true);
                    waveWidget->setParent(this);

                    currentWidget[i] = waveWidgetName;
                    _setCurrentWaveforms(currentWidget);
                    vlayout->addWidget(waveWidget, 1);
                }
            }
            else
            {
                waveWidgetName = waveWidgetList.at(0)->name();
                QMap<QString, IWidget *>::iterator waveMap = _winMap.find(waveWidgetName);
                if (waveMap != _winMap.end())
                {
                    waveWidget = waveMap.value();
                    waveWidget->setVisible(true);
                    waveWidget->setParent(this);

                    currentWidget[i] = waveWidgetName;
                    _setCurrentWaveforms(currentWidget);
                    vlayout->addWidget(waveWidget, 1);
                }
            }
        }
        else
        {
            currentWidget[i] = "NULL";
            _setCurrentWaveforms(currentWidget);
        }

        int row = (i / 2);
        int col = i % 2;
        _bigformMap.insert(newWidget, vlayout);
        _paramBox->addLayout(vlayout, row, col);
        _paramBox->setRowStretch(row, 1);
    }

    resetWave();

    bool isFocus = false;


    if (isFocus || setFocus)
    {
        _focusWaveformWidget(newWidget);
    }

    // 更新聚焦顺序。
    if (order)
    {
        setFocusOrder();
    }
}

void WindowManager::replacebigWaveform(const QString &oldWidget, const QString &newWidget,
                                       bool setFocus, bool order)
{
    QStringList currentWidget;
    QStringList currentTrend;
    _getCurrentDisplayTrendWindow(currentTrend);
    getCurrentWaveforms(currentWidget);

    int i = 0;
    for (; i < currentTrend.size(); i++)
    {
        if (currentTrend[i] == "ECGTrendWidget")
        {
            break;
        }
    }

    if (i == currentTrend.size())
    {
        return;
    }

    QMap<QString, QVBoxLayout *>::iterator bigform = _bigformMap.find("ECGTrendWidget");
    if (bigform != _bigformMap.end())
    {
        QBoxLayout *lable = qobject_cast<QBoxLayout *>(bigform.value()->layout());
        if (lable != NULL)
        {
            int lablecount = lable->count();
            for (int j = 0; j < lablecount; j++)
            {
                QLayoutItem *item = lable->itemAt(j);
                IWidget *widget = qobject_cast<IWidget *>(item->widget());
                if (widget != NULL)
                {
                    if (widget->name() == oldWidget)
                    {
                        widget->setVisible(false);
                        widget->setParent(NULL);
                        lable->removeWidget(widget);

                        QMap<QString, IWidget *>::iterator waveMap = _winMap.find(newWidget);
                        if (waveMap != _winMap.end())
                        {
                            waveMap.value()->setVisible(true);
                            waveMap.value()->setParent(this);

                            currentWidget[i] = waveMap.value()->name();
                            _setCurrentWaveforms(currentWidget);
                            lable->addWidget(waveMap.value(), 1);
                        }
                        break;
                    }
                }
            }
        }
    }

    resetWave();

    bool isFocus = false;


    if (isFocus || setFocus)
    {
        _focusWaveformWidget(newWidget);
    }

    // 更新聚焦顺序。
    if (order)
    {
        setFocusOrder();
    }
}

/***************************************************************************************************
 * 功能：插入波形。
 * 参数：
 *      frontWaveform： 前面的波形。
 *      insertedWaveform： 插入的波形。
 **************************************************************************************************/
void WindowManager::insertWaveform(const QString &frontWaveform, const QString &insertedWaveform,
                                   bool setFocus, bool order)
{
    QStringList currentWaveforms;
    getCurrentWaveforms(currentWaveforms);

    if (-1 != currentWaveforms.indexOf(insertedWaveform))
    {
        return;
    }

    //将标准界面修改为自定义界面
    if (_currenUserFaceType == UFACE_MONITOR_STANDARD)
    {
        _setCustomFaceType();
    }

    int i = 0;
    int size = currentWaveforms.size();
    for (; i < size; i++)
    {
        if (currentWaveforms[i] == frontWaveform)
        {
            currentWaveforms.insert(i + 1, insertedWaveform);
            _setCurrentWaveforms(currentWaveforms);
            break;
        }
    }

    if (i == size)
    {
        debug("the front wave:%s is not find.\n", qPrintable(frontWaveform));
        return;
    }

    QMap<QString, IWidget *>::iterator it = _winMap.find(insertedWaveform);
    if (it != _winMap.end())
    {
        it.value()->setVisible(true);
        it.value()->setParent(this);
        _waveformMap.insert(it.key(), it.value());
        _waveformBox->insertWidget(i + 1, it.value(), 1);
    }

    int normalF, specialFactor, trendF;
    _calcWaveFactor(currentWaveforms, normalF, specialFactor, trendF);

    it = _waveformMap.begin();
    for (; it != _waveformMap.end(); ++it)
    {
        if (NULL != it.value())
        {
            QString name = it.value()->name();
            if (-1 != currentWaveforms.indexOf(name))
            {
                if (name == "WaveTrendWidget")
                {
                    _waveformBox->setStretchFactor(it.value(), trendF);
                }
                else if (name == "ShortTrendManager" || name == "OxyCRGWidget")
                {
                    _waveformBox->setStretchFactor(it.value(), specialFactor);
                }
                else
                {
                    if (name == "ShortTrendManager")
                    {
                        _waveformBox->setStretchFactor(it.value(), normalF + normalF);
                    }
                    else
                    {
                        _waveformBox->setStretchFactor(it.value(), normalF);
                    }
                }
            }
            else
            {
                continue;
            }
        }
        it.value()->resetWaveWidget();
    }

    // 更新聚焦顺序。
    if (order)
    {
        setFocusOrder();
    }

    bool isFocus = false;
    it = _waveformMap.find(frontWaveform);
    if (it != _waveformMap.end())
    {
        WaveWidget *wave = dynamic_cast<WaveWidget *>(it.value());
        if (NULL != wave)
        {
            isFocus = wave->isFocus();
        }
    }

    if (isFocus || setFocus)
    {
        _focusWaveformWidget(frontWaveform);
    }

    waveWidgetSelectMenu.close();
}

/***************************************************************************************************
 * 功能：插入趋势。
 * 参数：
 *      insertTrendWindow: 插入的趋势窗体。
 **************************************************************************************************/
void WindowManager::insertTrendWindow(const QString &insertTrendWindow)
{
    QStringList curDisplayTrend;
    _getCurrentDisplayTrendWindow(curDisplayTrend);
    int index = curDisplayTrend.indexOf(insertTrendWindow);
    if (-1 == index)
    {
        return;
    }

    IWidget *w = getWidget(insertTrendWindow);
    if (NULL == w)
    {
        return;
    }

    w->setParent(this); // 设置父窗体
    w->setVisible(true);
    _trendRowLayout->insertWidget(index + 1, w, 1);

    // 更新聚焦顺序。
    setFocusOrder();
    return;
}

/***************************************************************************************************
 * 功能：移除波形。
 * 参数：
 *      waveform：待移除波形的名称。
 **************************************************************************************************/
void WindowManager::removeWaveform(const QString &waveform, bool setFocus)
{
    QStringList currentWaveforms;
    getCurrentWaveforms(currentWaveforms);
    if (-1 == currentWaveforms.indexOf(waveform))
    {
        return;
    }

    //将标准界面修改为自定义界面
    if (_currenUserFaceType == UFACE_MONITOR_STANDARD)
    {
        _setCustomFaceType();
    }

    currentWaveforms.removeOne(waveform);
    _setCurrentWaveforms(currentWaveforms);

//    int count = currentWaveforms.size();
//    int normalFactor = 1;
//    //调整比例使面板区占整个波形区域的1/3
//    if (2 == count)
//    {
//        normalFactor = 2;
//    }

    int normalF, specialFactor, trendF;
    _calcWaveFactor(currentWaveforms, normalF, specialFactor, trendF);

    bool isFocus = false;
    QMap<QString, IWidget *>::iterator it = _waveformMap.begin();
    for (; it != _waveformMap.end();)
    {
        if (NULL != it.value())
        {
            QString name = it.value()->name();
            if (name == waveform)
            {
                WaveWidget *wave = dynamic_cast<WaveWidget *>(it.value());
                if (NULL != wave)
                {
                    isFocus = wave->isFocus();
                }

                it.value()->setVisible(false);
                it.value()->setParent(NULL);
                _waveformBox->removeWidget(it.value());
                it = _waveformMap.erase(it);
                continue;
            }

            if (-1 != currentWaveforms.indexOf(name))
            {
                if (name == "WaveTrendWidget")
                {
                    _waveformBox->setStretchFactor(it.value(), trendF);
                }
                else if (name == "ShortTrendManager" || name == "OxyCRGWidget")
                {
                    _waveformBox->setStretchFactor(it.value(), specialFactor);
                }
                else
                {
                    if (name == "ShortTrendManager")
                    {
                        _waveformBox->setStretchFactor(it.value(), normalF + normalF);
                    }
                    else
                    {
                        _waveformBox->setStretchFactor(it.value(), normalF);
                    }
                }
            }
        }
        it.value()->resetWaveWidget();
        ++it;
    }

    if (!currentWaveforms.isEmpty() && (isFocus || setFocus))
    {
        _focusWaveformWidget(currentWaveforms.first()); // 聚焦到第一个波形窗体。
    }

    // 更新聚焦顺序。
    setFocusOrder();

    waveWidgetSelectMenu.close();
}

/***************************************************************************************************
 * 功能：移除趋势。
 * 参数：
 *      removeTrendWindow: 插入的趋势窗体。
 **************************************************************************************************/
void WindowManager::removeTrendWindow(const QString &removeTrendWindow)
{
    bool isFocus = false;
    int count = _trendRowLayout->count();
    int i = 0;
    for (i = 1; i < count; i++)
    {
        QLayoutItem *item = _trendRowLayout->itemAt(i);
        IWidget *widget = dynamic_cast<IWidget *>(item->widget());
        if (widget != NULL)
        {
            if (removeTrendWindow == widget->name())
            {
                isFocus = widget->hasFocus();
                widget->setVisible(false);
                widget->setParent(NULL);
                _trendRowLayout->removeWidget(widget);
                break;
            }
        }
    }

    if (i == count)
    {
        return;
    }

    if (isFocus)
    {
        if (i < _trendRowLayout->count())
        {
            QLayoutItem *item = _trendRowLayout->itemAt(i);
            IWidget *widget = dynamic_cast<IWidget *>(item->widget());
            if (widget != NULL)
            {
                widget->setFocus();
            }
        }
    }

    // 更新聚焦顺序。
    setFocusOrder();
    return;
}

/***************************************************************************************************
 * 功能：注册窗体
 * 参数：
 *      win：待注册窗体的指针
 * 返回：
 *      true，注册成功；false，注册失败。
 **************************************************************************************************/
bool WindowManager::addWidget(IWidget *win, IWidget *trend)
{
    if (!win)
    {
        return false;
    }

    if (_winMap.find(win->name()) != _winMap.end())
    {
        debug("Win %s is already registered!\n", qPrintable(win->name()));
        return false;
    }

    _winMap.insert(win->name(), win);

    if (trend != NULL)
    {
        QMap<QString, IWidget *>::iterator it = _winMap.find(trend->name());
        if (it != _winMap.end())
        {
            _trendWave.insert(trend, win);
        }
    }

    return true;
}

/***************************************************************************************************
 * 功能：获取窗体
 * 参数：
 *      name：窗体名字。
 * 返回：
 *      窗体指针。
 **************************************************************************************************/
IWidget *WindowManager::getWidget(const QString &name)
{
    QMap<QString, IWidget *>::Iterator iter = _winMap.find(name);
    if (iter != _winMap.end())
    {
        return iter.value();
    }
    return NULL;
}

/***************************************************************************************************
 * 功能：构造函数
 **************************************************************************************************/
#if defined(Q_WS_QWS)
WindowManager::WindowManager() : QWidget(NULL, Qt::FramelessWindowHint), timer(NULL)
#else
WindowManager::WindowManager() : QWidget(), _demoWidget(NULL), timer(NULL)
#endif
{
    _demoWidget = NULL;

    _doesFixedLayout = false;
    _currenUserFaceType = UFACE_MONITOR_UNKNOW;

    // 设置调色
    QPalette p;
    p.setColor(QPalette::Background, Qt::black);
    p.setColor(QPalette::Foreground, Qt::white);
    setPalette(p);


//    _firstColumnFactor = 30;
//    _secondColumnFactor = 270;
//    _thirdColumnFactor = 55;//102;

    //波形区与趋势区的比列
    QStringList factors;
    factors = systemConfig.getChildNodeNameList("PrimaryCfg|UILayout|WidgetsOrder|ScreenHLayoutStretch");
    if (factors.size() < 2)
    {
        debug("size of ScreenHLayoutStretch is wrong \n");
        return;
    }
    int index = 1;
    systemConfig.getNumValue("PrimaryCfg|UILayout|WidgetsOrder|ScreenHLayoutStretch|volatileLayout", index);
    _volatileLayoutStretch = index;
    systemConfig.getNumValue("PrimaryCfg|UILayout|WidgetsOrder|ScreenHLayoutStretch|trendRowLayout", index);
    _trendRowLayoutStretch = index;

    _newLayoutStyle();
    setVisible(true);

    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(10 * 1000);    // 10s
    connect(timer, SIGNAL(timeout()), this, SLOT(closeAllWidows()));

    qApp->installEventFilter(this);
}

/***************************************************************************************************
 * 功能：析构函数
 **************************************************************************************************/
WindowManager::~WindowManager()
{
    // 清除窗体
    QList<IWidget *> winList = _winMap.values();
    foreach(IWidget *w, winList)
    {
        if (w && !w->parent())
        {
            delete w;   // WindowManager的所有子窗体会自动析构，因此不需要手动析构。
            w = NULL;
        }
    }

    _winMap.clear();
}

void WindowManager::showWindow(Window *w, ShowBehavior behaviors)
{
    if (w == NULL)
    {
        qWarning() << "Invalid window";
        return;
    }

    Window *top = topWindow();

    if (top == w)
    {
        if (behaviors & ShowBehaviorCloseIfVisiable)
        {
            // close will emit windowHide signal, so the window will remove from the stack after calling
            // the window hide slot
            w->close();
        }
        else
        {
            w->activateWindow();
        }
        return;
    }


    if (behaviors & ShowBehaviorCloseOthers)
    {
        while (!windowStacks.isEmpty())
        {
            Window *p = windowStacks.last();
            if (p)
            {
                p->close();
            }
        }
        // when got here, no any top window
        top = NULL;
    }

    if (top)
    {
        // will not enter here if the behaviro is close others
        if (behaviors & ShowBehaviorHideOthers)
        {
            // don't emit windowHide signal
            top->blockSignals(true);
            top->hide();
            top->blockSignals(false);
        }
        else
        {
            top->showMask(true);
        }
    }

    if (!(behaviors & ShowBehaviorNoAutoClose))
    {
        timer->start();
    }

    QPointer<Window> newP = w;
    windowStacks.append(newP);
    connect(w, SIGNAL(windowHide(Window *)), this, SLOT(onWindowHide(Window *)), Qt::DirectConnection);

    // move the proper position
    QRect r = _volatileLayout->geometry();
    if (r.width() < w->width())         // 如果波形区无完全显示窗口，则居中处理
    {
        QRect mainR = _mainLayout->geometry();
        QPoint globalCenter = mapToGlobal(mainR.center());
        mainR.moveTo(0, 0);
        w->move(globalCenter.x()-w->width()/2 , globalCenter.y()-w->height()/2);
    }
    else
    {
        QPoint globalTopLeft = mapToGlobal(r.topLeft());
        r.moveTo(0, 0);
        w->move(globalTopLeft + r.center() - w->rect().center());
    }

    if (behaviors & ShowBehaviorModal)
    {
        w->exec();
    }
    else
    {
        w->show();
        w->activateWindow();
    }
}

Window *WindowManager::topWindow()
{
    // find top window
    QPointer<Window> top;
    while (!windowStacks.isEmpty())
    {
        top = windowStacks.last();
        if (top)
        {
            break;
        }
        else
        {
            windowStacks.takeLast();
        }
    }
    return top.data();
}

void WindowManager::showDemoWidget(bool flag)
{
    if (_demoWidget == NULL)
    {
        // demo widget no exist yet, create one
        QLabel *l = new QLabel(trs("DEMO"), this);
        l->setAutoFillBackground(true);
        QPalette pal = l->palette();
        pal.setColor(QPalette::Window, Qt::gray);
        pal.setColor(QPalette::WindowText, Qt::white);
        l->setPalette(pal);
        l->setFont(fontManager.textFont(64));
        _demoWidget = l;
        _demoWidget->move(430, 100);
        _demoWidget->setFixedSize(l->sizeHint());
    }

    if (flag)
    {
        _demoWidget->show();
        _demoWidget->raise();
    }
    else
    {
        _demoWidget->lower();
        _demoWidget->hide();
    }
}

bool WindowManager::eventFilter(QObject *obj, QEvent *ev)
{
    Q_UNUSED(obj)
    if (windowStacks.isEmpty())
    {
        return false;
    }

    if ((ev->type() == QEvent::KeyPress) || (ev->type() == QEvent::MouseButtonPress))
    {
        // reactive the timer
        if (timer->isActive())
        {
            timer->start();
        }
    }

    return false;
}

void WindowManager::closeAllWidows()
{
    while (!windowStacks.isEmpty())
    {
        Window *p = windowStacks.last();
        if (p)
        {
            p->close();
        }
    }

    QWidget *activeWindow = NULL;
    while ((activeWindow = QApplication::activeWindow()))
    {
        if (activeWindow == this)
        {
            break;
        }
        activeWindow->close();
    }
    timer->stop();
}

void WindowManager::onWindowHide(Window *w)
{
    // find top window
    Window *top = topWindow();

    if (top == w)
    {
        // remove the window,
        windowStacks.takeLast();
        disconnect(w, SIGNAL(windowHide(Window *)), this, SLOT(onWindowHide(Window *)));

        // show the previous window
        top = topWindow();
        if (top)
        {
            top->showMask(false);
            top->show();
        }
        else
        {
            timer->stop();
        }
    }
    else if (windowStacks.isEmpty())
    {
        disconnect(w, SIGNAL(windowHide(Window *)), this, SLOT(onWindowHide(Window *)));
        timer->stop();
    }
}
