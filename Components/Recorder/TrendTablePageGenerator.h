#pragma once
#include "RecorderManager.h"
#include <QScopedPointer>
#include <IStorageBackend.h>

class TrendTablePageGeneratorPrivate;
class TrendTablePageGenerator : public RecordPageGenerator
{
    Q_OBJECT
public:
    enum {
        Type = 4
    };

    TrendTablePageGenerator(IStorageBackend *backend, int startIndex, int stopIndex, ResolutionRatio interval,QObject *parent = 0);

    ~TrendTablePageGenerator();

    /* override */
    virtual int type() const;

protected:
    /* override */
    virtual RecordPage *createPage();

private:
    QScopedPointer<TrendTablePageGeneratorPrivate> d_ptr;
};
