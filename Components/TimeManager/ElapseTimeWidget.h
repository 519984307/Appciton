/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/19
 **/

#pragma once
#include "IWidget.h"

class QLabel;
class QHBoxLayout;

/***************************************************************************************************
 * 显示流逝的时间。
 **************************************************************************************************/
class ElapseTimeWidget : public IWidget
{
    Q_OBJECT

public:
    explicit ElapseTimeWidget(QWidget *parent = 0);
    void setText(const QString &str);

private:
    QLabel *_elapseLabel;
};
