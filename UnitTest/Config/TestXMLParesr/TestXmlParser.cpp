/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/2/20
 **/

#include "TestXmlParser.h"
#include <QTemporaryFile>
using ::testing::_;
using ::testing::NotNull;
using ::testing::Mock;
Q_DECLARE_METATYPE(QDomElement)

static void createTestFileOnDisk(QTemporaryFile *tempFile, const QString &resFilename)
{
    QFile resFile(resFilename);
    QCOMPARE(resFile.open(QIODevice::ReadOnly), true);
    tempFile->open();
    tempFile->write(resFile.readAll());
    resFile.close();
    tempFile->close();
}

static QVariantMap createMap()
{
    QVariantMap map;
    QVariantList list;
    QVariantMap n1;
    n1["@text"] = QString("aaa");
    n1["@attr22"] = QString("attr22");
    list.append(n1);
    QVariantMap n2;
    n2["@text"] = QString("bbb");
    n2["@attr22"] = QString("attr22");
    n2["@attr21"] = QString("attr21");
    list.append(n2);
    QVariantMap n3;
    n3["@text"] = QString("ccc");
    n3["@attr22"] = QString("attr22");
    n3["@attr23"] = QString("attr23");
    list.append(n3);
    QVariantMap n11;
    n11["Node111"] = list;
    n11["@attr11"] = QString("attr11");
    map["Node11"] = n11;

    return map;
}

TestXMLParser::TestXMLParser()
    : m_validConfigFile(NULL), m_invalidConfigFile(NULL)
{
}

TestXMLParser::~TestXMLParser()
{
}

void TestXMLParser::init()
{
    m_validConfigFile = new QTemporaryFile();
    createTestFileOnDisk(m_validConfigFile, ":/ValidConfig.xml");
    m_invalidConfigFile = new QTemporaryFile();
    createTestFileOnDisk(m_invalidConfigFile, ":/Invalid1.xml");
}

void TestXMLParser::cleanup()
{
    delete m_validConfigFile;
    m_validConfigFile = NULL;
    delete m_invalidConfigFile;
    m_invalidConfigFile = NULL;
}

void TestXMLParser::testNoExistFile()
{
    XmlParser parser;

    QCOMPARE(parser.open("~/noExistXmlFile.xml"), false);
}

void TestXMLParser::testInvalidXmlFile()
{
    XmlParser parser;

    QCOMPARE(parser.open(m_invalidConfigFile->fileName()), false);
}
void TestXMLParser::testAddNode_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("key");
    QTest::addColumn<QString>("keyStr");
    QTest::addColumn<bool>("result");
    QTest::newRow("row1") << "Node3" << "Node33" << "ddd" << "attr44" << "attr44" << true;
    QTest::newRow("flaseRow") << "false" << "Node23" << "ddd" << "attr44" << "attr44" << false;
    QTest::newRow("exit Node") << "Node11" <<"Node111" << "aaa" << "attr22" << "attr22" << false;
}
void TestXMLParser::testAddNode()
{
    QFETCH(QString, path);
    QFETCH(QString, name);
    QFETCH(QString, value);
    QFETCH(QString, key);
    QFETCH(QString, keyStr);
    QFETCH(bool, result);

    XmlParser parser;
    QCOMPARE(parser.open(m_validConfigFile->fileName()), true);

    QMap<QString, QString> map;
    map[key] = keyStr;
    QCOMPARE(parser.addNode(path, name, value, map), result);
    QString str;
    parser.getValue(path, str);
    QCOMPARE(str == value, result);
}

void TestXMLParser::testRemoveNode_data()
{
    QTest::addColumn<QString>("node");
    QTest::addColumn<bool>("result");
    QTest::newRow("rightPath") << "Node1|Node11" << true;
    QTest::newRow(("falsePath")) << "Node1|Node22|Node111" << false;
    QTest::newRow("NotExitPath") << "Node4" << false;
    QTest::newRow("errorPath") << "Node1|Node111" << false;
}

void TestXMLParser::testRemoveNode()
{
    XmlParser parser;
    QCOMPARE(parser.open(m_validConfigFile->fileName()), true);

    QFETCH(QString, node);
    QFETCH(bool, result);
    QCOMPARE(parser.removeNode(node), result);
    QCOMPARE(parser.hasNode(node), false);
}

