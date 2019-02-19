#include "TestXmlParser.h"
#include <QTemporaryFile>
using namespace ::testing;

static void createTestFileOnDisk(QTemporaryFile *tempFile, const QString &resFilename)
{
    QFile resFile(resFilename);
    QCOMPARE(resFile.open(QIODevice::ReadOnly), true);
    tempFile->open();
    tempFile->write(resFile.readAll());
    resFile.close();
    tempFile->close();
}

TestXMLParser::TestXMLParser()
    :m_validConfigFile(NULL), m_invalidConfigFile(NULL)
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
    MockErrorLog mockErrorLog;
    ErrorLogInterface::registerErrorLog(&mockErrorLog);
    EXPECT_CALL(mockErrorLog, append(NotNull()));

    XmlParser parser;

    QCOMPARE(parser.open(m_invalidConfigFile->fileName()), false);
}

void TestXMLParser::testAddNode()
{

}

void TestXMLParser::testRemoveNode()
{

}

void TestXMLParser::testHasNode_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("result");

    QTest::newRow("Parent Node")<<"Node1"<<true;
    QTest::newRow("Parent Node")<<"Node2"<<true;
    QTest::newRow("No Exist Node")<<"Node1|NodeNoExist"<<false;
    QTest::newRow("Child Node1")<<"Node1|Node11|Node111"<<true;
    QTest::newRow("Child Node2")<<"Node2|Node22"<<true;
    QTest::newRow("Can't test Root node")<<"Root"<<false;
    QTest::newRow("Can't start with Root node")<<"Root|Node1"<<false;
    QTest::newRow("weird path1")<<"|Node1|"<<true;
    QTest::newRow("weird path2")<<"Node1||"<<true;
    QTest::newRow("weird path3")<<"Node1||Node2"<<false;
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
    QVERIFY(str=="aaa");

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

    QTest::newRow("exist node")<<"Node1|Node11"<<"attr11"<<"abc"<<"abc"<<true;
    QTest::newRow("Invalid attr")<<"Node1|Node11"<<"NonExit"<<"abc"<<QString()<<false;
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

    QTest::newRow("Valid node")<<"Node1|Node11"<<"attr11"<<"attr11"<<true;
    QTest::newRow("Invalid attr")<<"Node1|Node11"<<"NonExit"<<QString()<<false;
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


void TestXMLParser::testGetNode()
{

}

void TestXMLParser::testSetNode()
{

}

void TestXMLParser::testGetFirstValue()
{

}

void TestXMLParser::testGetNextValue()
{

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

    QTest::newRow("normal")<<":/ValidConfig.xml"<<"configFilename"<<"configFilename";
    QTest::newRow("empty file name")<<":/ValidConfig.xml"<<""<<":/ValidConfig.xml";
    QTest::newRow("null file name")<<":/ValidConfig.xml"<<QString()<<":/ValidConfig.xml";
    QTest::newRow("all empty")<<""<<""<<"";
    QTest::newRow("new name")<<":/ValidConfig.xml"<<""<<":/ValidConfig.xml";
}

void TestXMLParser::testGetConfig()
{

}

void TestXMLParser::testSetConfig()
{

}
