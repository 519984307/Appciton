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
        Type = 5
    };

    TrendGraphPageGenerator(const QList<TrendGraphInfo>& trendInfos, unsigned startTime, unsigned endTime, QObject *parent = NULL);

    ~TrendGraphPageGenerator();

    /* override */
    virtual int type() const;

protected:
    /* override */
    virtual RecordPage *createPage();

private:
    QScopedPointer<TrendGraphPageGeneratorPrivate> d_ptr;
};
