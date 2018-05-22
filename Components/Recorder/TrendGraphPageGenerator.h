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

    struct TrendGraphInfo
    {
        SubParamID subParamID;
        UnitType unit;
        struct {
            int max;
            int min;
        } scale;
        QVector<short> trendData;
    };

    TrendGraphPageGenerator(IStorageBackend *backend, const QList<TrendGraphInfo>& trendInfos, unsigned startTime, unsigned endTime, QObject *parent = NULL);

    ~TrendGraphPageGenerator();

    /* overide */
    virtual int type() const;

protected:
    /* overide */
    virtual RecordPage *createPage();

private:
    QScopedPointer<TrendGraphPageGeneratorPrivate> d_ptr;
};
