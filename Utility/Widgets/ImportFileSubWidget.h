/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/6
 **/

#pragma once
#include "Dialog.h"

class ImportSubWidgetPrivate;
class ImportSubWidget :public Dialog
{
    Q_OBJECT
public:
    explicit ImportSubWidget(const QStringList &iListWidgetName, const QString &path);
public slots:
    void getSelsectItems();
    void updateBtnStatus();
    QMap<int, bool> &readRowsMap();

private:
    ImportSubWidgetPrivate *const d_ptr;
};
