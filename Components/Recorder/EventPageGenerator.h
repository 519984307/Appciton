#include "RecorderManager.h"
#include <QScopedPointer>

class EventPageGeneratorPrivate;
class EventPageGenerator : public RecordPageGenerator
{
    Q_OBJECT
public:
    enum {
        Type = 2
    };

    EventPageGenerator(QObject *parent = 0);
    ~EventPageGenerator();

    /* override */
    virtual int type() const;

protected:
    /* override */
    virtual RecordPage *createPage();

private:
    QScopedPointer<EventPageGeneratorPrivate> d_ptr;
};
