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

    /* override */
    PrintPriority getPriority() const;

protected:
    /* override */
    virtual RecordPage *createPage();


    /* override */
    virtual void onStartGenerate();

    /* override */
    virtual void onStopGenerate();

private:
    QScopedPointer<ContinuousPageGeneratorPrivate> d_ptr;
};
