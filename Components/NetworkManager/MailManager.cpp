#include "MailManager.h"
#include "smtpclient.h"
#include "IConfig.h"
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include "mimetext.h"
#include "mimeattachment.h"
#include "IThread.h"
#include "Utility.h"
#include <QEventLoop>
#include <QMutexLocker>

#define RETRY_TIMES 3   //retry 3 time on failed
#define RETRY_INTERVAL 3000 //interval between each retry, ms

class MailManagerPrivate
{
public:
    Q_DECLARE_PUBLIC(MailManager)
    MailManagerPrivate(MailManager * const q_ptr) :q_ptr(q_ptr), status(MailManager::Idle){}
    MailManager * const q_ptr;
    void loadRecipients();
    void startWorkerThread();
    void handleResult(long status);
    void getExportProcess(); //进度条改变
    QString smtpServer;
    int smtpServerPort;
    QString smtpPassword;
    SmtpClient::ConnectionType smtpConnType;
    QList<RecipientInfo> recipientInfos;
//    QString mailSenderName;
    QString mailSenderAddress;
    MailManager::Status status;
    MailManager::Mail mail;
    QMutex mutex;
    QMutex processMutex;
    static quint32 curExportItems;
    static quint32 curExportItemPercent;
    static quint32 totalExportItems;
};

/***************************************************************************************************
 * loadRecipients : load recipient infos from the config files
 **************************************************************************************************/
void MailManagerPrivate::loadRecipients()
{
    QString tmpStr;
    int count;
    bool ok;

    if(!currentConfig.getStrAttr("Mail|Recipients", "Count", tmpStr))
    {
        return;
    }

    count = tmpStr.toInt(&ok);
    if(!ok)
    {
        return;
    }

    recipientInfos.clear();

    for(int i = 0; i< count; i++)
    {
        RecipientInfo recipientInfo;
        QString prefix = QString("Mail|Recipients|Recipient%1|").arg(i);
        currentConfig.getStrValue(prefix+"Address", recipientInfo.address);
        currentConfig.getStrValue(prefix+"Name", recipientInfo.name);
        recipientInfos.append(recipientInfo);
    }
}



/***************************************************************************************************
 * sendMailProc : send mail process, call in other thread, use MailManagerPrivate * as parameter.
 **************************************************************************************************/
static long sendMailProc(const QVariant &para)
{
    MimeMessage message;
    MailManagerPrivate *d = (MailManagerPrivate *) para.value<void *>();
    EmailAddress senderAddr(d->mailSenderAddress, d->mailSenderAddress);
    EmailAddress recipient(d->mail.recipient.address, d->mail.recipient.name);
    message.setSender(&senderAddr);
    message.addRecipient(&recipient);
    message.setSubject(d->mail.subject);
    message.addPart(new MimeText(d->mail.message));
    QMap<QString,QByteArray>::iterator it;

    for(it = d->mail.mailmap.begin();it != d->mail.mailmap.end();it++)
    {
        MimeAttachment *p = new MimeAttachment(it.value(),it.key());
        //debug("new ++++ %p\n", p);
        message.addPart(p);
    }

    SmtpClient client(d->smtpServer, d->smtpServerPort, d->smtpConnType);
    mailManager.connect(&mailManager, SIGNAL(sendMailCancel()), &client, SLOT(sendMailCancelProcess()));

    client.setUser(d->mailSenderAddress);
    client.setPassword(d->smtpPassword);

    MailManager::Status st;

    if(client.connectToHost())
    {
        if(client.login())
        {
            d->mutex.lock();
            d->status = MailManager::Sending;
            d->mutex.unlock();

            if(client.sendMail(message))
            {
                qdebug("Send mail success.");
                st = MailManager::Success;
            }
            else
            {
                qdebug("Send mail failed.");
                st = MailManager::SendFail;
            }
            client.quit();
        }
        else
        {
            qdebug("Login failed.");
            st = MailManager::AuthFail;
        }
    }
    else
    {
        qdebug("Connected to host failed");
        st = MailManager::ConnectFail;
    }

    if (client.isSendMessCancel())
    {
        st = MailManager::Cancel;
    }

    QList<MimePart*> mimePart;
    mimePart = message.getParts();
    //debug("delete count = %d\n", mimePart.count());
    while (!mimePart.isEmpty())
    {
        MimePart *part = mimePart.takeFirst();
        if (NULL != part)
        {
            //debug("delete ---- %p\n", part);
            delete part;
            part = NULL;
        }
    }

    return st;
}

/***************************************************************************************************
 * startWorkerThread : start the send mail thread
 **************************************************************************************************/
void MailManagerPrivate::startWorkerThread()
{
    Q_Q(MailManager);
    Util::WorkerThread *workerThread = new Util::WorkerThread(sendMailProc, QVariant::fromValue((void *)this));
    q->connect(workerThread, SIGNAL(resultReady(long)), q, SLOT(handleResult(long)));
    workerThread->start();
}

/***************************************************************************************************
 * handleResult :  handle the send mail thread signal;
 **************************************************************************************************/
void MailManagerPrivate::handleResult(long st)
{
    Q_Q(MailManager);
    mutex.lock();
    status = (MailManager::Status) st;
    mutex.unlock();
   emit q->sendMailFinish();
}

/***************************************************************************************************
 * MailManager : constructor
 **************************************************************************************************/
