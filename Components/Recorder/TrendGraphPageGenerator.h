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

    struct TrendGraphData
    {
        unsigned timestamp;
        TrendDataType data;
    };

    struct TrendGraphDataV3     //trend graph draw 3 values
    {
        unsigned timestamp;
        TrendDataType data[3];
    };

    struct TrendGraphInfo
    {
        SubParamID subParamID;
        UnitType unit;
        struct {
            int max;
            int min;
        } scale;
        QVector<TrendGraphData> trendData;
        QVector<TrendGraphDataV3> trendDataV3;
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
