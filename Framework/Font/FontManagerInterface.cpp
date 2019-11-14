/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/20
 **/

#include "FontManagerInterface.h"
#include <stddef.h>

static FontManagerInterface* fontManager = NULL;

FontManagerInterface *FontManagerInterface::registerFontManager(FontManagerInterface *instance)
{
    FontManagerInterface *old = fontManager;
    fontManager = instance;
    return old;
}

FontManagerInterface *FontManagerInterface::getFontManager()
{
    return fontManager;
}
