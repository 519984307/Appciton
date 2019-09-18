/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/2/20
 **/

#ifndef TESTXMLPARSER_H
#define TESTXMLPARSER_H
#include "MockErrorLog.h"
#include "XmlParser.h"
#include <QTest>

class QTemporaryFile;
class TestXMLParser : public QObject
{
    Q_OBJECT

public:
    TestXMLParser();
    ~TestXMLParser();

private slots:
    void init();
    void cleanup();
    void testNoExistFile();
    void testInvalidXmlFile();
    void testAddNode_data();
    void testAddNode();
    void testRemoveNode_data();
    void testRemoveNode();
    void testHasNode_data();
    void testHasNode();
    void testGetValue();
    void testSetValue();
    void testGetAttr_data();
    void testGetAttr();
    void testSetAttr_data();
    void testSetAttr();
    void testGetNode_data();
    void testGetNode();
    void testSetNode_data();
    void testSetNode();
    void testGetFirstValue();
    void testGetNextValue();
    void testFileName();
    void testFileName_data();
    void testGetConfig();
    void testSetConfig();

private:
    QTemporaryFile *m_validConfigFile;
    QTemporaryFile *m_invalidConfigFile;
};

#endif  // TESTXMLPARSER_H
