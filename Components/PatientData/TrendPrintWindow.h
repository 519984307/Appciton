/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/26
 **/

#pragma once
#include "Dialog.h"
#include "TrendDataDefine.h"
#include "IStorageBackend.h"

class TrendPrintWindowPrivate;
class TrendPrintWindow : public Dialog
{
    Q_OBJECT
public:
    explicit TrendPrintWindow(const QList<BlockEntry> blockList);
    ~TrendPrintWindow();

    void initPrintTime(unsigned start, unsigned end);
    void printTimeRange(unsigned startLimit, unsigned endLimit);

private slots:
    void startTimeChangeSlot(int, int);
    void endTimeChangeSlot(int, int);
    void printReleased(void);

private:
    QScopedPointer<TrendPrintWindowPrivate> d_ptr;
};
