#pragma once
#include <QObject>
#include <QScopedPointer>
#include "FreezeDataModel.h"


class FreezeManagerPrivate;
class FreezeManager : public QObject
{
    Q_OBJECT
public:
    enum {
        MAX_REVIEW_SECOND = 120
    };
    FreezeManager();
    ~FreezeManager();

    static FreezeManager &getInstance();

    /**
     * @brief startFreeze start freeze the wave
     */
    void startFreeze();

    /**
     * @brief stopFreeze stop freeze
     */
    void stopFreeze();

    /**
     * @brief getCurReviewSecond get the current review second
     * @return
     */
    int getCurReviewSecond() const;

    /**
     * @brief setCurReviewSecond set teh current review second
     * @param reviewSecond
     */
    void setCurReviewSecond(int reviewSecond);

    /**
     * @brief getWaveDataModel get the review wave data model
     * @param waveid
     * @return
     */
    FreezeDataModel* getWaveDataModel(int waveid);


signals:
    //notify freeze or not
    void freeze(bool flag);

private:
    QScopedPointer<FreezeManagerPrivate> d_ptr;
};

#define freezeManager (FreezeManager::getInstance())
