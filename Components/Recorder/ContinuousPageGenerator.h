/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/19
 **/



#pragma once
#include "RecordPageGenerator.h"
#include "PrintDefine.h"
#include <QScopedPointer>

class ContinuousPageGeneratorPrivate;
class ContinuousPageGenerator : public RecordPageGenerator
{
    Q_OBJECT
public:
    enum {
        Type = 2
    };

    explicit ContinuousPageGenerator(QObject *parent = 0);
    ~ContinuousPageGenerator();

    /* override */
    virtual int type() const;

    /* override */
    PrintPriority getPriority() const;

protected:
    /* override */
    virtual RecordPage *createPage();


    /* override */
    virtual void onStartGenerate();

    /* override */
    virtual void onStopGenerate();

private:
    QScopedPointer<ContinuousPageGeneratorPrivate> d_ptr;
};
