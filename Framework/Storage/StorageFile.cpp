/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/1
 **/

#include "StorageFile.h"
#include <QVector>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include <unistd.h>

#define STORAGE_FILE_SUFFIX QString::fromLatin1(".seq")
#define BACKUP_FILE_SUFFIX QString::fromLatin1(".bak")

#define FILE_HEADER_SIZE sizeof(FileHeader)
#define BLOCK_ENTRY_SIZE sizeof(BlockEntry)

// large enough ???
#define MAX_BLOCK_NUM 1000000

struct FileHeader
{
    FileHeader()
        : fileType(StorageFile::Type), reservedAreaOffset(FILE_HEADER_SIZE), reservedAreaSize(0),
          blockEntryListOffset(reservedAreaOffset + reservedAreaSize), blockNumber(0), blockEntryListChecksum(0)
    {}

    bool isValid()
    {
        return fileType == StorageFile::Type
               && (reservedAreaOffset == 0 || reservedAreaOffset >= FILE_HEADER_SIZE)
               && blockEntryListOffset >= FILE_HEADER_SIZE
               && blockEntryListOffset >= reservedAreaOffset
               && blockNumber <= MAX_BLOCK_NUM;
    }

    quint32 fileType;
    quint32 reservedAreaOffset;
    quint32 reservedAreaSize;
    quint32 blockEntryListOffset;
    quint32 blockNumber;
    quint32 blockEntryListChecksum;
};

class StorageFilePrivate
{
public:
    Q_DECLARE_PUBLIC(StorageFile)
    explicit StorageFilePrivate(StorageFile *const q_ptr)
        : q_ptr(q_ptr), isValid(false), dataChanged(false) {}

    void sync();
    bool parseOrCreateStorageFile();

    quint32 write(const char *data, quint32 pos, quint32 len);
    quint32 read(char *data, quint32 pos, quint32 len);
    quint32 calcBlockEntryListChecksum(const QVector<BlockEntry> &entryList);
    bool backupHeader();
    bool backupBlockEntryList(bool append);
    void backup(bool append = false);

    bool updateFileHeader();
    bool updateBlockEntryList();

    StorageFile *const q_ptr;
    QVector<BlockEntry> blockEntryList;
    QFile storageFile;
    QFile backupFile;
    FileHeader fileHeader;
    bool isValid;
    bool dataChanged;
    QMutex fileMutex;
    QMutex entryListMutex;
};

/***************************************************************************************************
 * sync : update file header and block entry list into file
 **************************************************************************************************/
void StorageFilePrivate::sync()
{
    if (dataChanged)
    {
        // write to storage file
        if (!updateBlockEntryList())
        {
            qDebug() << Q_FUNC_INFO << "Write block entry list failed!";
        }
        if (!updateFileHeader())
        {
            qDebug() << Q_FUNC_INFO << "Write File header failed!";
        }
    }

    storageFile.flush();
    fsync(storageFile.handle());
    dataChanged = false;
}

/***************************************************************************************************
 * parseOrCreateStorageFile : parse storage file or create storage file
 **************************************************************************************************/
