#pragma once
#include <QObject>
#include <QScopedPointer>
#include "FreezeDataModel.h"
#include "TrendDataDefine.h"

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
     * @brief isInReviewMode check whether in review freeze data mode
     * @return
     */
    bool isInReviewMode() const;

    /**
     * @brief enterReviewMode enter the review freeze wave mode
     */
    void enterReviewMode();

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

    /**
     * @brief getTrendData get the trend data that capture when the freeze button press
     * @return trenddatapacket
     */
    const TrendDataPackage &getTrendData() const;

signals:
    // notify freeze or not
    void freeze(bool flag);
    // enter freeze review mode
    void freezeReview();

private:
    QScopedPointer<FreezeManagerPrivate> d_ptr;
};

#define freezeManager (FreezeManager::getInstance())
