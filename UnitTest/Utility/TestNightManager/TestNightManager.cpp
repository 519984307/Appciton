/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/6
 **/

#include "TestNightManager.h"
#include "NightModeManager.h"
#include "Framework/Sound/SoundManagerInterface.h"
#include "ConfigManagerInterface.h"
#include "MockSystemManager.h"
#include "MockSoundManager.h"
#include "MockNIBPParam.h"
#include "MockConfigManager.h"
#include "IConfig.h"

using ::testing::Mock;
using ::testing::AtMost;
using ::testing::ReturnRef;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::_;

void TestNightManager::initTestCase()
{
}

void TestNightManager::cleanupTestCase()
{
}

void TestNightManager::testSetNightMode_data()
{
    QTest::addColumn<bool>("nightMode");
    QTest::addColumn<int>("screenBrightness");
    QTest::addColumn<int>("alarmVolume");
    QTest::addColumn<int>("heartBeatVolume");
    QTest::addColumn<int>("notificationVolume");
    QTest::addColumn<int>("completeTips");
    QTest::addColumn<int>("stopNibpMeasure");

    QTest::newRow("nightMode") << true << 0 << 1 << 0 << 0 << 1 << 0;
    QTest::newRow("normalMode") << false << 1 << 1 << 1 << 1 << 1 << 0;

    QTest::newRow("nightMode_stop_nibpMeasure") << true << 1 << 1 << 1 << 1 << 1 << 1;
    QTest::newRow("normalMode_stop_nibpMeasure") << false << 1 << 1 << 1 << 1 << 1 << 1;

    QTest::newRow("nightMode_close_screen") << true << 0 << 1 << 1 << 1 << 1 << 1;
    QTest::newRow("normalMode_close_screen") << false << 0 << 1 << 1 << 1 << 1 << 1;

    QTest::newRow("nightMode_stop_alarmVolume") << true << 1 << 0 << 1 << 1 << 1 << 1;
    QTest::newRow("normalMode_stop_alarmVolume") << false << 1 << 0 << 1 << 1 << 1 << 1;

    QTest::newRow("nightMode_stop_heartBeatVolume") << true << 1 << 1 << 0 << 1 << 1 << 1;
    QTest::newRow("normalMode_stop_heartBeatVolume") << false << 1 << 1 << 0 << 1 << 1 << 1;

    QTest::newRow("nightMode_stop_notificationVolume") << true << 1 << 1 << 1 << 0 << 1 << 1;
    QTest::newRow("normalMode_stop_notificationVolume") << false << 1 << 1 << 1 << 0 << 1 << 1;

    QTest::newRow("nightMode_stop_completeTips") << true << 1 << 1 << 1 << 1 << 0 << 1;
    QTest::newRow("normalMode_stop_completeTips") << false << 1 << 1 << 1 << 1 << 0 << 1;

    QTest::newRow("nightMode_stop_notificationVolume_completeTips") << true << 1 << 1 << 1 << 0 << 0 << 1;
    QTest::newRow("normalMode_stop_notificationVolume_completeTips") << false << 1 << 1 << 1 << 0 << 0 << 1;

    QTest::newRow("nightMode_all_close") << true << 0 << 0 << 0 << 0 << 0 << 0;
    QTest::newRow("normalMode_all_close") << false << 0 << 0 << 0 << 0 << 0 << 0;
}

void TestNightManager::testSetNightMode()
{
    QFETCH(bool, nightMode);
    QFETCH(int, screenBrightness);
    QFETCH(int, alarmVolume);
    QFETCH(int, heartBeatVolume);
    QFETCH(int, notificationVolume);
    QFETCH(int, completeTips);
    QFETCH(int, stopNibpMeasure);

    systemConfig.load(":System.Original.xml");
    nightModeData data = {nightMode, screenBrightness, alarmVolume, heartBeatVolume,
                         notificationVolume, completeTips, stopNibpMeasure};
    readyNightModeData(data);

    // mock system manager
    MockSystemManager mockSystemManager;
    SystemManagerInterface::registerSystemManager(&mockSystemManager);
    BrightnessLevel brightness = static_cast<BrightnessLevel>(screenBrightness);
    EXPECT_CALL(mockSystemManager, setBrightness(brightness));
    EXPECT_CALL(mockSystemManager, getBrightness()).WillRepeatedly(Return(
                                                                       static_cast<BrightnessLevel>(screenBrightness)));

    // mock sound manager
    MockSoundManager mockSoundManager;
    SoundManagerInterface::registerSoundManager(&mockSoundManager);

    // mock configManager
    MockConfigManager mockConfigManager;
    MockConfigManager::registerConfigManager(&mockConfigManager);
    Config currentConfig(":/AdultConfig.Original.xml");

    EXPECT_CALL(mockConfigManager, getCurConfig()).WillRepeatedly(ReturnRef(currentConfig));


    /* night mode will no longer set alarm volumn */
    EXPECT_CALL(mockSoundManager, setVolume(SoundManagerInterface::SOUND_TYPE_ALARM,
                                            static_cast<SoundManagerInterface::VolumeLevel>(alarmVolume))).Times(0);
    if (!nightMode) {
        currentConfig.getNumValue("ECG|QRSVolume", heartBeatVolume);
        systemConfig.getNumValue("General|KeyPressVolume", notificationVolume);

        systemConfig.getNumValue("PrimaryCfg|NIBP|CompleteTone", completeTips);
    }
    EXPECT_CALL(mockSoundManager, setVolume(SoundManagerInterface::SOUND_TYPE_HEARTBEAT,
                                            static_cast<SoundManagerInterface::VolumeLevel>(heartBeatVolume)));
    EXPECT_CALL(mockSoundManager, setVolume(SoundManagerInterface::SOUND_TYPE_NOTIFICATION,
                                            static_cast<SoundManagerInterface::VolumeLevel>(notificationVolume)));
    if (completeTips)
    {
        EXPECT_CALL(mockSoundManager, setVolume(SoundManagerInterface::SOUND_TYPE_NIBP_COMPLETE,
                                                static_cast<SoundManagerInterface::VolumeLevel>(notificationVolume)));
    }
    else
    {
        EXPECT_CALL(mockSoundManager, setVolume(SoundManagerInterface::SOUND_TYPE_NIBP_COMPLETE,
                                                static_cast<SoundManagerInterface::VolumeLevel>(0)));
    }

    // mock nibpparam
    MockNIBPParam mockNIBPParam;
    MockNIBPParam::registerNIBPParam(&mockNIBPParam);
    EXPECT_CALL(mockNIBPParam, stopMeasure()).Times(AnyNumber());



    nightModeManager.setNightMode(nightMode);
//    QVERIFY(Mock::VerifyAndClearExpectations(&mockSystemManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockSoundManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockNIBPParam));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockConfigManager));
}

