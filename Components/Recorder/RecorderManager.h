/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/7
 **/

#pragma once
#include <QObject>
#include <QScopedPointer>
#include "PrintProviderIFace.h"
#include "PrintDefine.h"
#include "PrintTypeDefine.h"
#include "RecordPageGenerator.h"

class RecorderManagerPrivate;
class RecorderManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief getInstance singleton
     * @return reference to the Recorder Manager
     */
    static RecorderManager &getInstance();
    ~RecorderManager();

    /**
     * @brief getPrintSpeed get the recorder speed
     * @return
     */
    PrintSpeed getPrintSpeed() const;

    /**
     * @brief setPrintSpeed set the recorder speed
     * @param speed
     */
    void setPrintSpeed(PrintSpeed speed);

    /**
     * @brief getPrintWaveNum get realtime print wave num
     * @return the realtime print wave num
     */
    int getPrintWaveNum();

    /**
     * @brief setPrintWaveNum set the realtime print wave num
     * @param num
     */
    void setPrintWaveNum(int num);

    /**
     * @brief setPrintPrividerIFace set the print provider
     * @param iface
     */
    void setPrintPrividerIFace(PrintProviderIFace *iface);

    /**
     * @brief provider get the print provider
     * @return the print provider iterface
     */
    PrintProviderIFace *provider() const;

    /**
     * @brief isConnected check whether the printer is connected
     * @return true if connected, otherwise, false
     */
    bool isConnected() const;

    /**
     * @brief isPrinting check whether the printer is printing;
     * @return  true if printing, otherwise, false
     */
    bool isPrinting() const;

    /**
     * @brief abort abort current printing
     */
    void abort();

    /**
     * @brief isAbort check whether in abort state
     * @return
     */
    bool isAbort() const;

    /**
     * @brief getPrintStatus get the printer status
     * @return
     */
    PrinterStatus getPrintStatus() const;

    /**
     * @brief selfTest perform the selftest
     */
    void selfTest();

    /**
     * @brief requestPrint request print a specific type
     * @param type print type
     * @return  sucess or not
     */
    bool requestPrint(PrintType type);

    /**
     * @brief addPageGeneratory add a page generator to generator the print page
     * @param generator the generator
     * @return true when adding sucessfully, otherwise, false.
     *         It happens when existing a higher priority generator
     */
    bool addPageGenerator(RecordPageGenerator *generator);

    /**
     * @brief setPrintTime
     * @param timeSec
     */
    void setPrintTime(PrintTime timeSec);

    /**
     * @brief getPrintTime
     * @return
     */
    PrintTime getPrintTime(void) const;

    /**
     * @brief stopPrint 停止打印
     * @return
     */
    void stopPrint(void);

    /**
     * @brief getCurPrintPriority 获取当前打印优先级
     * @return
     */
    RecordPageGenerator::PrintPriority getCurPrintPriority(void);

    /**
     * @brief printWavesUpdateInit  打印波形初始化--配置文件初始化时需要重新更新打印波形id
     */
    void printWavesUpdateInit(void);

private slots:

    void testSlot();

    /**
     * @brief providerRestarted provider restart
     */
    void providerRestarted();

    /**
     * @brief prodierConnectionChanged provider connection changed
     */
    void providerConnectionChanged(bool isConnected);

    /**
     * @brief providerStatusChanged provider status changde
     * @param status
     */
    void providerStatusChanged(PrinterStatus status);

    /**
     * @brief providerBufferStatusChanged provider status changed
     * @param isFull
     */
    void providerBufferStatusChanged(bool full);

    /**
     * @brief providerReportError retport an error
     * @param err error id
     */
    void providerReportError(unsigned char err);

    /**
     * @brief onGeneratorStopped handle the generator stop signal
     */
    void onGeneratorStopped();

signals:
    /**
     * @brief speedChanged emit when the speed is changed
     * @param speed the new speed
     */
    void speedChanged(PrintSpeed speed);

private:
    RecorderManager();
    QScopedPointer<RecorderManagerPrivate> d_ptr;
};

#define recorderManager (RecorderManager::getInstance())
#define deleteRecorderManager() (delete &recorderManager)
