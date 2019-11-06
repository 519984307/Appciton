/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/10/30
 **/
#pragma once

#include "MenuWindow.h"
#include <QScopedPointer>

class CO2SetAGMenuPrivate;
class CO2SetAGMenu : public Dialog
{
    Q_OBJECT
public:
     CO2SetAGMenu();
    ~CO2SetAGMenu();
public slots:
     void onComboIndexChanged(int index);
private:
    QScopedPointer<CO2SetAGMenuPrivate>d_ptr;
};
