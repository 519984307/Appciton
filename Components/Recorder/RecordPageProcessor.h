#pragma once
#include <QObject>
#include <RecordPage.h>
#include <PrintProviderIFace.h>
#include <QScopedPointer>

class RecordPageProcessorPrivate;
class RecordPageProcessor: public QObject
{
    Q_OBJECT
public:
    RecordPageProcessor(PrintProviderIFace *iface, QObject *parent = 0);
    ~RecordPageProcessor();

    /**
     * @brief isProcessing check whether current is processing
     * @return  true if in process state, otherwise, return false
     */
    bool isProcessing() const;

signals:
    /**
     * @brief processFinished emit when all page is processed
     */
    void processFinished();

    /**
     * @brief pageQueueFull emit when the page queue status changed
     * @param isFull page queue full or not
     */
    void pageQueueFull(bool isFull);

public slots:
    /**
     * @brief updatePrintSpeed update the print speed
     */
    void updatePrintSpeed(PrintSpeed speed);

    /**
     * @brief addPage add page to print
     * @param page
     */
    void addPage(RecordPage* page);

    /**
     * @brief flushPages flush all the pages, the pages haven't been print will be dropped
     */
    void flushPages();

    /**
     * @brief pauseProcessing pause the processing or not
     * @param pause pause or not
     */
    void pauseProcessing(bool pause);

private slots:
    /**
     * @brief processPages process the pages
     */
    void processPages();

private:
    QScopedPointer<RecordPageProcessorPrivate> d_ptr;
};