void TestXMLParser::testHasNode_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("result");

    QTest::newRow("Parent Node") << "Node1" << true;
    QTest::newRow("Parent Node") << "Node2" << true;
    QTest::newRow("No Exist Node") << "Node1|NodeNoExist" << false;
    QTest::newRow("Child Node1") << "Node1|Node11|Node111" << true;
    QTest::newRow("Child Node2") << "Node2|Node22" << true;
    QTest::newRow("Can't test Root node") << "Root" << false;
    QTest::newRow("Can't start with Root node") << "Root|Node1" << false;
    QTest::newRow("weird path1") << "|Node1|" << true;
    QTest::newRow("weird path2") << "Node1||" << true;
    QTest::newRow("weird path3") << "Node1||Node2" << false;
}

void TestXMLParser::testHasNode()
{
    QFETCH(QString, path);
    QFETCH(bool, result);

    XmlParser parser;
    parser.open(m_validConfigFile->fileName());
    QCOMPARE(parser.hasNode(path), result);
}

void TestXMLParser::testGetValue()
{
    XmlParser parser;
    parser.open(m_validConfigFile->fileName());

    // test exist node
    QString str;
    QCOMPARE(parser.getValue("Node1|Node11|Node111", str), true);
    QVERIFY(str == "aaa");

    QString noExist;
    QCOMPARE(parser.getValue("Node1|NonExit", noExist), false);
    QCOMPARE(noExist.isNull(), true);
}

void TestXMLParser::testSetValue()
{
    XmlParser parser;
    parser.open(m_validConfigFile->fileName());

    QString result;
    // write a exist node
    QCOMPARE(parser.setValue("Node1|Node11|Node111", "ccc"), true);
    parser.getValue("Node1|Node11|Node111", result);
    QVERIFY(result == "ccc");

    QString testString = "<testNode>abc</testNode>";
    QCOMPARE(parser.setValue("Node2|Node22", testString), true);
    parser.getValue("Node2|Node22", result);
    QVERIFY(result == testString);

    // write to a non exist node
    QCOMPARE(parser.setValue("Node1|Node11|NoExist", "ddd"), false);
}


void TestXMLParser::testSetAttr_data()
{
    QTest::addColumn<QString>("node");
    QTest::addColumn<QString>("nodeAttr");
    QTest::addColumn<QString>("writeValue");
    QTest::addColumn<QString>("readValue");
    QTest::addColumn<bool>("result");

    QTest::newRow("exist node") << "Node1|Node11" << "attr11" << "abc" << "abc" << true;
    QTest::newRow("Invalid attr") << "Node1|Node11" << "NonExit" << "abc" << QString() << false;
}

void TestXMLParser::testSetAttr()
{
    QFETCH(QString, node);
    QFETCH(QString, nodeAttr);
    QFETCH(QString, writeValue);
    QFETCH(QString, readValue);
    QFETCH(bool, result);

    XmlParser parser;
    parser.open(m_validConfigFile->fileName());

    QString curReadValue;

    QCOMPARE(parser.setAttr(node, nodeAttr, writeValue), result);
    parser.getAttr(node, nodeAttr, curReadValue);
    QVERIFY(readValue == curReadValue);
}

void TestXMLParser::testGetAttr_data()
{
    QTest::addColumn<QString>("node");
    QTest::addColumn<QString>("nodeAttr");
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("result");

    QTest::newRow("Valid node") << "Node1|Node11" << "attr11" << "attr11" << true;
    QTest::newRow("Invalid attr") << "Node1|Node11" << "NonExit" << QString() << false;
}

void TestXMLParser::testGetAttr()
{
    QFETCH(QString, node);
    QFETCH(QString, nodeAttr);
    QFETCH(QString, value);
    QFETCH(bool, result);

    XmlParser parser;
    parser.open(m_validConfigFile->fileName());

    QString readValue;

    QCOMPARE(parser.getAttr(node, nodeAttr, readValue), result);
    QVERIFY(readValue == value);
}

