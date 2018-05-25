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

    OxyCRGPageGenerator(const QList<TrendGraphInfo> &trendInfos, const OxyCRGWaveInfo &waveInfo, QObject *parent = 0);
    ~OxyCRGPageGenerator();

    /* override */
    virtual int type() const;

protected:
    /* override */
    virtual RecordPage *createPage();

private:
    QScopedPointer<OxyCRGPageGeneratorPrivate> d_ptr;
};


