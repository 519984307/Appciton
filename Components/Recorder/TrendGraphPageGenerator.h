/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/25
 **/

#pragma once
#include "RecordPageGenerator.h"
#include <QScopedPointer>
#include "IStorageBackend.h"
#include <QList>
#include "EventDataDefine.h"

class TrendGraphPageGeneratorPrivate;
class TrendGraphPageGenerator : public RecordPageGenerator
{
public:
    enum {
        Type = 5
    };

    TrendGraphPageGenerator(const QList<TrendGraphInfo>& trendInfos, const QList<BlockEntry>& eventList,
                            const PatientInfo &patientInfo, QObject *parent = NULL);

    ~TrendGraphPageGenerator();

    /* override */
    virtual int type() const;

protected:
    /* override */
    virtual RecordPage *createPage();

private:
    QScopedPointer<TrendGraphPageGeneratorPrivate> d_ptr;
};
