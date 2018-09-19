/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/20
 **/


#pragma once
#include "IWidget.h"
#include "SystemDefine.h"
#include "SoftkeyActionBase.h"
#include <QMap>

class SoftkeyWidget;
class SoftKeyManagerPrivate;
class SoftKeyManager : public IWidget
{
    Q_OBJECT

public:
    static SoftKeyManager &getInstance(void);
    ~SoftKeyManager();

public:
    // 界面类型到软按键类型的转换
    SoftKeyActionType uFaceTypeToSoftKeyType(UserFaceType type);

    // 根据界面类型显示不同的内容。
    void setContent(SoftKeyActionType type);

    // 切换到指定的动作组。
    void switchTo(SoftKeyActionType type);

    // 切换到上一个page。
    void previousPage(void);

    // 切换到下一个page。
    void nextPage(void);

    // 是否存在上一页
    bool hasPreviousPage();

    // 是否存在下一页
    bool hasNextPage();

    // 切换到首页
    bool returnRootPage(void);

    // 刷新当前页。
    void refresh(bool toFirstPage = false);

    // 获取软按键动作对象。
    SoftkeyActionBase *getAction(SoftKeyActionType t);

    virtual void getSubFocusWidget(QList<QWidget*> &/*subWidget*/) const;

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

private slots:
    void _clickKey(IWidget *w);
    /* handle the dynamic key clicked signal */
    void _dynamicKeyClicked(int index);
    /* handle the fixed key clicked event */
    void _fixedKeyClicked();

private:
    SoftKeyManager();
    void _layoutKeys(void);

    typedef QMap<SoftKeyActionType, SoftkeyActionBase*> ActionMap;
    ActionMap _actions;
    SoftkeyActionBase *_currentAction;
    SoftKeyActionType _curSoftKeyType;

    int _totalPages;      // 总共的页数。
    int _currentPage;     // 当前页。
    QList<SoftkeyWidget*> _keyWidgets;
    SoftKeyManagerPrivate * const d_ptr;
};
#define softkeyManager (SoftKeyManager::getInstance())
