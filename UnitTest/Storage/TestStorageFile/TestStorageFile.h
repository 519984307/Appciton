/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/6
 **/

#ifndef TESTSTORAGEFILE_H
#define TESTSTORAGEFILE_H
#include <QString>
#include <QtTest>
#include <QMetaType>
#include "StorageFile.h"

class TestStorageFile : public QObject
{
    Q_OBJECT

public:
    TestStorageFile();

private Q_SLOTS:
    void init();
    void cleanup();
    void testFileType_data();
    void testFileType();
    void testFileSize_data();
    void testFileSize();
    void testIsValid_data();
    void testIsValid();
    void testReservedSize_data();
    void testReservedSize();
    void testReload_data();
    void testReload();
    void testGetBlockNR_data();
    void testGetBlockNR();
    void testAdditionalData_data();
    void testAdditionalData();
    void testGetBlockDataLen_data();
    void testGetBlockDataLen();
    void testBlockData_data();
    void testBlockData();
    void testGetBlockType_data();
    void testGetBlockType();
    void testGetBlockInfo_data();
    void testGetBlockInfo();
    void testGetBlockEntryList_data();
    void testGetBlockEntryList();
    void testModifyBlockData_data();
    void testModifyBlockData();
    void testRemove_data();
    void testRemove();
    void testRename_data();
    void testRename();

private:
    StorageFile *m_StorageFile;
    QTemporaryFile *m_File;
};
#endif // TESTSTORAGEFILE_H
