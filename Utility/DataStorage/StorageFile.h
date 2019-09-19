/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/1
 **/

#pragma once
#include "IStorageBackend.h"


/***************************************************************************************************
 * StorageFile : sequence stroage file,use to store block data. Block data will always be added to
 *               the end of the file, block data index will append to the end of the file.
 *
 *
 * file structure：
 * ------------------------
 * |     File Header      |
 * ------------------------
 * |     reserved area    |
 * ------------------------
 * |     block data       |
 * ------------------------
 * |     block data       |
 * ------------------------
 * |       .......        |
 * ------------------------
 * |     block data       |
 * ------------------------
 * |     block data       |
 * ------------------------
 * | block data index list|
 * ------------------------
 *
 * file header structure：
 * -------------------------------------------------------------------------------------
 * |          |reserved area | reserved area | block entry | block number| block entry |
 * |file type |  offset      |     size      | list offset |             |  checksum   |
 * | 4 bytes  | 4 bytes      |   4 bytes     |   4 bytes   |    4 bytes  |  4 bytes    |
 * -------------------------------------------------------------------------------------
 *
 * block entry structure:
 * -----------------------------------------------------
 * | data type |data offset | data length | extra data |
 * |   4 bytes |  4 bytes   |   4 bytes   |   4 bytes  |
 * -----------------------------------------------------
 **************************************************************************************************/

class StorageFilePrivate;
class StorageFile: public IStorageBackend
{
public:
    enum
    {
        Type = 0x514553 // "SEQ"
    };
    StorageFile();
    explicit StorageFile(const QString &filename, QIODevice::OpenMode openmode = QIODevice::ReadWrite);
    ~StorageFile();

    // override
    int fileType();

    // override
    quint32 fileSize();

    // override
    bool isValid();

    // override
    bool setReservedSize(quint32 size);

    // override
    quint32 getReservedSize();

    // override
    void reload(const QString &filename, QIODevice::OpenMode openmode);

    // override
    quint32 getBlockNR();

    // override
    quint32 writeAdditionalData(const char *data, quint32 len);

    // override
    quint32 readAdditionalData(char *data, quint32 len);

    // override
    quint32 getBlockDataLen(quint32 index);

    // override
    QByteArray getBlockData(quint32 index);

    // override
    quint32 getBlockType(quint32 index);

    // override
    void getBlockInfo(quint32 index, BlockEntry &info);

    // override
    QVector<BlockEntry> getBlockEntryList();

    // override
    quint32 readBlockData(quint32 index, char *data, quint32 len);

    // override
    quint32 appendBlockData(quint32 type, const char *data, quint32 len, quint32 extraData = 0);

    // override
    quint32 modifyBlockData(quint32 index, char *data, quint32 len);

    // override
    uchar *mapAdditionalData();

    // override
    bool unmapAdditionalData(uchar *address);

    // remove the file managed by StorageFile
    static void remove(StorageFile *file);

    // rename the file managed by StorageFile
    static bool rename(StorageFile *file, const QString &newname);

protected:
    const QScopedPointer<StorageFilePrivate> d_ptr;
private:
    Q_DECLARE_PRIVATE(StorageFile)
};
