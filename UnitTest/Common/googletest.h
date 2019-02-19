#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <QTest>

#define GTEST_APPLESS_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    ::testing::InitGoogleMock(&argc, argv); \
    ::testing::InitGoogleTest(&argc, argv); \
    TestObject tc; \
    return QTest::qExec(&tc, argc, argv); \
}

#ifdef QT_GUI_LIB

#include <QtTest/qtest_gui.h>

#define GTEST_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    ::testing::InitGoogleMock(&argc, argv); \
    ::testing::InitGoogleTest(&argc, argv); \
    QApplication app(argc, argv); \
    QTEST_DISABLE_KEYPAD_NAVIGATION \
    TestObject tc; \
    return QTest::qExec(&tc, argc, argv); \
}

#else

#define GTEST_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    ::testing::InitGoogleMock(&argc, argv); \
    ::testing::InitGoogleTest(&argc, argv); \
    QCoreApplication app(argc, argv); \
    TestObject tc; \
    return QTest::qExec(&tc, argc, argv); \
}

#endif // QT_GUI_LIB