MailManager::MailManager()
    :d_ptr(new MailManagerPrivate(this))
{
    Q_D(MailManager);
    int connType = 0;
   // currentConfig.getStrValue("General|DeviceIdentifier", d->mail.mailSenderName);
    currentConfig.getStrValue("Mail|SmtpUsername", d->mailSenderAddress);

    currentConfig.getNumValue("Mail|ConnectionSecurity", connType);
    d->smtpConnType = (SmtpClient::ConnectionType) connType;

    currentConfig.getStrValue("Mail|SmtpServer", d->smtpServer);
    currentConfig.getNumValue("Mail|SmtpServerPort", d->smtpServerPort);
    currentConfig.getStrValue("Mail|Password", d->smtpPassword);

    _fileBuilder = NULL;
    mail.mailmap.clear();

    d->loadRecipients();

    connect(this, SIGNAL(sendMailFinish()), this, SLOT(onSendMailFinish()));
}

/***************************************************************************************************
 * construction : singleton
 **************************************************************************************************/
MailManager &MailManager::construction()
{
    static MailManager *_instance = NULL;
    if(_instance == NULL)
    {
        _instance = new MailManager();
    }
    return *_instance;
}

MailManager::~MailManager()
{
}

/***************************************************************************************************
 * send : send a  mail to specific recipient
 **************************************************************************************************/
bool MailManager::send(const MailManager::Mail &mail)
{
    Q_D(MailManager);

    QMutexLocker locker(&d->mutex);
    if(d->status == Pending || d->status == Sending)
    {
        return false;
    }

    d->mail = mail;
    d->status = MailManager::Pending;
    locker.unlock();
    d->startWorkerThread();
    return true;
}

/***************************************************************************************************
 * getRecipientInfos : get the Recipient info list
 **************************************************************************************************/
QList<RecipientInfo> MailManager::getRecipientInfos() const
{
    Q_D(const MailManager);
    return d->recipientInfos;
}

/***************************************************************************************************
 * getStatus : get MailManager status
 **************************************************************************************************/
MailManager::Status MailManager::getStatus() const
{
    Q_D(const MailManager);
    return d->status;
}

/***************************************************************************************************
 * setStatus : set MailManager status
 **************************************************************************************************/
void MailManager::setStatus(MailManager::Status status)
{
    Q_D(MailManager);

    d->mutex.lock();
    d->status = status;
    d->mutex.unlock();

    return;
}

/***************************************************************************************************
 * export12LeadData : export 12-Lead report by Wifi
 **************************************************************************************************/
void MailManager::export12LeadData(ECG12LeadFileBuilderBase *fileBuilder)
{
    fileBuilder->moveToThread(thread());
    connect(fileBuilder, SIGNAL(oneFileComplete(QString, QByteArray)), this, SLOT(onOneFileComplete(QString, QByteArray)));
    connect(fileBuilder, SIGNAL(buildFinished(bool)), this, SLOT(onBuildFinished(bool)));

    QMutexLocker locker(&_pendingMutex);

    _fileBuilder = fileBuilder;
    setStatus(Idle);
    QTimer::singleShot(0, _fileBuilder, SLOT(startBuilding()));

}

/***************************************************************************************************
 * onOneFileComplete : finish build one report file
 **************************************************************************************************/
void MailManager::onOneFileComplete(QString filename, QByteArray content)
{
    if (filename.isEmpty() || (0 == content.count()))
    {
        return;
    }

    QString filepath = filename;
    int index = filename.lastIndexOf("/");
    QString name = filepath.right(filepath.length() - index - 1);

    mail.mailmap.insert(name, content);

    emit exportProcessChanged(90 * _fileBuilder->filesProcess() / _fileBuilder->filesToBuild());

    return ;
}

/***************************************************************************************************
 * onBuildFinished : all report file build finish
 **************************************************************************************************/
void MailManager::onBuildFinished(bool transfer)
{
    if (transfer)
    {
        sendMail();
        _pendingMutex.lock();
        mail.mailmap.clear();
        delete _fileBuilder;
        _fileBuilder = NULL;
        _pendingMutex.unlock();
    }
    else
    {
        //caneled
        _pendingMutex.lock();
        mail.mailmap.clear();
        delete _fileBuilder;
        _fileBuilder = NULL;

        Q_D(MailManager);
        d->handleResult(Cancel);

        _pendingMutex.unlock();
    }

    return;
}

/***************************************************************************************************
 * cancelExport : transfer cancel
 **************************************************************************************************/
void MailManager::cancelExport()
{
    emit sendMailCancel();

    if (_fileBuilder)
    {
        _fileBuilder->cancelBuilding();
    }

    return;
}

/***************************************************************************************************
 * onSendMailFinish : send mail finish cb
 **************************************************************************************************/
void MailManager::onSendMailFinish()
{
    emit wifiExportFileFinish((int)getStatus());

    if (Success == getStatus())
    {
        emit exportProcessChanged(100);
    }

    return;
}

/***************************************************************************************************
 * sendMail : send mail
 **************************************************************************************************/
bool MailManager::sendMail()
{
    int emailAddressindex = mailAddressWidget.getEmailAddressOffset();
    if(getRecipientInfos().count() == 0)
    {
        return true;
    }

    if(emailAddressindex > 4)
    {
        emailAddressindex = 0;
    }

    mail.subject = "nPM Export File";
    mail.recipient = getRecipientInfos().at(emailAddressindex);
    mail.message = "";

    return send(mail);
}

/***************************************************************************************************
 * isMailSendFinish : send mail finish 
 **************************************************************************************************/
bool MailManager::isMailSendFinish()
{
    if (NULL == _fileBuilder)
    {
        return true;
    }
    else
    {
        return false;
    }
}


#include "moc_MailManager.cpp"
