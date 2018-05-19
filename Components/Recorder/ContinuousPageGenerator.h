#pragma once
#include "RecordPageGenerator.h"
#include <QScopedPointer>

class ContinuousPageGeneratorPrivate;
class ContinuousPageGenerator : public RecordPageGenerator
{
    Q_OBJECT
public:
    enum {
        Type = 2
    };

    ContinuousPageGenerator(QObject *parent = 0);
    ~ContinuousPageGenerator();

    /* override */
    virtual int type() const;

protected:
    /* override */
    virtual RecordPage *createPage();

private:
    QScopedPointer<ContinuousPageGeneratorPrivate> d_ptr;
};
