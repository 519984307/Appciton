/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/12
 **/

#pragma once
#include <QMap>
#include <QWidget>
#include <QList>
#include "WaveWidget.h"
#include "SystemDefine.h"
#include "MenuManager.h"

/***************************************************************************************************
 * 窗体管理器: 负责管理所有窗体布局
 **************************************************************************************************/
class IWidget;
class SoftKeyManager;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class Window;
class WindowManagerPrivate;
class WindowManager : public QWidget
{
    Q_OBJECT
public:
    enum ShowBehaviorFlags
    {
        ShowBehaviorNone = 0x00,            // no any special behavior
        ShowBehaviorCloseIfVisiable = 0x01, // close the window that request to show and remove it from the window stack
                                            // when the window is on the top of the window stack

        ShowBehaviorModal = 0x02,           // Show the window as modal window
        ShowBehaviorHideOthers = 0x04,      // show the window and hide others window, the other windows still in the window stack
        ShowBehaviorCloseOthers = 0x08,     // show the window and close others window, the other windows will remove from the window stack
        ShowBehaviorNoAutoClose = 0x10,     // don't automatically close this window when there isn't any user action
    };
    Q_DECLARE_FLAGS(ShowBehavior, ShowBehaviorFlags)

    static WindowManager &getInstance(void);
    ~WindowManager();


    /**
     * @brief showWindow show a window and push the window to the window stack
     * @param w the window need to show
     * @param behaviors the behavior need to perform when showing the window
     */
    void showWindow(Window *w, ShowBehavior behaviors = ShowBehaviorNone);

    /**
     * @brief topWindow get the top window
     * @return the top window or NULL if the not top window
     */
    Window *topWindow();

    /**
     * @brief showDemoWidget show or hide the demo widget
     * @param flag true if need to show the widget
     *             or false to hide the demo widget
     */
    void showDemoWidget(bool flag);

    /**
     * @brief eventFilter event filer to filter the user input event,
     *                    this filer is use to setup on the qApp and
     *                    reset the auto close timer when not any user action
     * @param obj
     * @param ev
     * @return always false
     */
    bool eventFilter(QObject *obj, QEvent *ev);

public slots:
    /**
     * @brief closeAllWidows close all the windows
     */
    void closeAllWidows();

    /**
     * @brief onWindowHide handle the windows hide signal in the window stack
     * @param w
     */
    void onWindowHide(Window *w);

public:
    // 注册窗体。
    bool addWidget(IWidget *win, IWidget *trend = NULL);

    // 获取指定窗体。
    IWidget *getWidget(const QString &name);

    // 设置Top Waveform。
    void setTopWaveform(const QString &waveformName, bool needRefresh = false);

    // 排除给定的波形。
    void setExcludeWaveforms(const QStringList &waveformName);

    // ecg改为3导联
    void changeToECGLead3(const QStringList &waveformName);

    // 设置界面类型。
    void setUFaceType(UserFaceType type);
    UserFaceType getUFaceType() const
    {
        return _currenUserFaceType;
    }

    // 获取菜单区域的Rect，实际上是波形区域。
    QRect getMenuArea(void);

    // 获取弹出菜单宽度
    int getPopMenuWidth();

    // 获取弹出菜单高度
    int getPopMenuHeight();

    // 设置弹出菜单大小
    void setPopMenuSize(int w, int h);

    // 获取已显示的波形窗体名称。
    void getDisplayedWaveformIDsAndLabels(QList<int> &id, QStringList &waveLabels);
    void getDisplayTrendWindow(QStringList &trendName);

    // 获取当前界面还能新增几道波形。
    int getLeftWaveformChannelNR(void);

    // 替换趋势窗体。
    void replaceTrendWindow(const QString &oldWin, const QString &newWin);
    void replaceTrendWindow(const QStringList &oldWin, const QStringList &newWin);

    // 替换波形。
    void replaceWaveform(const QString &oldWaveform, const QString &newWaveform,
                         bool setFocus = true, bool order = true);
    void replaceWaveform(const QStringList &oldWaveform, const QStringList &newWaveform);
    void replacebigWidgetform(const QString &oldWidget, const QString &newWidget,
                              bool setFocus = true, bool order = true);

    // 插入波形。
    void insertWaveform(const QString &frontWaveform, const QString &insertedWaveform,
                        bool setFocus = true, bool order = true);
    void insertTrendWindow(const QString &insertTrendWindow);

    // 移除波形。
    void removeWaveform(const QString &waveform, bool setFocus = true);
    void removeTrendWindow(const QString &removeTrendWindow);