bool StorageFilePrivate::parseOrCreateStorageFile()
{
    FileHeader header;
    bool existBackup = backupFile.exists();
    bool writable = storageFile.openMode() & QIODevice::WriteOnly;
    bool parseSuccess = false;
    bool parseBackupSuccess = false;
    quint32 readLen = this->read(reinterpret_cast<char *>(&header), 0, FILE_HEADER_SIZE);

    // File might be empty or new create or a wrong file,
    // we will resize the file if filesize is less than FILE_HEADER_SIZE
    if (readLen < FILE_HEADER_SIZE)
    {
        // failed and file is no opened or read only
        if (!writable)
        {
            return false;
        }

        // try to create storage file
        if (!storageFile.resize(0))
        {
            qDebug() << Q_FUNC_INFO << "Fail to resize" << storageFile.fileName()
                     << "," << storageFile.errorString();
            return false;
        }

        dataChanged = true;
        // write file header
        sync();
    }
    else if (!header.isValid())
    {
        qDebug() << Q_FUNC_INFO << "File format invalid";
        return false;
    }
    else
    {
        // try to load block entry list from storage file and verify
        QVector<BlockEntry> list(header.blockNumber);
        quint32 readLen = header.blockNumber * BLOCK_ENTRY_SIZE;

        if (readLen == this->read(reinterpret_cast<char *>(list.data()), header.blockEntryListOffset, readLen))
        {
            // calculate checksum
            if (header.blockEntryListChecksum == calcBlockEntryListChecksum(list))
            {
                blockEntryList = list;
                fileHeader = header;
                parseSuccess = true;
            }
        }

        // parse backup file, try to recover from backup file
        if (existBackup)
        {
            if (backupFile.open(QIODevice::ReadOnly))
            {
                FileHeader headerBackup;
                if (FILE_HEADER_SIZE == backupFile.read(reinterpret_cast<char *>(&headerBackup), FILE_HEADER_SIZE)
                        && headerBackup.isValid())
                {
                    QVector<BlockEntry> listBackup(headerBackup.blockNumber);
                    quint32 readLen = headerBackup.blockNumber * BLOCK_ENTRY_SIZE;
                    if (readLen == backupFile.read(reinterpret_cast<char *>(listBackup.data()), readLen))
                    {
                        // calculate checksum;
                        if (headerBackup.blockEntryListChecksum == calcBlockEntryListChecksum(listBackup))
                        {
                            blockEntryList = listBackup;
                            fileHeader = headerBackup;
                            parseBackupSuccess = true;
                            dataChanged = true;
                        }
                    }

                    if (!parseBackupSuccess)
                    {
                        if (header.blockNumber != headerBackup.blockNumber
                                || header.blockEntryListOffset != headerBackup.blockEntryListOffset
                                || header.blockEntryListChecksum != headerBackup.blockEntryListChecksum)
                        {
                            // try to parse with the file header of the original file
                            qDebug() << Q_FUNC_INFO
                                     << "Parse backup file failed, try to restore file with the original file header";
                            backupFile.seek(FILE_HEADER_SIZE);
                            listBackup.resize(header.blockNumber);
                            readLen = header.blockNumber * BLOCK_ENTRY_SIZE;
                            if (readLen == backupFile.read(reinterpret_cast<char *>(listBackup.data()), readLen)
                                    && header.blockEntryListChecksum == calcBlockEntryListChecksum(listBackup))
                            {
                                blockEntryList = listBackup;
                                fileHeader = header;
                                parseBackupSuccess = true;
                                dataChanged = true;
                            }
                            qDebug() << Q_FUNC_INFO << "Restore file success";
                        }
                    }
                }
                backupFile.close();
            }
        }

        if (!parseBackupSuccess && !parseSuccess)
        {
            qDebug() << Q_FUNC_INFO << "broken file:" << storageFile.fileName();
            return false;
        }
    }

    if (writable)
    {
        if (!backupFile.open(QIODevice::ReadWrite))
        {
            qDebug() << Q_FUNC_INFO << "Open or create backup file failed!";
        }

        if (parseBackupSuccess)
        {
            // no need to sync
        }
        else
        {
            backup();
        }
    }

    return true;
}

/***************************************************************************************************
 * calcBlockEntryListchecksum : calculate the checksum of block entry list
 **************************************************************************************************/
quint32 StorageFilePrivate::calcBlockEntryListChecksum(const QVector<BlockEntry> &entryList)
{
    QVector<BlockEntry>::ConstIterator iter;
    quint32 sum = 0;
    for (iter = entryList.constBegin(); iter != entryList.constEnd(); iter++)
    {
        sum += iter->length;
        sum += iter->offset;
        sum += iter->type;
        sum += iter->extraData;
    }
    return sum;
}

/***************************************************************************************************
 * backupHeader : write the file header to the backup file
 **************************************************************************************************/
bool StorageFilePrivate::backupHeader()
{
    if (backupFile.isOpen())
    {
        if (backupFile.seek(0))
        {
            return FILE_HEADER_SIZE == backupFile.write(reinterpret_cast<char *>(&fileHeader), FILE_HEADER_SIZE);
        }
    }
    return false;
}

/***************************************************************************************************
 * backupBlockentry :  write the block entry list to backup file
 * @append :append one entry to the end or write all entries
 **************************************************************************************************/
