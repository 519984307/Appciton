/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/6
 **/

#pragma once
#include "Dialog.h"
#include <QScopedPointer>

class OxyCRGEventSetWindowPrivate;
class OxyCRGEventSetWindow : public Dialog
{
    Q_OBJECT
public:
    static OxyCRGEventSetWindow &constraction()
    {
        if (NULL == selfObj)
        {
            selfObj = new OxyCRGEventSetWindow();
        }
        return *selfObj;
    }
    ~OxyCRGEventSetWindow();

private slots:
    void trend1Slot(int);
    void compressedSlot(int);

private:
    OxyCRGEventSetWindow();
    static OxyCRGEventSetWindow *selfObj;
    QScopedPointer<OxyCRGEventSetWindowPrivate> d_ptr;
};

#define oxyCRGEventSetWindow            (OxyCRGEventSetWindow::constraction())
#define deleteOxyCRGEventSetWindow      (delete OxyCRGEventSetWindow::selfObj)
