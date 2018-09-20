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
     * @brief updateLayout update the layout info, if the internal layout info changed,
     *        the layout manager will re-perform layout stuff
     *
     */
    void updateLayout();

private:
    LayoutManagerPrivate * const d_ptr;
    LayoutManager();
};

#define layoutManager (LayoutManager::getInstance())