bool StorageFilePrivate::backupBlockEntryList(bool append)
{
    if (fileHeader.blockNumber <= 0)
    {
        return true;
    }

    if (backupFile.isOpen())
    {
        QMutexLocker locker(&entryListMutex);
        if (append)
        {
            if (backupFile.seek(FILE_HEADER_SIZE + BLOCK_ENTRY_SIZE * (fileHeader.blockNumber - 1)))
            {
                return BLOCK_ENTRY_SIZE == backupFile.write((const char *)&blockEntryList.last(), BLOCK_ENTRY_SIZE);
            }
        }
        else
        {
            if (backupFile.seek(FILE_HEADER_SIZE))  // right after file header
            {
                int writeLen = BLOCK_ENTRY_SIZE * fileHeader.blockNumber;
                return writeLen == backupFile.write((const char *)blockEntryList.constData(), writeLen);
            }
        }
    }
    return false;
}

/***************************************************************************************************
 * backup : backup data to backup file
 **************************************************************************************************/
void StorageFilePrivate::backup(bool append)
{
    if (!backupBlockEntryList(append) || !backupHeader())
    {
        qDebug() << Q_FUNC_INFO << "Write backup file failed!";
        return;
    }
    backupFile.flush();
    fsync(backupFile.handle());
}

/***************************************************************************************************
 * write : Write data to the specific offset of the opened storage file.
 **************************************************************************************************/
quint32 StorageFilePrivate::write(const char *data, quint32 pos, quint32 len)
{
    if (data == NULL || len == 0)
    {
        return 0;
    }

    if (!storageFile.isOpen())
    {
        qDebug() << Q_FUNC_INFO << "File is not opened!";
        return 0;
    }

    if (storageFile.openMode() == QIODevice::ReadOnly)
    {
        qDebug() << Q_FUNC_INFO << "File is readonly!";
        return 0;
    }

    QMutexLocker locker(&fileMutex);
    if (!storageFile.seek(pos))
    {
        qDebug() << Q_FUNC_INFO << storageFile.fileName() << "seek failed!";
        return 0;
    }

    int writeLen = storageFile.write(data, len);
    if (writeLen < 0)
    {
        qDebug() << Q_FUNC_INFO << storageFile.fileName() << "write failed:"
                 << storageFile.errorString();
        return 0;
    }

    return writeLen;
}

/***************************************************************************************************
 * read : Read data from specific offset of the opened storage file
 **************************************************************************************************/
quint32 StorageFilePrivate::read(char *data, quint32 pos, quint32 len)
{
    if (data == NULL || len == 0)
    {
        return 0;
    }

    if (!storageFile.isOpen())
    {
        qDebug() << Q_FUNC_INFO << "File is not opened!";
        return 0;
    }

    if (storageFile.openMode() == QIODevice::WriteOnly)
    {
        qDebug() << Q_FUNC_INFO << "File is WriteOnly!";
        return 0;
    }


    QMutexLocker locker(&fileMutex);
    if (!storageFile.seek(pos))
    {
        qDebug() << Q_FUNC_INFO << storageFile.fileName() << "seek failed!";
        return 0;
    }

    int readLen = storageFile.read(data, len);
    if (readLen < 0)
    {
        qDebug() << Q_FUNC_INFO << storageFile.fileName() << "read failed:"
                 << storageFile.errorString();
        return 0;
    }
    return readLen;
}

/***************************************************************************************************
 * updateFileHeader : write the file header to the storage file
 **************************************************************************************************/
bool StorageFilePrivate::updateFileHeader()
{
    return FILE_HEADER_SIZE == this->write(reinterpret_cast<char *>(&fileHeader), 0, FILE_HEADER_SIZE);
}

/***************************************************************************************************
 * updateBlockEntryList : write the block entry list into the storage file
 **************************************************************************************************/
bool StorageFilePrivate::updateBlockEntryList()
{
    quint32 writeLen = fileHeader.blockNumber * BLOCK_ENTRY_SIZE;
    QMutexLocker locker(&entryListMutex);
    return writeLen == this->write((const char *) blockEntryList.constData(),
                                   fileHeader.blockEntryListOffset, writeLen);
}

/***************************************************************************************************
 * SeqStorageFile: just create a invalid object, need to reload specific file
 **************************************************************************************************/
