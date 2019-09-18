/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/16
 **/

#pragma once
#include <QStringList>
#include <QMetaType>

struct ItemEditInfo
{
    ItemEditInfo()
        : type(VALUE), highLimit(100), lowLimit(0),
          curValue(50), scale(1), step(1), startValue(0)
    {
    }

    enum EditType
    {
        VALUE,
        LIST,
    };

    EditType type;
    int highLimit;
    int lowLimit;
    int curValue;
    int scale;
    int step;
    int startValue;
    QStringList list;
};

Q_DECLARE_METATYPE(ItemEditInfo)
