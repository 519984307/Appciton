/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/30
 **/

#pragma once
#include "Framework/UI/TableViewItemDelegate.h"

class ScreenLayoutItemDelegatePrivate;
class QTableView;
class ScreenLayoutItemDelegate : public TableViewItemDelegate
{
    Q_OBJECT
public:
    explicit ScreenLayoutItemDelegate(QTableView *view = NULL);

protected:
    /* reimplement */
    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
                     const QRect &rect, const QString &text) const;

private slots:
    void onSelectChanged(const QString &text);
    void onCommitChanged(int role, const QString &value);
    void onEditorDestroy();

private:
    Q_DECLARE_PRIVATE(ScreenLayoutItemDelegate)
};