StorageFile::StorageFile()
    : d_ptr(new StorageFilePrivate(this))
{
}

/***************************************************************************************************
 * SeqStorageFile : Constructor, use the specific @filename , filename should not contain suffix.
 **************************************************************************************************/
StorageFile::StorageFile(const QString &filename, QIODevice::OpenMode openmode)
    : d_ptr(new StorageFilePrivate(this))
{
    Q_D(StorageFile);
    if (!filename.isEmpty())
    {
        d->storageFile.setFileName(filename + STORAGE_FILE_SUFFIX);
        d->backupFile.setFileName(d->storageFile.fileName() + BACKUP_FILE_SUFFIX);
        // open or create file
        if (!d->storageFile.open(openmode))
        {
            qDebug() << Q_FUNC_INFO << "fail to open or create" << d->storageFile.fileName()
                     << "," << d->storageFile.errorString();
        }
        else
        {
            d->isValid = d->parseOrCreateStorageFile();
        }
    }
}


/***************************************************************************************************
 * ~SeqStorageFile: deconstructor
 **************************************************************************************************/
StorageFile::~StorageFile()
{
    Q_D(StorageFile);
    if (d->storageFile.isOpen())
    {
        if (d->backupFile.isOpen())
        {
            d->backupFile.close();
        }

        if (d->storageFile.openMode() != QIODevice::ReadOnly)
        {
            d->sync();
            if (d->isValid)
            {
                d->backupFile.remove();
            }
        }
        d->storageFile.close();
    }
}

/***************************************************************************************************
 * fileType : get the type of the storage file
 **************************************************************************************************/
int StorageFile::fileType()
{
    return StorageFile::Type;
}

/***************************************************************************************************
 * fileSize : get the size of storage file
 **************************************************************************************************/
quint32 StorageFile::fileSize()
{
    Q_D(const StorageFile);
    if (d->isValid)
    {
        // ignore backup file size, just return storage file size
        return d->storageFile.size();
    }
    return 0;
}

/***************************************************************************************************
 * isValid : Check whether the storage file is parse success and ready to read/write.
 **************************************************************************************************/
bool StorageFile::isValid()
{
    Q_D(const StorageFile);
    return d->isValid;
}


/***************************************************************************************************
 * setReservedSize : set the reserved size in the storage file, must set before store data
 *                   return true if success
 **************************************************************************************************/
bool StorageFile::setReservedSize(quint32 size)
{
    Q_D(StorageFile);
    if (d->fileHeader.blockNumber > 0 || !d->isValid)
    {
        return false;
    }
    d->fileHeader.reservedAreaSize = size;
    d->fileHeader.reservedAreaOffset = FILE_HEADER_SIZE;
    d->fileHeader.blockEntryListOffset = d->fileHeader.reservedAreaOffset + d->fileHeader.reservedAreaSize;
    d->backupHeader();
    d->backupFile.flush();
    fsync(d->backupFile.handle());
    return true;
}

/***************************************************************************************************
 * getReservedSize : get the reserved size in the storage file
 **************************************************************************************************/
quint32 StorageFile::getReservedSize()
{
    Q_D(const StorageFile);
    return d->fileHeader.reservedAreaSize;
}

/***************************************************************************************************
 * reload : reload a specific storage file
 **************************************************************************************************/
void StorageFile::reload(const QString &filename, QIODevice::OpenMode openmode)
{
    Q_D(StorageFile);
    QString actualFilename = filename + STORAGE_FILE_SUFFIX;

    // close current file
    if (d->storageFile.isOpen())
    {
        if (d->backupFile.isOpen())
        {
            d->backupFile.close();
        }

        if (d->storageFile.openMode() != QIODevice::ReadOnly)
        {
            d->sync();
            d->backupFile.remove();
        }
        d->storageFile.close();
    }

    d->fileHeader = FileHeader();
    d->blockEntryList.clear();
    d->isValid = false;

    if (!filename.isEmpty())
    {
        d->storageFile.setFileName(actualFilename);
        d->backupFile.setFileName(actualFilename + BACKUP_FILE_SUFFIX);
        // open or create file
        if (!d->storageFile.open(openmode))
        {
            qDebug() << Q_FUNC_INFO << "fail to open or create" << d->storageFile.fileName()
                     << "," << d->storageFile.errorString();
        }
        else
        {
            d->isValid = d->parseOrCreateStorageFile();
        }
    }
}