void TestNightManager::testNightMode_data()
{
    QTest::addColumn<bool>("isNightMode");

    QTest::newRow("nightMode") << true;
    QTest::newRow("Normal") << false;
}

void TestNightManager::testNightMode()
{
    QFETCH(bool, isNightMode);
    nightModeData data = {isNightMode, 1, 1, 1, 1, 0, 0};
    readyNightModeData(data);

    // mock nibpparam
    MockNIBPParam mockNIBPParam;
    MockNIBPParam::registerNIBPParam(&mockNIBPParam);
    EXPECT_CALL(mockNIBPParam, stopMeasure()).Times(AnyNumber());

    // mock system manager
    MockSystemManager mockSystemManager;
    SystemManagerInterface::registerSystemManager(&mockSystemManager);
    EXPECT_CALL(mockSystemManager, enableBrightness(_)).Times(AnyNumber());
    EXPECT_CALL(mockSystemManager, getBrightness()).WillRepeatedly(testing::Return(
                                                                       static_cast<BrightnessLevel>(1)));

    // mock configManager
    MockConfigManager mockConfigManager;
    MockConfigManager::registerConfigManager(&mockConfigManager);
    Config currentConfig("/usr/local/nPM/etc/AdultConfig.xml");
    EXPECT_CALL(mockConfigManager, getCurConfig()).WillRepeatedly(ReturnRef(currentConfig));

    // mock sound manager
    MockSoundManager mockSoundManager;
    SoundManagerInterface::registerSoundManager(&mockSoundManager);
    EXPECT_CALL(mockSoundManager, setVolume(_, _)).Times(AnyNumber());
    nightModeManager.setNightMode(isNightMode);
    QCOMPARE(nightModeManager.nightMode(), isNightMode);
    QVERIFY(Mock::VerifyAndClearExpectations(&mockSystemManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockSoundManager));
    QVERIFY(Mock::VerifyAndClearExpectations(&mockNIBPParam));
}

void TestNightManager::readyNightModeData(nightModeData data)
{
    Config currentConfig("/usr/local/nPM/etc/AdultConfig.xml");
    // prepare night Mode Data start........
    if (data.nightMode)
    {
        // 屏幕亮度(读取夜间模式)
        systemConfig.setNumValue("NightMode|ScreenBrightness", data.screenBrightness);
        // 报警音量
        systemConfig.setNumValue("NightMode|AlarmVolume", data.alarmVolume);
        // 心跳音量
        systemConfig.setNumValue("NightMode|HeartBeatVolume", data.heartBeatVolume);
        // 按键音量
        systemConfig.setNumValue("NightMode|KeyPressVolume", data.notificationVolume);
        // nibp完成音
        systemConfig.setNumValue("NightMode|NIBPCompletedTips", data.completeTips);
        // 停止nibp测量
        systemConfig.setNumValue("NightMode|StopNIBPMeasure", data.stopNibpMeasure);
    }
    else
    {
        systemConfig.setNumValue("General|DefaultDisplayBrightness", data.screenBrightness);

        systemConfig.setNumValue("Alarms|DefaultAlarmVolume", data.alarmVolume);

        currentConfig.setNumValue("ECG|QRSVolume", data.heartBeatVolume);
        currentConfig.saveToDisk();

        systemConfig.setNumValue("General|KeyPressVolume", data.notificationVolume);

        systemConfig.setNumValue("PrimaryCfg|NIBP|CompleteTone", data.completeTips);

        systemConfig.setNumValue("NightMode|StopNIBPMeasure", data.stopNibpMeasure);
    }
    systemConfig.saveToDisk();
    // prepare night Mode Data end........
}
