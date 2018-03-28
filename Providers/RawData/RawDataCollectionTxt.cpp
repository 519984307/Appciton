#include "Debug.h"
#include "RawDataCollectionTxt.h"
#include <QMutexLocker>

#include "TimeManager.h"
#include "TimeDate.h"
#include "USBManager.h"
#include <QDateTime>

#if defined(Q_WS_QWS)

RawDataCollectionTxt *RawDataCollectionTxt::_selfObj = NULL;
/**************************************************************************************************
 * 向缓冲区中写入数据。
 *************************************************************************************************/
void RawDataCollectionTxt::PushData(QString name, const unsigned char *data, int len)
{
    //检查U盘
    if (!usbManager.isUSBExist())
    {
        return;
    }

    //判断缓冲区是否创建
    _buffCreate(name);

    FileData* channel = _provider.value(name);



    channel->name = name;

    char *buf = (char *) malloc(len);
    memcpy(buf, data, len);
    unsigned char *a = (unsigned char *)buf;
    BuffItem item(name,a,len);

    channel->_mutex.lock();
    channel->_provideList[channel->currentItemList].append(item);
    channel->buff1++;
    channel->_mutex.unlock();
}

/**************************************************************************************************
 * 打开U盘文件。
 *************************************************************************************************/
void RawDataCollectionTxt::openFile(FileData* channel)
{
    if (_folderCreate(channel))
    {
        channel->_txtOpen = true;
        channel->_textStream = new QTextStream(channel->_pfile);
        channel->_data_1 = 0;
        channel->_data_2 = 0;
        channel->_numData = 0;
    }
    else
    {
        channel->_txtOpen = false;
    }
}

/**************************************************************************************************
 * 关闭txt文件。
 *************************************************************************************************/
void RawDataCollectionTxt::closeFile(FileData* channel)
{
    if (!channel->_txtOpen)
    {
        return;
    }

    channel->_txtOpen = false;
    channel->_pfile->flush();
    *channel->_textStream << dec << channel->_data_1 << "\r\n";
    *channel->_textStream << dec << channel->_data_2 << "\r\n";
    channel->_pfile->close();
    delete channel->_pfile;
    debug("%s File Close!\n",qPrintable(channel->name));
}

/**************************************************************************************************
 * buff是否存在。
 *************************************************************************************************/
void RawDataCollectionTxt::_buffCreate(QString name)
{
    ProviderMap::iterator it = _provider.find(name);
    // 已经存在。
    if (it != _provider.end())
    {
        return;
    }
    _provider.insert(name, new FileData() );
}

/**************************************************************************************************
 * buff双缓冲切换。
 *************************************************************************************************/
void RawDataCollectionTxt::_switchList(FileData* channel)
{
    channel->_mutex.lock();
//    debug("%d,%d\n",channel->buff1,channel->buff2);
    channel->currentItemList = !channel->currentItemList;
    channel->buff1 = 0;
    channel->buff2 = 0;
    channel->_mutex.unlock();
}

/**************************************************************************************************
 * 文件夹的创建。
 *************************************************************************************************/
bool RawDataCollectionTxt::_folderCreate(FileData* channel)
{
    QDir *temp = new QDir;

    channel->_folderPath = usbManager.getUdiskMountPoint();
    channel->_folderPath += "/";
    channel->_folderPath += channel->name;

    bool exist = temp->exists(channel->_folderPath);
    if(exist)
    {
        debug("%s Folder Exist!\n",qPrintable(channel->name));
        return _openTxt(channel);
    }
    else
    {
        bool folderOK = temp->mkdir(channel->_folderPath);
        if (folderOK)
        {
            debug("%s Folder mkdir is OK!\n",qPrintable(channel->name));
            return _openTxt(channel);
        }
        else
        {
            debug("%s Folder mkdir is Failed!\n",qPrintable(channel->name));
            return false;
        }
    }
}

/**************************************************************************************************
 * 打开U盘文件。
 *************************************************************************************************/
