/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/4/12
 **/

#pragma once
#include "RecorderManager.h"
#include <QScopedPointer>
#include <IStorageBackend.h>

class EventPageGeneratorPrivate;
class EventPageGenerator : public RecordPageGenerator
{
    Q_OBJECT
public:
    enum {
        Type = 3
    };

    EventPageGenerator(IStorageBackend *backend, int eventIndex, const PatientInfo &patientInfo, int gain, QObject *parent = 0);

    ~EventPageGenerator();

    /* override */
    virtual int type() const;

protected:
    /* override */
    virtual RecordPage *createPage();

private:
    QScopedPointer<EventPageGeneratorPrivate> d_ptr;
};
