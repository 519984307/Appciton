#include <QString>
#include <QtTest>

class Hemodymic : public QObject
{
    Q_OBJECT

public:
    Hemodymic();

private Q_SLOTS:
    void testCase1();
    void testCase2();
};

Hemodymic::Hemodymic()
{
}

void Hemodymic::testCase1()
{
int sum = _referenceReleased(1,2);
    QVERIFY2(sum == 3, "Failure");
}

void Hemodymic::testCase2()
{
    QVERIFY2(false, "Failure");
}

QTEST_APPLESS_MAIN(Hemodymic)

#include "hemodymic.moc"
