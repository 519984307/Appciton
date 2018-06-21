#pragma once
#include "RecorderManager.h"
#include <QScopedPointer>
#include "EventStorageItem.h"

class TriggerPageGeneratorPrivate;
class TriggerPageGenerator : public RecordPageGenerator
{
    Q_OBJECT
public:
    enum {
        Type = 8
    };

    TriggerPageGenerator(EventStorageItem *item, QObject *parent = 0);

    ~TriggerPageGenerator();

    /* override */
    virtual int type() const;

    /* override */
    virtual PrintPriority getPriority() const;

protected:
    /* override */
    virtual RecordPage *createPage();

private:
    QScopedPointer<TriggerPageGeneratorPrivate> d_ptr;
};
