/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/18
 **/

#pragma once
#include "RecordPageGenerator.h"
#include <QScopedPointer>

class OxyCRGPageGeneratorPrivate;
class OxyCRGPageGenerator : public RecordPageGenerator
{
    Q_OBJECT
public:
    enum {
      Type = 6
    };

    OxyCRGPageGenerator(const QList<TrendGraphInfo> &trendInfos, const OxyCRGWaveInfo &waveInfo, QString &eventTitle, QObject *parent = 0);
    ~OxyCRGPageGenerator();

    /* override */
    virtual int type() const;

protected:
    /* override */
    virtual RecordPage *createPage();

private:
    QScopedPointer<OxyCRGPageGeneratorPrivate> d_ptr;
};


