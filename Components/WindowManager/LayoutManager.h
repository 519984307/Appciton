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
class LayoutManager : public QObject
{
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
     * @brief updateLayout update the layout info and re-perform layout stuff
     */
    void updateLayout();

    /**
     * @brief getDisplayedWaveform get the current displayed waveforms
     * @return a list of display wavewidget's name
     */
    QStringList getDisplayedWaveforms();

    /**
     * @brief getTheDisplayWaveformIDs get the current displayed waveforms id
     * @return a list of display waveform's id
     */
    QList<int> getDisplayedWaveformIDs();

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

private:
    LayoutManagerPrivate * const d_ptr;
    LayoutManager();
};

#define layoutManager (LayoutManager::getInstance())
