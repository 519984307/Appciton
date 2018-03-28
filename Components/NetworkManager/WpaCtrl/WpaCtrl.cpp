#include "WpaCtrl.h"
#include "wpa_ctrl.h"
#include <QSocketNotifier>
#include <QTimer>
#include "Debug.h"
#include <dirent.h>
#include <QStringList>
#include <QProcess>
#include <QDir>
#include <QBasicTimer>
#include <QTimerEvent>
#include <unistd.h>
#include <QFile>

#define CTRL_IFACE_DIR "/var/run/wpa_supplicant"
#define WPA_SUPPLICANT_CONF_PATH "/etc/wpa_supplicant.conf"
#define PING_INTERVAL 1000  //1s
#define SCAN_PERIOD 10 //scan while disconnect

class WpaCtrlPrivate {
public:
    Q_DECLARE_PUBLIC(WpaCtrl)
    WpaCtrlPrivate(WpaCtrl * const q_ptr)
        :q_ptr(q_ptr), pingsToStatusUpdate(0), requestToScan(SCAN_PERIOD), ctrl_conn(NULL),
          monitor_conn(NULL), msgNotifier(NULL), timer(NULL), ctrl_iface(NULL), scanResultReady(false){}
    ~WpaCtrlPrivate();


    int ctrlRequest(const char *cmd, char *buf, size_t *buflen);
    int openCtrlConnection(const char *ifname);
    void receiveMsgs();
    void processMsg(char * msg);
    void ping();
    void triggerUpdate();
    void updateScanResults();
    void updateStatus();
    void startWpaSupplicant(const char *ifname);
    void terminate();

    WpaCtrl * const q_ptr;
    int pingsToStatusUpdate;
    int requestToScan;
    struct wpa_ctrl *ctrl_conn;
    struct wpa_ctrl *monitor_conn;
    QSocketNotifier *msgNotifier;
    QBasicTimer *timer;
    char *ctrl_iface;
    bool scanResultReady;
    QList<SSIDInfo> scanResults;
    WiFiStatus status;
};

WpaCtrlPrivate::~WpaCtrlPrivate()
{
   delete msgNotifier;
    if(timer)
    {
        delete timer;
        timer = NULL;
    }
   if(monitor_conn)
   {
       wpa_ctrl_detach(monitor_conn);
       wpa_ctrl_close(monitor_conn);
       monitor_conn = NULL;
   }
   if(ctrl_conn)
   {
       wpa_ctrl_close(ctrl_conn);
       ctrl_conn = NULL;
   }
   if(ctrl_iface)
   {
       free(ctrl_iface);
       ctrl_iface = NULL;
   }
}

/***************************************************************************************************
 * ctrlRequest : set wpa command, command result will store at @buf with a length of @buflen
 **************************************************************************************************/
int WpaCtrlPrivate::ctrlRequest(const char *cmd, char *buf, size_t *buflen)
{
    int ret;

    if (ctrl_conn == NULL)
        return -3;
    ret = wpa_ctrl_request(ctrl_conn, cmd, strlen(cmd), buf, buflen, NULL);
    if (ret == -2)
        qdebug("'%s' command timed out.", cmd);
    else if (ret < 0)
        qdebug("'%s' command failed.", cmd);

    return ret;
}

/***************************************************************************************************
 * openCtrlConnection : open the wpa control interface, return 0 on success, ohterwise, return -1
 **************************************************************************************************/
