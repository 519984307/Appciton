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

static FontMangerInterface* fontManager = NULL;

FontMangerInterface *FontMangerInterface::registerFontManager(FontMangerInterface *instance)
{
    FontMangerInterface *old = fontManager;
    fontManager = instance;
    return old;
}

FontMangerInterface *FontMangerInterface::getFontManager()
{
    return fontManager;
}
