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

//do {
//    flockfile(stderr);
//    fprintf(stderr, "%s(line %d): \t", basename(__FILE__), __LINE__);
//    fprintf(stderr, args);
//    funlockfile(stderr);
//} while(false)

#define debug(args...) \
do { \
    qDebug()<<(QString().sprintf("%s(line %d): %s", basename(__FILE__), __LINE__, \
    QString().sprintf(args).toStdString().c_str()).toStdString().c_str());\
} while(false)

#if 0
#define outHex(buff, len) \
for(int i = 0; i < len; i++) \
{\
    fprintf(stderr, "0x%02x, ", buff[i]); \
} \
fprintf(stderr, "\t%s:%d\n", basename(__FILE__), __LINE__)
#endif

#define outHex(buff, len) \
    do { \
        QString s; \
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

#define qdebug(args...) do{qDebug()<<Q_FUNC_INFO<<QString().sprintf(args);}while(0)

#else

#define dbg
#define debug(args...)
#define outHex(buff, len)
#define qdebug(args...)

#endif

// 捕捉段错误等, 自动打印函数栈
void catchFatalError(void);
void deleteErrorCatch(void);