void TestXMLParser::testGetNode_data()
{
     QTest::addColumn<QString>("indexstr");
     QTest::addColumn<QString>("value");
     QTest::addColumn<bool>("result");

     QTest::newRow("rightPath1") << "Node1|Node11|Node111" << "aaa" << true;
     QTest::newRow("rightPath2") << "Node2|Node22" << "bbb" << true;
     QTest::newRow("errorPaht") << "Node22|Node22" << "bbb" << false;
     QTest::newRow("emptyValue") << "Node3" << "" << true;
     QTest::newRow("errorPath2") << "Node111" << "aaa" << false;
}

void TestXMLParser::testGetNode()
{
   QFETCH(QString, indexstr);
   QFETCH(QString, value);
   QFETCH(bool, result);
   QDomElement str;
   XmlParser parser;
   QCOMPARE(parser.open(m_validConfigFile->fileName()), true);

   parser.getNode(indexstr, str);
   QCOMPARE(str.text() == value, result);
}
void TestXMLParser::testSetNode_data()
{
    QDomElement element1;
    element1.setAttribute("xxx", "aaa");

    QDomElement element2;
    element2.setNodeValue("ddd");

    QDomElement element3;

    QTest::addColumn<QString>("indexstr");
    QTest::addColumn<QDomElement>("value");
    QTest::addColumn<bool>("result");

    QTest::newRow("addAtrributeNode") << "Node1|Node11|Node112" << element1 << true;
    QTest::newRow("addValueNode") << "Node2|Node23" << element2 << true;
    QTest::newRow("emptyNode") << "Node4" << element3 << true;
    QTest::newRow("exitNode") << "Node1|Node11|Node111" << element1 << false;
    QTest::newRow("StrangeString") << "Node]]]][[[[.." << element1 << true;
}

void TestXMLParser::testSetNode()
{
    QFETCH(QString, indexstr);
    QFETCH(QDomElement, value);
    QFETCH(bool, result);

    XmlParser parser;
    QCOMPARE(parser.open(m_validConfigFile->fileName()), true);

    QCOMPARE(parser.setNode(indexstr, value), result);

    QDomElement element;
    parser.getNode(indexstr, element);
    QCOMPARE(element == value, result);
}

void TestXMLParser::testGetFirstValue()
{
    QSKIP("Todo:: implement this test!", SkipAll);
}

void TestXMLParser::testGetNextValue()
{
    QSKIP("Todo:: implement this test!", SkipAll);
}

void TestXMLParser::testFileName()
{
    QFETCH(QString, origin);
    QFETCH(QString, test);
    QFETCH(QString, result);

    XmlParser parser;
    parser.open(origin);
    parser.setCurrentFilePath(test);
    QCOMPARE(parser.currentFileName(), result);
}

void TestXMLParser::testFileName_data()
{
    QTest::addColumn<QString>("origin");
    QTest::addColumn<QString>("test");
    QTest::addColumn<QString>("result");

    QTest::newRow("normal") << ":/ValidConfig.xml" << "configFilename" << "configFilename";
    QTest::newRow("empty file name") << ":/ValidConfig.xml" << "" << ":/ValidConfig.xml";
    QTest::newRow("null file name") << ":/ValidConfig.xml" << QString() << ":/ValidConfig.xml";
    QTest::newRow("all empty") << "" << "" << "";
    QTest::newRow("new name") << ":/ValidConfig.xml" << "" << ":/ValidConfig.xml";
}

void TestXMLParser::testGetConfig()
{
    QVariantMap map = createMap();

    XmlParser parser;
    parser.open(m_validConfigFile->fileName());

    QCOMPARE(parser.getConfig("Node1"), map);

    QVariantMap emptyMap;
    QCOMPARE(parser.getConfig("Node3"), emptyMap);

    QCOMPARE(parser.getConfig("Node3|NonExistNode"), emptyMap);
}

void TestXMLParser::testSetConfig()
{
    QVariantMap map = createMap();

    XmlParser parser;
    parser.open(m_validConfigFile->fileName());

    parser.setConfig("Node3", map);
    QCOMPARE(parser.getConfig("Node3"), map);

    // will create the node is the node is not exist
    parser.setConfig("Node3|NoExistNode", map);
    QCOMPARE(parser.getConfig("Node3|NoExistNode"), map);

    // will fail if the parent node is not exist
    parser.setConfig("Node3|NoExistParentNode|NoExistNode", map);
    QCOMPARE(parser.getConfig("Node3|NoExistParentNode|NoExistNode"), QVariantMap());
}
