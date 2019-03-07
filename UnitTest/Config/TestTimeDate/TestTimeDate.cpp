#include <QString>
#include <QtTest>

class TestTimeDateTest : public QObject
{
    Q_OBJECT

public:
    TestTimeDateTest();

private Q_SLOTS:
    void testCase1();
};

TestTimeDateTest::TestTimeDateTest()
{
}

void TestTimeDateTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(TestTimeDateTest)

#include "testtimedate.moc"