/***************************************************************************************************
 * getBlockNR : get the data block number
 **************************************************************************************************/
quint32 StorageFile::getBlockNR()
{
    Q_D(const StorageFile);
    return d->fileHeader.blockNumber;
}

/***************************************************************************************************
 * writeAdditionalData : write additional data to the reserved area
 **************************************************************************************************/
quint32 StorageFile::writeAdditionalData(const char *data, quint32 len)
{
    Q_D(StorageFile);
    if (data == NULL || len == 0 || !d->isValid)
    {
        return 0;
    }

    int writeLen = d->fileHeader.reservedAreaSize > len ? len : d->fileHeader.reservedAreaSize;

    writeLen = d->write(data, d->fileHeader.reservedAreaOffset, writeLen);

    d->storageFile.flush();
    fsync(d->storageFile.handle());
    return writeLen;
}

/***************************************************************************************************
 * readAdditionalData : read additional data from the reserved area
 **************************************************************************************************/
quint32 StorageFile::readAdditionalData(char *data, quint32 len)
{
    Q_D(StorageFile);
    if (data == NULL || len == 0 || !d->isValid)
    {
        return 0;
    }

    int readLen = d->fileHeader.reservedAreaSize > len ? len : d->fileHeader.reservedAreaSize;
    return d->read(data, d->fileHeader.reservedAreaOffset, readLen);
}

/***************************************************************************************************
 * getBlockDatalen : get the length of block data at specific index , index start form zero
 **************************************************************************************************/
quint32 StorageFile::getBlockDataLen(quint32 index)
{
    Q_D(StorageFile);
    if (!d->isValid || index >= d->fileHeader.blockNumber)
    {
        return 0;
    }

    QMutexLocker locker(&d->entryListMutex);
    return d->blockEntryList.at(index).length;
}

/***************************************************************************************************
 * getBlockData : get the block data of a specific index
 **************************************************************************************************/
QByteArray StorageFile::getBlockData(quint32 index)
{
    quint32 len = getBlockDataLen(index);
    QByteArray result(len, 0);
    this->readBlockData(index, result.data(), len);
    return result;
}

/***************************************************************************************************
 * getBlockData : get the block data type of specific index
 **************************************************************************************************/
quint32 StorageFile::getBlockType(quint32 index)
{
    Q_D(StorageFile);
    if (!d->isValid
            || index >= d->fileHeader.blockNumber)
    {
        return 0;
    }
    QMutexLocker locker(&d->entryListMutex);
    return d->blockEntryList.at(index).type;
}

/***************************************************************************************************
 * getBlockData : get the block data type of specific index
 **************************************************************************************************/
BlockEntry StorageFile::getBlockInfo(quint32 index)
{
    Q_D(StorageFile);
    if (!d->isValid
            || index >= d->fileHeader.blockNumber)
    {
        return BlockEntry();
    }

    QMutexLocker locker(&d->entryListMutex);
    return d->blockEntryList.at(index);
}

/***************************************************************************************************
 * getBlockData : get block entry list
 **************************************************************************************************/
QVector<BlockEntry> StorageFile::getBlockEntryList()
{
    Q_D(StorageFile);
    QMutexLocker locker(&d->entryListMutex);
    return d->blockEntryList;
}

/***************************************************************************************************
 * readBlockData : read block data at specific index, index start from zero
 **************************************************************************************************/
quint32 StorageFile::readBlockData(quint32 index, char *data, quint32 len)
{
    Q_D(StorageFile);
    if (!d->isValid
            || data == NULL
            || len == 0
            || index >= d->fileHeader.blockNumber
            || d->storageFile.openMode() == QIODevice::WriteOnly)
    {
        return 0;
    }
    d->entryListMutex.lock();
    int readLen = d->blockEntryList.at(index).length > len ? len : d->blockEntryList.at(index).length;
    quint32 offset = d->blockEntryList.at(index).offset;
    d->entryListMutex.unlock();
    return d->read(data, offset, readLen);
}

/***************************************************************************************************
 * appendBlockData : append a block to the end of the file
 **************************************************************************************************/
