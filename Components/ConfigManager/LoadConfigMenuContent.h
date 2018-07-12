#pragma once

#include "LoadConfigMenuContent.h"
#include "MenuContent.h"

class LoadConfigMenuContentPrivate;
class LoadConfigMenuContent: public MenuContent
{
    Q_OBJECT
public:
    /**
     * @brief LoadConfigMenuContent
     */
    explicit LoadConfigMenuContent();
    ~LoadConfigMenuContent();
    /**
     * @brief readyShow
     */
    virtual void readyShow();
    /**
     * @brief layoutExec
     */
    virtual void layoutExec();
    /**
     * @brief hideEvent
     */
    virtual void hideEvent(QHideEvent *);
    /**
     * @brief changeEvent
     */
    virtual void changeEvent(QEvent *);
signals:
    /**
     * @brief configUpdated
     */
    void configUpdated();
private slots:
    /**
     * @brief onConfigClick
     */
    void onConfigClick();
    /**
     * @brief onBtnClick
     */
    void onBtnClick();
private:
    LoadConfigMenuContentPrivate * const d_ptr;
};
