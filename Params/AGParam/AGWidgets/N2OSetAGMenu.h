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

class N2OSetAGMenuPrivate;
class N2OSetAGMenu : public Dialog
{
    Q_OBJECT
public:
     N2OSetAGMenu();
    ~N2OSetAGMenu();
public slots:
     void onComboIndexChanged(int index);
private:
    QScopedPointer<N2OSetAGMenuPrivate>d_ptr;
};
