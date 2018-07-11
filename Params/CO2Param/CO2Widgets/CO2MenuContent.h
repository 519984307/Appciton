#pragma once

#include "MenuContent.h"

class CO2MenuContentPrivate;
class CO2MenuContent: public MenuContent
{
    Q_OBJECT
public:
    explicit CO2MenuContent();
    ~CO2MenuContent();
    /**
     * @brief readyShow
     */
    virtual void readyShow();
    /**
     * @brief layoutExec
     */
    virtual void layoutExec();
private slots:
    /**
     * @brief onComboBoxIndexChanged
     * @param index
     */
    void onComboBoxIndexChanged(int index);
    /**
     * @brief onBtnReleasedChanged
     */
    void onBtnReleasedChanged();
private:
    CO2MenuContentPrivate * const d_ptr;
};