int WpaCtrlPrivate::openCtrlConnection(const char *ifname)
{
    char *cfile;
    int flen;

    if (ifname) {
        if (ifname != ctrl_iface) {
            free(ctrl_iface);
            ctrl_iface = strdup(ifname);
        }
        startWpaSupplicant(ctrl_iface);
    } else {
        struct dirent *dent;
        DIR *dir = opendir(CTRL_IFACE_DIR);
        free(ctrl_iface);
        ctrl_iface = NULL;
        if (dir) {
            while ((dent = readdir(dir))) {
#ifdef _DIRENT_HAVE_D_TYPE
                /* Skip the file if it is not a socket.
                 * Also accept DT_UNKNOWN (0) in case
                 * the C library or underlying file
                 * system does not support d_type. */
                if (dent->d_type != DT_SOCK &&
                    dent->d_type != DT_UNKNOWN)
                    continue;
#endif /* _DIRENT_HAVE_D_TYPE */

                if (strcmp(dent->d_name, ".") == 0 ||
                    strcmp(dent->d_name, "..") == 0)
                    continue;
                qdebug("Selected interface '%s'",
                      dent->d_name);
                ctrl_iface = strdup(dent->d_name);
                break;
            }
            closedir(dir);
        }
    }

    if (ctrl_iface == NULL) {
        return -1;
    }

    flen = strlen(CTRL_IFACE_DIR) + strlen(ctrl_iface) + 2;
    cfile = (char *) malloc(flen);
    if (cfile == NULL)
        return -1;
    snprintf(cfile, flen, "%s/%s", CTRL_IFACE_DIR, ctrl_iface);

    if (ctrl_conn) {
        wpa_ctrl_close(ctrl_conn);
        ctrl_conn = NULL;
    }

    if (monitor_conn) {
        delete msgNotifier;
        msgNotifier = NULL;
        wpa_ctrl_detach(monitor_conn);
        wpa_ctrl_close(monitor_conn);
        monitor_conn = NULL;
    }

    if(timer) {
        delete timer;
        timer = NULL;
    }

    qdebug("Trying to connect to '%s'", cfile);
    ctrl_conn = wpa_ctrl_open(cfile);
    if (ctrl_conn == NULL) {
        free(cfile);
        return -1;
    }
    monitor_conn = wpa_ctrl_open(cfile);
    free(cfile);
    if (monitor_conn == NULL) {
        wpa_ctrl_close(ctrl_conn);
        return -1;
    }
    if (wpa_ctrl_attach(monitor_conn)) {
        qdebug("Failed to attach to wpa_supplicant");
        wpa_ctrl_close(monitor_conn);
        monitor_conn = NULL;
        wpa_ctrl_close(ctrl_conn);
        ctrl_conn = NULL;
        return -1;
    }

    msgNotifier = new QSocketNotifier(wpa_ctrl_get_fd(monitor_conn),
                      QSocketNotifier::Read, q_ptr);
    q_ptr->connect(msgNotifier, SIGNAL(activated(int)), SLOT(receiveMsgs()));

    timer = new QBasicTimer();
    timer->start(PING_INTERVAL, q_ptr);

    return 0;

}

/***************************************************************************************************
 * setNetworkParam : set the network parameters of specific network
 **************************************************************************************************/
int WpaCtrl::setNetworkParam(int id, const char *field, const char *value, bool quote)
{
    Q_D(WpaCtrl);
    if(!isValid())
    {
        return -1;
    }
    char reply[10], cmd[256];
    size_t reply_len;
    snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s %s%s%s",
         id, field, quote ? "\"" : "", value, quote ? "\"" : "");
    reply_len = sizeof(reply);
    d->ctrlRequest(cmd, reply, &reply_len);
    return strncmp(reply, "OK", 2) == 0 ? 0 : -1;
}

/***************************************************************************************************
 * receiveMsgs : handle monitor connection data receive signal
 **************************************************************************************************/
void WpaCtrlPrivate::receiveMsgs()
{
    char buf[256];
    size_t len;

    while (monitor_conn && wpa_ctrl_pending(monitor_conn) > 0) {
        len = sizeof(buf) - 1;
        if (wpa_ctrl_recv(monitor_conn, buf, &len) == 0) {
            buf[len] = '\0';
            //qdebug(buf);
            processMsg(buf);
        }
    }
}

/***************************************************************************************************
 * str_match: string compare with the length of string @b
 **************************************************************************************************/
static int str_match(const char *a, const char *b)
{
    return strncmp(a, b, strlen(b)) == 0;
}

