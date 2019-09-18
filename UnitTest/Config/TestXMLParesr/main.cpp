#include "googletest.h"
#include "TestXmlParser.h"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    QCoreApplication app(argc, argv);
    TestXMLParser tc;
    return QTest::qExec(&tc, argc, argv);
}
