/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/9/18
 **/

#pragma once
#include <QString>
#include <QIODevice>
#include <QByteArray>

struct BlockEntry
{
    BlockEntry(): type(0), offset(0), length(0), extraData(0) {}
    BlockEntry(quint32 type, quint32 offset, quint32 length, quint32 exitaData = 0):
        type(type), offset(offset), length(length), extraData(exitaData) {}
    quint32 type;
    quint32 offset;
    quint32 length;
    quint32 extraData;
};

class IStorageBackend
{
public:
    IStorageBackend() {}
    virtual ~IStorageBackend() {}

    // get the file type
    virtual int fileType()
    {
        return 0;
    }

    // get the file size
    virtual quint32 fileSize() = 0;

    // check whether the file is valid
    virtual bool isValid() = 0;

    // get reserved size, additional data will be written to reserve area
    // return true if success
    virtual bool setReservedSize(quint32 size) = 0;

    // get reserved size
    virtual quint32  getReservedSize() = 0;

    // reload file
    virtual void reload(const QString &filename, QIODevice::OpenMode openmode) = 0;

    // get block data in the file
    virtual quint32 getBlockNR() = 0;

    // write additional data
    virtual quint32 writeAdditionalData(const char *data, quint32 len) = 0;

    // read additional data
    virtual quint32 readAdditionalData(char *data, quint32 len) = 0;

    // get block data length, index start from zero
    virtual quint32 getBlockDataLen(quint32 index) = 0;

    // get the block data
    virtual QByteArray getBlockData(quint32 index) = 0;

    // get the block data type in specific index
    virtual quint32 getBlockType(quint32 index) = 0;

    // get the block info in specific index
    virtual BlockEntry getBlockInfo(quint32 index) = 0;

    // get block entry list
    virtual QVector<BlockEntry> getBlockEntryList() = 0;

    // read block data, index start from zero
    virtual quint32 readBlockData(quint32 index, char *data, quint32 len) = 0;

    // append block data
    virtual quint32 appendBlockData(quint32 type, const char *data, quint32 len, quint32 extraData = 0) = 0;

    // modify data at specific index, index start from zero
    virtual quint32 modifyBlockData(quint32 index, char *data, quint32 len) = 0;

    // get the memory map of the additional data, 0 for invalid
    virtual uchar *mapAdditionalData() = 0;

    // unmap the memory map of the additional data
    virtual bool unmapAdditionalData(uchar *address) = 0;

private:
    Q_DISABLE_COPY(IStorageBackend)
};