    // 设置聚焦顺序。
    void setFocusOrder(void);

    // 重画波形
    void resetWave();

    // 获取当前显示的波形窗体内容。
    void getCurrentWaveforms(QStringList &waveformNames);

private:
    WindowManager();

    // 设置当前显示的波形窗体内容。
    void _setCurrentWaveforms(const QStringList &waveformNames);
    void _setCurrentTrendWindow(const QStringList &trendNames);

    // 聚焦到指定的波形窗体。
    void _focusWaveformWidget(const QString &name);

    // 获取当前显示的波形。
    void _getDisplayedWaveform(QList<WaveWidget *> &widgets);
    void _getDisplayedWaveWidget(QList<IWidget *> &widgets);
    void _getDisplayedWaveTrendWidget(QStringList &names);
    void _getCurrentDisplayTrendWindow(QStringList &names);

    // 布局样式生成。
    void _newLayoutStyle(void);
    void _paramLayoutStyle(UserFaceType type);
    void _fixedLayout(void);
    void _trendLayout(UserFaceType type);

    void _execVolitileLayout(QStringList &nodeWidgets);
    void _doVolatileLayout(UserFaceType type);
    void _volatileStandardLayout(void);
    void _volatile12LeadLayout(void);
    void _volatileOxyCRGLayout(void);
    void _volatileTrendLayout(void);
    void _volatileBigFontLayout(void);
    void _volatileCustomLayout(void);

    void _getWaveFormBoxWidget(QList<IWidget *> &list);
    void _calcWaveFactor(const QStringList &waveForms, int &normalFactor,
                         int &specialFactor, int &trendFactor);

    void _setUFaceType(UserFaceType type);
    void _setCustomFaceType(void);

    bool _doesFixedLayout;
    UserFaceType _currenUserFaceType;

    // 布局形态。
    // ----------------------------------------------------------------|
    // | Bed  | Name |  Patient Type                                   |
    // |---------------------------------------------------------------|
    // |        Phy Alarm Info      |       Tech Alarm Info            |
    // |---------------------------------------------------------------|
    // |                                       |           |           |
    // |                                       |           |           |
    // |                                       |           |           |
    // |                                       |           |           |
    // |             _volatileLayout           |   trend   |   trend   |
    // |                                       |           |           |
    // |                                       |           |           |
    // |                                       |           |           |
    // |                                       |           |           |
    // |---------------------------------------------------------------|
    // |                                            icons      Time    |
    // |---------------------------------------------------------------|
    // |                            softkey                            |
    // ----------------------------------------------------------------|
//    QHBoxLayout *_mainLayout;                // 顶层布局器。
//    QVBoxLayout *_softkeyColumn;             // softkey布局器。
//    QVBoxLayout *_firstColumn;               // 第一列布局器，bar and waveform。
//    QHBoxLayout *_barRow;                    // 放置状态栏。
//    QVBoxLayout *_volatileLayout;            // 放置波形可变区窗体。
//    QHBoxLayout *_trendHeadLayout;           // 放置第一行趋势窗体，电池图标和运行时间。
//    QVBoxLayout *_secondColumn;              // 放置趋势窗体。



    QVBoxLayout *_mainLayout;               // 顶层布局器。
    QHBoxLayout *_topBarRow;                // 顶部信息栏。
    QHBoxLayout *_paramLayout;              // 参数区
    QVBoxLayout *_volatileLayout;           // 放置波形窗体。
    QVBoxLayout *_trendRowLayout;           // 放置趋势窗体。
    QGridLayout *_paramBox;                 // 大字体界面窗体。
    QHBoxLayout *_softkeyRow;               // softkey布局器。

    // 这些布局器将根据实际情况放到_volatileLayout中。
    QVBoxLayout *_waveformBox;
    QGridLayout *_12LeadBox;

private:
    int _subMenuWidth;               // 弹出菜单宽度。
    int _subMenuHeight;              // 弹出菜单高度。

    int _volatileLayoutStretch;      // 波形区比例
    int _trendRowLayoutStretch;      // 趋势比例

    QMap<QString, IWidget *> _winMap;      // 保存所有的窗体。
    QMap<QString, IWidget *> _waveformMap; // 保存当前波形区的窗体。
    QMap<QString, QVBoxLayout *> _bigformMap; // 保存当前大字体区的窗体。
    QMultiMap<IWidget *, IWidget *> _trendWave;            // 参数列表

    WindowManagerPrivate * const d_ptr;
};
#define windowManager (WindowManager::getInstance())
Q_DECLARE_OPERATORS_FOR_FLAGS(WindowManager::ShowBehavior)
