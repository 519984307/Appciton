/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/12/18
 **/

#pragma once
#include <stdio.h>
#include <string.h>
#include <QDebug>
#include <QString>

/*******************************************************************************
 * Linux 程序自调试功能
 ******************************************************************************/
#ifndef NDEBUG

#define dbg \
    fprintf(stderr, "%s(line %d): %s()\t$$$$$$$$$$$$$$$$$$$$\n", basename(__FILE__), __LINE__, __FUNCTION__);

#define outHex(buff, len) \
    do { \
        QString s; \
        for (int i = 0; i < len; ++i) \
            s += QString::number(buff[i], 16) + " "; \
        qDebug() << s; \
    } while (0)

#define outHexWithTitle(title, buff, len) \
    do { \
        QString s = title; \
        for (int i = 0; i < len; ++i) \
            s += QString::number(buff[i], 16) + " "; \
        qDebug() << s; \
    } while (0)

#define outDec(buff, len) \
for(int i = 0; i < len; i++) \
{\
    fprintf(stderr, "%d, ", buff[i]); \
} \
fprintf(stderr, "\t%s:%d\n", basename(__FILE__), __LINE__)

#define qdebug(args...) do{qDebug() << Q_FUNC_INFO << QString().sprintf(args);} while(0)        // NOLINT

#define debug qdebug

#else

#define dbg
#define debug(args...)
#define outHex(buff, len)
#define qdebug(args...)

#endif

// 捕捉段错误等, 自动打印函数栈
void catchFatalError(void);
void deleteErrorCatch(void);