bool RawDataCollectionTxt::_openTxt(FileData* channel)
{
    unsigned time = timeManager.getCurTime();

    //文件名，以时间命名
    QDateTime dt = QDateTime::fromTime_t(time);
//    channel->_fileName.sprintf("%02d.%02d.%02d", timeDate.getTimeHour(time), timeDate.getTimeMinute(time),
//                 timeDate.getTimeSenonds(time));
    channel->_fileName = dt.toString("yyyy.MM.dd-HH.mm.ss");

    channel->_filePath = channel->_folderPath;
    channel->_filePath += "/";
    channel->_filePath += channel->name + "_" + channel->_fileName + ".txt";
    channel->_pfile = new QFile(channel->_filePath);

    if (channel->_pfile->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        return true;
    }
    else if (!channel->_pfile->open(QIODevice::WriteOnly))
    {
        //如果文件不存在就创建
        channel->_pfile->close();
        if (channel->_pfile->open(QIODevice::ReadWrite | QIODevice::Text))
        {
            return true;
        }
        else
        {
            debug("%s open wrong!\n", qPrintable(channel->_filePath));
            return false;
        }
    }
    else
        return false;
}

/**************************************************************************************************
 * 保存数据到U盘。
 *************************************************************************************************/
void RawDataCollectionTxt::_saveData(void)
{
    BuffItem item;
    ProviderMap::iterator iter;
    for(iter = _provider.begin(); iter != _provider.end(); iter++)
    {
        FileData* channel = iter.value();

        //flush item
        while(!channel->_provideList[!channel->currentItemList].isEmpty())
        {
            item = channel->_provideList[!channel->currentItemList].takeFirst();
            exportUdisk(channel, item.data, item.length);
            free(item.data);
        }
        //switch cache list
        _switchList(channel);
    }
}

/**************************************************************************************************
 * 将数据写入TXT文件。
 *************************************************************************************************/
void RawDataCollectionTxt::exportUdisk(FileData* channel, unsigned char *data, int len)
{
    //检查U盘
    if (!usbManager.isUSBExist())
    {
        return;
    }
    if (channel->_txtOpen)
    {
        unsigned time = timeManager.getCurTime();
        QString _data = _data.sprintf("%02d:%02d:%02d", timeDate.getTimeHour(time), timeDate.getTimeMinute(time),timeDate.getTimeSenonds(time));
        *channel->_textStream << dec << _data << "\t";

        //charge num
        //*channel->_textStream << dec << pdDefibInfo.getChargeNum() << "\t";

        //导联信息采集
        for(int i = 0; i < len; i++)
        {
            *channel->_textStream << dec << data[i]<< "\t";
        }
        *channel->_textStream << dec << channel->_data_2 << "\t";
        *channel->_textStream << "\r\n";
        channel->_textStream->flush();
        //心电波形采集
//        for(int i = 2; i < len; i++)
//        {
//            *_textStream << dec << data[i]<< "\t";
//            _numData++;
//            if (_numData > 25)
//                {
//                _numData = 0;
//                *_textStream << dec << _data_2 << "\t";
//                *_textStream << "\r\n";
//            }
//        }

        channel->buff2++;
        channel->_data_2++;
        int data;
//        if (channel->name == "BLM_TE3")
//        {
//            data = 10000;
//        }
//        else
//        {
//            data = 10;
//        }
        data = 1024;
        if (channel->_data_2 >= data)
        {
            closeFile(channel);
            if (_openTxt(channel))
            {
                channel->_txtOpen = true;
                channel->_textStream = new QTextStream(channel->_pfile);
                channel->_data_1 = 0;
                channel->_data_2 = 0;
            }
            else
            {
                channel->_txtOpen = false;
            }
        }
    }
    else
    {
        openFile(channel);
    }
}

/***************************************************************************************************
 * 构造函数
 **************************************************************************************************/
RawDataCollectionTxt::RawDataCollectionTxt()
{
}

/***************************************************************************************************
 * 析构函数
 **************************************************************************************************/
RawDataCollectionTxt::~RawDataCollectionTxt()
{
    ProviderMap::iterator iter;
    for(iter = _provider.begin(); iter != _provider.end(); iter++)
    {
        FileData* channel = iter.value();
        channel->_mutex.lock();
        channel->_provideList[channel->currentItemList].append(channel->_provideList[!channel->currentItemList]);
        channel->_mutex.unlock();
        _saveData();

        closeFile(channel);
    }
}

void RawDataCollectionTxt::run()
{
    _saveData();
}

#elif defined(Q_WS_X11)
RawDataCollectionTxt *RawDataCollectionTxt::_selfObj = NULL;
void RawDataCollectionTxt::PushData(QString /*name*/, const unsigned char */*data*/, int /*len*/) {}

void RawDataCollectionTxt::run() { }
RawDataCollectionTxt::RawDataCollectionTxt() { }
RawDataCollectionTxt::~RawDataCollectionTxt() { }

#endif
