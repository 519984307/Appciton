#pragma once
#include "RecordPageGenerator.h"
#include <QScopedPointer>
#include <QVector>
#include <QMap>

class FreezePageGeneratorPrivate;
class FreezePageGenerator : public RecordPageGenerator
{
    Q_OBJECT
public:
    enum {
        Type = 7
    };

    struct FreezeWaveInfo
    {
        unsigned timestampOfLastSecond;     //timestamp of the last second wave samples
        WaveformID id;
        QVector<WaveDataType> data;
    };

    FreezePageGenerator(const TrendDataPackage &trendData,
                        const QList<FreezeWaveInfo> &freezeWaveInfos,
                        QObject *parent = 0);
    ~FreezePageGenerator();

    /* override */
    virtual int type() const;

protected:
    /* override */
    virtual RecordPage *createPage();

private:
    QScopedPointer<FreezePageGeneratorPrivate> d_ptr;
};
