/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/11
 **/

#pragma once
#include "Dialog.h"
#include "ScreenLayoutDefine.h"

class ScreenLayoutEditorPrivate;
class ScreenLayoutEditor : public Dialog
{
    Q_OBJECT
public:
    explicit ScreenLayoutEditor(const QString &title = QString());
    ~ScreenLayoutEditor();

    /**
     * @brief setReplacelist set the avaliable replacable list
     * @param list the list
     */
    void setReplaceList(const QVariantList &list);

    /**
     * @brief setInsertList set the replace list
     * @param list
     */
    void setInsertList(const QVariantList &list);

    /**
     * @brief setDisplayPosition
     * @param p
     */
    void setDisplayPosition(const QPoint &p);

    /**
     * @brief setRemoveable set whether the region is removeable
     * @param flag true if is removeable
     */
    void setRemoveable(bool flag);

signals:
    void commitChanged(int role, const QString &value);


private slots:
    void onComboIndexChanged(int index);
    void onRemoveBtnClicked();

protected:
    void showEvent(QShowEvent *ev);

private:
    ScreenLayoutEditorPrivate * const d_ptr;
};
