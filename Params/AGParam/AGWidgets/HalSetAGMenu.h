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

#include "Framework/UI/MenuWindow.h"
#include <QScopedPointer>

class HalSetAGMenuPrivate;
class HalSetAGMenu : public Dialog
{
    Q_OBJECT
public:
     HalSetAGMenu();
    ~HalSetAGMenu();
public slots:
     void onComboIndexChanged(int index);
private:
    QScopedPointer<HalSetAGMenuPrivate>d_ptr;
};
