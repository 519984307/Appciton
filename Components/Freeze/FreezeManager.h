#pragma once
#include <QObject>
#include <QScopedPointer>

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
     * @brief getCurReviewSecond get the current review second
     * @return
     */
    int getCurReviewSecond() const;

    /**
     * @brief setCurReviewSecond set teh current review second
     * @param reviewSecond
     */
    void setCurReviewSecond(int reviewSecond);

signals:
    //notify the freeze review time changed
    void reviewSecondChanged(int second);
    //notify freeze or not
    void freeze(bool flag);

private slots:
    void testSlot();

private:
    QScopedPointer<FreezeManagerPrivate> d_ptr;
};

#define freezeManager (FreezeManager::getInstance())
