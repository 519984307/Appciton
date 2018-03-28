#pragma once
#include <QObject>
#include "MailRecipientEditor.h"
#include <QScopedPointer>
#include <QThread>
#include "NetworkManager.h"
#include <QMap>
#include "ECG12LeadFileBuilderBase.h"
#include "EmailAddressLayout.h"
#include <QMutex>

class MailManagerPrivate;
class MailManager : public QObject
{
    Q_OBJECT
public:
    enum Status {
        Idle,        //Idle
        Pending,     //waiting to send
        Sending,     //mail is sending
        ConnectFail, //failed to connect to the mail server
        AuthFail,    //failed to login on the mail server
        SendFail,    //send mail failed
        Cancel,  //cancel send by user
        Success      //send mail success
    };

    struct Mail {
        RecipientInfo recipient;
        QString subject;
        QString message;
        QByteArray attachmentContent;
        QString attachmentName;
        QString mailSenderName;
        QMap<QString, QByteArray> mailmap;
    };


    //singleton
    static MailManager &construction();
    ~MailManager();

    //send mail, can only send one mail every time. if current status is Pending or Sending,
    //it will return false
    bool send(const Mail &mail);
    //get the recipient list
    QList<RecipientInfo> getRecipientInfos() const;
    //get current status
    Status getStatus() const;

    void setStatus(MailManager::Status status);

    void export12LeadData(ECG12LeadFileBuilderBase *fileBuilder);

    bool sendMail();

    bool isMailSendFinish();

#ifdef CONFIG_UNIT_TEST
    friend class TestECG12L;
    friend class TestEmail;
#endif
signals:
//    void exportError();
    void exportProcessChanged(unsigned char process);
    void sendMailFinish();
    void wifiExportFileFinish(int status);

    void sendMailCancel();

private slots:
    void onOneFileComplete(QString filename, QByteArray content);
    void onBuildFinished(bool transfer);
    void cancelExport();
    void onSendMailFinish();

protected:
    const QScopedPointer<MailManagerPrivate>  d_ptr;

private:
    Q_DECLARE_PRIVATE(MailManager)
    Q_DISABLE_COPY(MailManager)
    MailManager();
    Q_PRIVATE_SLOT(d_func(), void handleResult(long))

private:
    struct Mail mail;
    ECG12LeadFileBuilderBase * _fileBuilder;
    QMutex _pendingMutex;

};

#define mailManager (MailManager::construction())
#define deleteMailManger() (delete  &mailManager)
