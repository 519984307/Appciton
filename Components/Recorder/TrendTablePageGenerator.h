/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/30
 **/

#pragma once
#include "RecorderManager.h"
#include <QScopedPointer>
#include "Framework/Storage/IStorageBackend.h"

struct TrendTablePrintInfo
{
    TrendTablePrintInfo(): startIndex(0), stopIndex(0), interval(0)
    {}
    int startIndex;
    int stopIndex;
    int interval;
    QList<SubParamID> list;
    QMap<unsigned int, unsigned int> timestampEventMap;  // key--timestamp  value--event status
};

class TrendTablePageGeneratorPrivate;
class TrendTablePageGenerator : public RecordPageGenerator
{
    Q_OBJECT
public:
    enum {
        Type = 4
    };

    TrendTablePageGenerator(IStorageBackend *backend, const TrendTablePrintInfo &printInfo,
                            const PatientInfo &patientInfo, QObject *parent = 0);

    ~TrendTablePageGenerator();

    /* override */
    virtual int type() const;

protected:
    /* override */
    virtual RecordPage *createPage();

private:
    QScopedPointer<TrendTablePageGeneratorPrivate> d_ptr;
};
