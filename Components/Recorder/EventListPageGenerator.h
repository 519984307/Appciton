/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/12/26
 **/

#pragma once
#include "RecorderManager.h"
#include <QScopedPointer>

class EventListPageGeneratorPrivate;
class EventListPageGenerator : public RecordPageGenerator
{
    Q_OBJECT
public:
    explicit EventListPageGenerator(QStringList &eventList, const PatientInfo &patientInfo, QObject *parent = 0);
    ~EventListPageGenerator();

protected:
    virtual RecordPage *createPage();

private:
    QScopedPointer<EventListPageGeneratorPrivate> d_ptr;
};
