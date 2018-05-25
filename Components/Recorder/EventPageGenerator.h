#pragma once
#include "RecorderManager.h"
#include <QScopedPointer>
#include <IStorageBackend.h>

class EventPageGeneratorPrivate;
class EventPageGenerator : public RecordPageGenerator
{
    Q_OBJECT
public:
    enum {
        Type = 3
    };

    EventPageGenerator(IStorageBackend *backend, int eventIndex, QObject *parent = 0);

    ~EventPageGenerator();

    /* override */
    virtual int type() const;

protected:
    /* override */
    virtual RecordPage *createPage();

private:
    QScopedPointer<EventPageGeneratorPrivate> d_ptr;
};
