/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/20
 **/

#pragma once
#include <QFont>

class FontMangerInterface
{
public:
    virtual ~FontMangerInterface(){}

    static FontMangerInterface *registerFontManager(FontMangerInterface *instance);

    static FontMangerInterface *getFontManager(void);

    virtual QFont textFont(int fontSize = -1, bool isBold = false) = 0;

    virtual int getFontSize(int index) = 0;
};