/***************************************************************************************************
 * processMsg : handle wpa contrl interface event messages
 **************************************************************************************************/
void WpaCtrlPrivate::processMsg(char *msg)
{
    char *pos = msg;

    if (*pos == '<') {
        /* skip priority */
        pos++;
        pos = strchr(pos, '>');
        if (pos)
            pos++;
        else
            pos = msg;
    }

    pingsToStatusUpdate = 0;

    if (str_match(pos, WPA_CTRL_REQ))
    {

    }
    else if (str_match(pos, WPA_EVENT_SCAN_RESULTS))
    {
        updateScanResults();
    }
    else if (str_match(pos, WPA_EVENT_DISCONNECTED))
    {
        //TODO
    }
    else if (str_match(pos, WPA_EVENT_CONNECTED))
    {
        //TODO
    }
}

/***************************************************************************************************
 * ping : ping wpa supplicant, use to keep alive
 **************************************************************************************************/
void WpaCtrlPrivate::ping()
{
    char buf[10];
    size_t len;

    len = sizeof(buf) - 1;
    if (ctrlRequest("PING", buf, &len) < 0) {
        qdebug("PING failed - trying to reconnect");
        if (openCtrlConnection(ctrl_iface) >= 0) {
            qdebug("Reconnected successfully");
            pingsToStatusUpdate = 0;
        }
    }

    pingsToStatusUpdate--;
    if (pingsToStatusUpdate <= 0) {
        updateStatus();
    }
}

/***************************************************************************************************
 * triggerUpdate : trigger a status update
 **************************************************************************************************/
void WpaCtrlPrivate::triggerUpdate()
{
    updateStatus();
}

/***************************************************************************************************
 * updateScanResults : update scan results
 **************************************************************************************************/
void WpaCtrlPrivate::updateScanResults()
{
    char reply[2048];
    size_t reply_len;
    int index;
    char cmd[20];

    scanResultReady = true;
    scanResults.clear();

    index = 0;
    while (true)
    {
        snprintf(cmd, sizeof(cmd), "BSS %d", index++);
        if (index > 1000)
            break;

        reply_len = sizeof(reply) - 1;
        if (ctrlRequest(cmd, reply, &reply_len) < 0)
            break;
        reply[reply_len] = '\0';

        QString bss(reply);
        if (bss.isEmpty() || bss.startsWith("FAIL"))
            break;

        QString ssid, bssid,flags;
        int freq = 0, signal = 0;

        QStringList lines = bss.split(QRegExp("\\n"));
        for (QStringList::Iterator it = lines.begin();
             it != lines.end(); it++) {
            int pos = (*it).indexOf('=') + 1;
            if (pos < 1)
                continue;

            if ((*it).startsWith("bssid="))
                bssid = (*it).mid(pos);
            else if ((*it).startsWith("freq="))
                freq = (*it).mid(pos).toInt();
            else if ((*it).startsWith("level="))
                signal = (*it).mid(pos).toInt();
            else if ((*it).startsWith("flags="))
                flags = (*it).mid(pos);
            else if ((*it).startsWith("ssid="))
                ssid = (*it).mid(pos);
        }

        if (bssid.isEmpty())
            break;
        scanResults.append(SSIDInfo(ssid, bssid, flags, freq, signal));
    }
    requestToScan = SCAN_PERIOD;
}

/***************************************************************************************************
 * updateStatus : update wifi status
 **************************************************************************************************/
