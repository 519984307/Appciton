#pragma once
#include "IButton.h"

enum ButtonStatus
{
    BUTTON_FOCUS_IN,
    BUTTON_FOCUS_OUT
};

class IMoveButton : public IButton
{
    Q_OBJECT

public:
    IMoveButton(const QString &text = QString::null, QWidget *parent = 0);
    ~IMoveButton();

signals:
    void leftMove();
    void rightMove();

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);

private:
    ButtonStatus _status;
};
