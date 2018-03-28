#pragma once

#include <PopupWidget.h>

class PMessageBox : public PopupWidget
{
    Q_OBJECT

public:
    static PMessageBox &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new PMessageBox();
        }
        return *_selfObj;
    }

    static PMessageBox* _selfObj;

    PMessageBox(QString message = "", QColor bgColor = QColor(150, 200, 200), QColor textColor = Qt::black);
    ~PMessageBox();

    void setMessBoxInfo(QString message = "", QColor bgColor = QColor(150, 200, 200), QColor textColor = Qt::black);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void showEvent(QShowEvent *event);

public slots:
    void _exit();

private:
    QString _message;
    QColor _bgColor;
    QColor _textColor;

    QTimer *_timer;
};

#define pMessageBox (PMessageBox::construction())
#define deletepMessageBox() (delete PMessageBox::_selfObj)