void WpaCtrlPrivate::updateStatus()
{
    Q_Q(WpaCtrl);
    char buf[2048], *start, *end, *pos;
    size_t len;

    pingsToStatusUpdate = 10;

    len = sizeof(buf) - 1;
    if (ctrl_conn == NULL || ctrlRequest("STATUS", buf, &len) < 0) {
        qDebug()<<Q_FUNC_INFO<<"Could not get status from wpa_supplicant";
        return;
    }

    buf[len] = '\0';

    //qDebug()<<Q_FUNC_INFO<<buf;

    QString lastSSID = status.ssid;
    QString lastWpaSate = status.wpa_state;

    status.ssid.clear();
    status.bssid.clear();
    status.freq.clear();
    status.id.clear();
    status.mode.clear();
    status.pairwise_cipher.clear();
    status.group_cipher.clear();
    status.address.clear();
    status.wpa_state.clear();
    status.key_mgmt.clear();

    start = buf;
    while (*start) {
        bool last = false;
        end = strchr(start, '\n');
        if (end == NULL) {
            last = true;
            end = start;
            while (end[0] && end[1])
                end++;
        }
        *end = '\0';

        pos = strchr(start, '=');
        if (pos) {
            *pos++ = '\0';
            if (strcmp(start, "bssid") == 0) {
                status.bssid = pos;
            } else if (strcmp(start, "ssid") == 0) {
                status.ssid = pos;
            } else if (strcmp(start, "ip_address") == 0) {
                status.ip_address = pos;
            } else if (strcmp(start, "wpa_state") == 0) {
                status.wpa_state = pos;
            } else if (strcmp(start, "key_mgmt") == 0) {
                status.key_mgmt = pos;
                /* TODO: could add EAP status to this */
            } else if (strcmp(start, "pairwise_cipher") == 0) {
                status.pairwise_cipher = pos;
            } else if (strcmp(start, "group_cipher") == 0) {
                status.group_cipher = pos;
            } else if (strcmp(start, "mode") == 0) {
                status.mode = pos;
            }
        }

        if (last)
            break;
        start = end + 1;
    }

    if(lastWpaSate != "COMPLETED" && status.wpa_state == "COMPLETED")
    {
        emit q->connectToAp(status.ssid);
        qdebug("Connected");
    }
    else if(lastWpaSate == "COMPLETED" && status.wpa_state != "COMPLETED")
    {
        emit q->disconnectFromAp(lastSSID);
        requestToScan = SCAN_PERIOD;
        qdebug("Disconnected");
    }
}

/***************************************************************************************************
 * startWpaSupplicat : start wpa_supplicant process
 **************************************************************************************************/
void WpaCtrlPrivate::startWpaSupplicant(const char *ifname)
{
    if(QFile::exists(QString("%1/%2").arg(CTRL_IFACE_DIR).arg(ifname)))
    {
        qDebug()<<Q_FUNC_INFO<<"wpa_supplicant is already running, skip...";
    }
    else
    {
        QString cmd = QString("wpa_supplicant -B -c %1 -D nl80211 -i %2 -s").arg(WPA_SUPPLICANT_CONF_PATH).arg(ifname);
        if(QProcess::execute(cmd) != 0)
        {
            qDebug()<<Q_FUNC_INFO<<"wpa_supplicant failed to start...";
        }
        usleep(100*1000);
    }
}

/***************************************************************************************************
 * terminate : kill wpa_supplicant
 **************************************************************************************************/
void WpaCtrlPrivate::terminate()
{
    QString cmd("TERMINATE");
    char reply[10];
    size_t reply_len = sizeof(reply);
    ctrlRequest(cmd.toAscii().constData(), reply, &reply_len);
}

/***************************************************************************************************
 * destructor
 **************************************************************************************************/
WpaCtrl::~WpaCtrl()
{
    //kill wpa_supplicant
    Q_D(WpaCtrl);
    d->terminate();
}

/***************************************************************************************************
 * openInterface : open the wpa control interface
 **************************************************************************************************/
int WpaCtrl::openInterface(const QString &interface)
{
    Q_D(WpaCtrl);
    return d->openCtrlConnection(interface.toAscii().constData());
}

/***************************************************************************************************
 * isValid : check whether the wpa control interface is valid
 **************************************************************************************************/
bool WpaCtrl::isValid() const
{
    Q_D(const WpaCtrl);
    return d->ctrl_conn != NULL;
}


/***************************************************************************************************
 * getNetworks : get stored network configurations
 **************************************************************************************************/
