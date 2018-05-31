#include "FreezeManager.h"
#include <QTimer>

class FreezeManagerPrivate
{
public:
    FreezeManagerPrivate()
        :currentReviewSecond(0)
    {

    }
    int currentReviewSecond;
};

FreezeManager::FreezeManager()
    :QObject(), d_ptr(new FreezeManagerPrivate())
{
    QTimer::singleShot(10000, this, SLOT(testSlot()));
}

FreezeManager::~FreezeManager()
{

}

FreezeManager &FreezeManager::getInstance()
{
    static FreezeManager  *instance = NULL;
    if(instance == NULL)
    {
        instance = new FreezeManager();
    }
    return *instance;
}

int FreezeManager::getCurReviewSecond() const
{
    return d_ptr->currentReviewSecond;
}

void FreezeManager::setCurReviewSecond(int reviewSecond)
{
    if(reviewSecond == d_ptr->currentReviewSecond || reviewSecond > MAX_REVIEW_SECOND)
    {
        return;
    }

    d_ptr->currentReviewSecond = reviewSecond;

    emit reviewSecondChanged(d_ptr->currentReviewSecond);
}

void FreezeManager::testSlot()
{
    //emit freeze(true);
}
