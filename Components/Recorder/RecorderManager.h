#pragma once
#include <QObject>
#include <QScopedPointer>
#include "PrintProviderIFace.h"
#include "PrintDefine.h"
#include "PrintTypeDefine.h"

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
     * @brief setPrintPrividerIFace set the print provider
     * @param iface
     */
    void setPrintPrividerIFace(PrintProviderIFace *iface);


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


private slots:
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
