#ifndef TDA19988CTRL_H
#define TDA19988CTRL_H
#include <QScopedPointer>
#include <QObject>

class TDA19988CtrlPrivate;
class TDA19988Ctrl: public QObject
{
    Q_OBJECT
public:
    TDA19988Ctrl(QObject *parent=0);
    ~TDA19988Ctrl();

    //check this control is valid or not
    bool isValid() const;

    //check whether hdim is connnected
    bool isConnected() const;

    //enable signal output or not
    void setSignalOutput(bool onOff);

private slots:
    //call periodly to check the hdmi connection
    void checkHDMIConnection();

private:
    QScopedPointer<TDA19988CtrlPrivate> d_ptr;
    Q_DISABLE_COPY(TDA19988Ctrl)
};
#endif // TDA19988CTRL_H
