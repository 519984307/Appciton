#include "FreezeManager.h"
#include "ParamDefine.h"
#include "TimeDate.h"
#include <QList>

class FreezeManagerPrivate
{
public:
    FreezeManagerPrivate()
        :currentReviewSecond(0),
          inReviewMode(false)
    {

    }
    int currentReviewSecond;
    QList<FreezeDataModel*> dataModels;
    bool inReviewMode;
};

FreezeManager::FreezeManager()
    :QObject(), d_ptr(new FreezeManagerPrivate())
{
}

FreezeManager::~FreezeManager()
{
    qDeleteAll(d_ptr->dataModels);
    d_ptr->dataModels.clear();
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

void FreezeManager::startFreeze()
{
    emit freeze(true);
}

void FreezeManager::stopFreeze()
{
    emit freeze(false);
    qDeleteAll(d_ptr->dataModels);
    d_ptr->dataModels.clear();
    d_ptr->inReviewMode = false;
}

bool FreezeManager::isInReviewMode() const
{
    return d_ptr->inReviewMode;
}

void FreezeManager::enterReviewMode()
{
    if(!d_ptr->inReviewMode)
    {
        d_ptr->inReviewMode = true;
        emit freezeReview();
    }
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

    foreach (FreezeDataModel *m, d_ptr->dataModels) {
        m->setReviewStartSecond(reviewSecond);
    }
}

FreezeDataModel *FreezeManager::getWaveDataModel(int waveid)
{
    WaveformID id = static_cast<WaveformID> (waveid);
    FreezeDataModel *model = new FreezeDataModel(timeDate.time(), id);
    d_ptr->dataModels.append(model);
    return model;
}
