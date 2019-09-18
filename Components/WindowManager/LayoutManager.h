/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/17
 **/

#pragma once
#include <QObject>
#include "SystemDefine.h"
#include "ScreenLayoutDefine.h"

class LayoutManagerPrivate;
class QLayout;
class IWidget;
class WaveWidget;
class LayoutManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief ScreenLayoutManager::getInstance get the instance
     */
    static LayoutManager &getInstance();

    ~LayoutManager();

    /**
     * @brief reloadLayoutConfig reload the layout info from the config file,
     *        should be called when the config changed
     */
    void reloadLayoutConfig();

    /**
     * @brief mainLayout get the application's main layout
     * @return
     */
    QLayout *mainLayout();

    /**
     * @brief getContentView the contentView
     * @return the Contenet View widget
     */
    IWidget *getContentView() const;

    /**
     * @brief addLayoutWidget add widget that need layout
     * @param w the widget
     * @param nodeType the correspond node type in the content layout system
     */
    void addLayoutWidget(IWidget *w, LayoutNodeType nodeType = LAYOUT_NODE_NONE);

    /**
     * @brief getLayoutWidget get the layout widget base on the widget's name
     * @param name the layout widget's name
     * @return  the layout widget
     */
    IWidget *getLayoutWidget(const QString &name);

    /**
     * @brief setUFaceType set the current User interface
     * @param type the inter face type
     */
    void setUFaceType(UserFaceType type);

    /**
     * @brief getUFaceType get the current user interface layout type
     * @return
     */
    UserFaceType getUFaceType() const;

    /**
     * @brief getDisplayedWaveform get the current displayed waveforms
     * @return a list of display wavewidget's name
     */
    QStringList getDisplayedWaveforms() const;

    /**
     * @brief getTheDisplayWaveformIDs get the current displayed waveforms id
     * @return a list of display waveform's id
     */
    QList<int> getDisplayedWaveformIDs() const;

    /**
     * @brief getDisplayedWaveformLabels get the display waveforms labels
     * @return a list of display waveform's label
     */
    QStringList getDisplayedWaveformLabels() const;

    /**
     * @brief getDisplayedWaveWidget get the display wave widget
     * @param id the waveform id
     * @return pointer to the waveWidget if the wave is displayed, otherwise, return NULL
     */
    WaveWidget *getDisplayedWaveWidget(WaveformID id);

    /**
     * @brief isLastWaveWidget check whether a wave widget is the last display widget
     * @return true if the widget if the last display wave widget
     */
    bool isLastWaveWidget(const WaveWidget * w) const;

    /**
     * @brief resetWave reset the displayed wave widget
     */
    void resetWave();

    /**
     * @brief setWidgetLayoutable set whether the widget is layoutable in the layout manager
     * @node  all widget is layoutable by default
     *        you should call @updateLayout to fresh the layout if yout want the setting take effect
     * @param name the widget's name
     * @param enable true if layoutable, otherwise, false
     * @return true if the layout need to update
     */
    bool setWidgetLayoutable(const QString &name, bool enable);

    /**
     * @brief getMenuArea get the menu arae in global coordinate
     * @note    menu arae is a place where the popup window/menu show show at
     * @return the proper menu area
     */
    QRect getMenuArea() const;

    /**
     * @brief updateTabOrder update the tab order
     */
    void updateTabOrder();

    /**
     * @brief updateLayoutWidgetsConfig  更新布局窗口配置
     */
    void updateLayoutWidgetsConfig();

public slots:
    /**
     * @brief updateLayout update the layout info and re-perform layout stuff
     */
    void updateLayout();

    /**
     * @brief patientTypeChangeSlot - 病人类型改变槽函数
     */
    void patientTypeChangeSlot();

signals:
    /* emit when the content layout changed */
    void layoutChanged();

private:
    LayoutManagerPrivate * const d_ptr;
    LayoutManager();
};

#define layoutManager (LayoutManager::getInstance())