quint32 StorageFile::appendBlockData(quint32 type, const char *data, quint32 len, quint32 extraData)
{
    Q_D(StorageFile);
    if (!d->isValid || data == NULL || len == 0 || d->storageFile.openMode() == QIODevice::ReadOnly)
    {
        return 0;
    }

    BlockEntry entry(type, d->fileHeader.blockEntryListOffset, len, extraData);

    d->fileHeader.blockNumber++;
    d->fileHeader.blockEntryListOffset += len;
    d->fileHeader.blockEntryListChecksum += (entry.offset + entry.length + entry.type + entry.extraData);
    quint32 writeLen = d->write(data, entry.offset, entry.length);
    d->entryListMutex.lock();
    d->blockEntryList.append(entry);
    d->entryListMutex.unlock();
    d->dataChanged = true;

    d->storageFile.flush();  // make sure data is written
    fsync(d->storageFile.handle());

    d->backup(true);

    d->updateFileHeader();

    return writeLen;
}

/***************************************************************************************************
 * writeBlockData : modify  block data at specific index, data len should be equal to the lenght of
 *                  the block, otherwise, write nothing
 **************************************************************************************************/
quint32 StorageFile::modifyBlockData(quint32 index, char *data, quint32 len)
{
    Q_D(StorageFile);
    if (!d->isValid
            || data == NULL
            || index >= d->fileHeader.blockNumber
            || d->storageFile.openMode() == QIODevice::ReadOnly)
    {
        return 0;
    }

    d->entryListMutex.lock();
    if (d->blockEntryList.at(index).length != len)
    {
        d->entryListMutex.unlock();
        return 0;
    }

    quint32 offset = d->blockEntryList.at(index).offset;
    d->entryListMutex.unlock();

    d->dataChanged = true;
    quint32 writeLen = d->write(data, offset, len);
    d->storageFile.flush();
    fsync(d->storageFile.handle());
    return writeLen;
}

/***************************************************************************************************
 * mapAdditionalData : map the additional region data, return NULL if failed
 **************************************************************************************************/
uchar *StorageFile::mapAdditionalData()
{
    Q_D(StorageFile);
    if (d->fileHeader.reservedAreaSize == 0)
    {
        return NULL;
    }
    return d->storageFile.map(d->fileHeader.reservedAreaOffset, d->fileHeader.reservedAreaSize);
}

/***************************************************************************************************
 * unmapAdditionalData : unmap the additional region data memory ptr
 **************************************************************************************************/
bool StorageFile::unmapAdditionalData(uchar *address)
{
    Q_D(StorageFile);
    return  d->storageFile.unmap(address);
}

/***************************************************************************************************
 * remove : remove the real file managed by StorageFile object
 **************************************************************************************************/
void StorageFile::remove(StorageFile *file)
{
    if (file)
    {
        StorageFilePrivate *d = file->d_func();
        QString storageFilename = d->storageFile.fileName();
        QString backupFilename = d->backupFile.fileName();
        file->reload(QString(), QIODevice::ReadOnly);
        if (d->storageFile.exists())
        {
            QFile::remove(storageFilename);
        }
        if (d->backupFile.exists())
        {
            QFile::remove(backupFilename);
        }
    }
}

/***************************************************************************************************
 * rename : rename the file managed by StorageFile object
 **************************************************************************************************/
bool StorageFile::rename(StorageFile *file, const QString &newname)
{
    bool storageName = false;
    bool backupName = true;
    if (file && file->isValid())
    {
        StorageFilePrivate *d = file->d_func();
        QIODevice::OpenMode mode = d->storageFile.openMode();
        QString storageFilename = d->storageFile.fileName();
        QString backupFilename = d->backupFile.fileName();
        file->reload(QString(), QIODevice::ReadOnly);
        QString newFilename = newname + STORAGE_FILE_SUFFIX;
        QString newBackupFilename = newFilename + BACKUP_FILE_SUFFIX;
        storageName = QFile::rename(storageFilename, newFilename);
        if (d->backupFile.exists())
        {
            backupName = QFile::rename(backupFilename, newBackupFilename);
        }
        file->reload(newname, mode);
    }
    return storageName && backupName;
}
