#pragma once
#include "RecordPageGenerator.h"
#include <QScopedPointer>
#include "IStorageBackend.h"
#include <QList>

class TrendGraphPageGeneratorPrivate;
class TrendGraphPageGenerator : public RecordPageGenerator
{
public:
    enum {
        Type = 3
    };



    TrendGraphPageGenerator(const QList<TrendGraphInfo>& trendInfos, unsigned startTime, unsigned endTime, QObject *parent = NULL);

    ~TrendGraphPageGenerator();

    /* overide */
    virtual int type() const;

protected:
    /* overide */
    virtual RecordPage *createPage();

private:
    QScopedPointer<TrendGraphPageGeneratorPrivate> d_ptr;
};