QList<WiFiNetworkInfo> WpaCtrl::getNetworks()
{
    Q_D(WpaCtrl);
    QList<WiFiNetworkInfo> infos;
    char buf[2048], *start, *end, *id, *ssid, *bssid, *flags;
    size_t len;

    if (d->ctrl_conn == NULL)
        return infos;

    len = sizeof(buf) - 1;
    if (d->ctrlRequest("LIST_NETWORKS", buf, &len) < 0)
        return infos;

    buf[len] = '\0';
    start = strchr(buf, '\n');
    if (start == NULL)
        return infos;
    start++;

    while (*start) {
        bool last = false;
        end = strchr(start, '\n');
        if (end == NULL) {
            last = true;
            end = start;
            while (end[0] && end[1])
                end++;
        }
        *end = '\0';

        id = start;
        ssid = strchr(id, '\t');
        if (ssid == NULL)
            break;
        *ssid++ = '\0';
        bssid = strchr(ssid, '\t');
        if (bssid == NULL)
            break;
        *bssid++ = '\0';
        flags = strchr(bssid, '\t');
        if (flags == NULL)
            break;
        *flags++ = '\0';

        if (strstr(flags, "[DISABLED][P2P-PERSISTENT]")) {
            if (last)
                break;
            start = end + 1;
            continue;
        }

        infos.append(WiFiNetworkInfo(id, ssid, bssid, flags));

        if (last)
            break;
        start = end + 1;
    }

    return infos;
}

/***************************************************************************************************
 * enableNetwork : enable network by @id
 **************************************************************************************************/
void WpaCtrl::enableNetwork(const QString &id)
{
    Q_D(WpaCtrl);
    QString cmd(id);
    char reply[10];
    size_t reply_len = sizeof(reply);
    if(!isValid())
    {
        return;
    }

    if (!cmd.contains(QRegExp("^\\d+$")) && !cmd.startsWith("all"))
    {
        qdebug("Invalid editNetwork '%s'",
              cmd.toAscii().constData());
        return;
    }
    cmd.prepend("ENABLE_NETWORK ");
    d->ctrlRequest(cmd.toAscii().constData(), reply, &reply_len);
    d->triggerUpdate();
}

/***************************************************************************************************
 * disableNetwork : disable network by @id
 **************************************************************************************************/
void WpaCtrl::disableNetwork(const QString &id)
{
    Q_D(WpaCtrl);
    QString cmd(id);
    char reply[10];
    size_t reply_len = sizeof(reply);
    if(!isValid())
    {
        return;
    }

    if (!cmd.contains(QRegExp("^\\d+$")) && !cmd.startsWith("all"))
    {
        qdebug("Invalid editNetwork '%s'",
              cmd.toAscii().constData());
        return;
    }
    cmd.prepend("DISABLE_NETWORK ");
    d->ctrlRequest(cmd.toAscii().constData(), reply, &reply_len);
    d->triggerUpdate();
}

/***************************************************************************************************
 * selectNetwork : select network as current network
 **************************************************************************************************/
void WpaCtrl::selectNetwork(const QString &id)
{
    Q_D(WpaCtrl);
    QString cmd(id);
    char reply[10];
    size_t reply_len = sizeof(reply);
    if(!isValid())
    {
        return;
    }

    if (!cmd.contains(QRegExp("^\\d+$")))
        cmd = "any";
    cmd.prepend("SELECT_NETWORK ");
    d->ctrlRequest(cmd.toAscii().constData(), reply, &reply_len);
    d->triggerUpdate();
}

/***************************************************************************************************
 * addNetwork : add a wifi netowrk
 **************************************************************************************************/
QString WpaCtrl::addNetwork()
{
    Q_D(WpaCtrl);
    QString cmd("ADD_NETWORK");
    char reply[10];
    size_t reply_len = sizeof(reply);
    if(!isValid())
    {
        return QString();
    }
    d->ctrlRequest(cmd.toAscii().constData(), reply, &reply_len);
    reply[reply_len] = '\0';
    d->triggerUpdate();
    QString id(reply);
    id = id.trimmed();
    if(!id.contains(QRegExp("^\\d+$")))
    {
        return QString();
    }
    return id;
}

