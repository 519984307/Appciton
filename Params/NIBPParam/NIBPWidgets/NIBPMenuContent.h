#pragma  once

#include "MenuContent.h"

class NIBPMenuContentPrivate;
class NIBPMenuContent : public MenuContent
{
    Q_OBJECT
public:
    explicit NIBPMenuContent();
    ~NIBPMenuContent();
    /**
     * @brief readyShow  //reimplment load settings
     */
    virtual void readyShow();
    /**
     * @brief layoutExec  //reimplment play layout
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
    void onBtnReleasedChanged(void);
private:
    NIBPMenuContentPrivate * const d_ptr;
};
