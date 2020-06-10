/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/10
 **/

#include "COMeasureWindow.h"
#include "Framework/UI/ThemeManager.h"
#include "Framework/Language/LanguageManager.h"

class COMeasureWindowPrivate
{
public:
    COMeasureWindowPrivate() {}
};

COMeasureWindow::COMeasureWindow()
    : Dialog(), pimpl(new COMeasureWindowPrivate())
{
    setFixedSize(themeManager.defaultWindowSize());
    setWindowTitle(trs("COMeasurement"));
}

COMeasureWindow::~COMeasureWindow()
{
}