/***************************************************************************************************
 * removeNetowrk : remove network
 **************************************************************************************************/
void WpaCtrl::removeNetwork(const QString &id)
{
    Q_D(WpaCtrl);
    QString cmd(id);
    char reply[10];
    size_t reply_len = sizeof(reply);
    if(!isValid())
    {
        return;
    }

    if (cmd.contains(QRegExp("^\\d+$")) && !cmd.startsWith("all"))
    {
        debug("Invalid editNetwork '%s'",
              cmd.toAscii().constData());
        return;
    }
    cmd.prepend("REMOVE_NETWORK ");
    d->ctrlRequest(cmd.toAscii().constData(), reply, &reply_len);
    d->triggerUpdate();
}

/***************************************************************************************************
 * scanRequest : peform a scan
 **************************************************************************************************/
void WpaCtrl::scanRequest()
{
    Q_D(WpaCtrl);
    char reply[10];
    size_t reply_len = sizeof(reply);
    if(!isValid())
    {
        return;
    }
    d->ctrlRequest("SCAN", reply, &reply_len);
    d->scanResultReady = false;
}

/***************************************************************************************************
 * scanResultReady : check whether scan result is ready
 **************************************************************************************************/
bool WpaCtrl::scanResultReady() const
{
    Q_D(const WpaCtrl);
    return d->scanResultReady;
}

/***************************************************************************************************
 * getScanResult : get the scan result
 **************************************************************************************************/
QList<SSIDInfo> WpaCtrl::getScanResult() const
{
    Q_D(const WpaCtrl);
    return d->scanResults;
}

/***************************************************************************************************
 * saveConfig : save the network config
 **************************************************************************************************/
void WpaCtrl::saveConfig()
{
    Q_D(WpaCtrl);
    char reply[10];
    size_t reply_len = sizeof(reply);
    d->ctrlRequest("SAVE_CONFIG", reply, &reply_len);
    reply[reply_len] = '\0';
    if(str_match(reply, "FAIL"))
    {
        qDebug()<<Q_FUNC_INFO<<"The config can not be saved. Check update_config configuration.";
    }
    else
    {
        qDebug()<<Q_FUNC_INFO<<"Config saved.";
    }

}

/***************************************************************************************************
 * status : get the latest network status
 **************************************************************************************************/
WiFiStatus WpaCtrl::status() const
{
    Q_D(const WpaCtrl);
    return d->status;
}

/***************************************************************************************************
 * timerEvent : period timer event, period will be @PING_INTERVAL seconds
 **************************************************************************************************/
void WpaCtrl::timerEvent(QTimerEvent *e)
{
    Q_D(WpaCtrl);
    if(e->timerId() != d->timer->timerId())
    {
        return;
    }

    d->ping();

    d->requestToScan--;
    if(d->requestToScan == 0 && d->status.ssid.isEmpty())
    {
        //in disconnect state, request a scan. if found the select ap, wpa_supplicant will reconnected
        QList<WiFiNetworkInfo> infos = getNetworks();
        bool hasAcitveNetwork = false;
        for(int i=0; i< infos.count(); i++)
        {
            if(infos.at(i).flags.contains("[DISABLED]"))
            {
                continue;
            }
            else
            {
                hasAcitveNetwork = true;
                break;
            }
        }
        if(hasAcitveNetwork && !d->status.wpa_state.contains("DISCONNECTED"))
        {
            scanRequest();
        }
    }
}

/***************************************************************************************************
 * constructor
 **************************************************************************************************/
WpaCtrl::WpaCtrl(QObject *parent=NULL)
    :QObject(parent), d_ptr(new WpaCtrlPrivate(this))
{
}

#include "moc_WpaCtrl.cpp"
