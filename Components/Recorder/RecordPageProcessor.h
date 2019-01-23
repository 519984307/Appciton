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
#include <RecordPage.h>
#include <PrintProviderIFace.h>
#include <QScopedPointer>

class RecordPageProcessorPrivate;
class RecordPageProcessor: public QObject
{
    Q_OBJECT
public:
    explicit RecordPageProcessor(PrintProviderIFace *iface, QObject *parent = 0);
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
     * @brief setPrintSpeed immediately set print speed
     */
    void setPrintSpeed(PrintSpeed speed);

    /**
     * @brief addPage add page to print
     * @param page
     */
    void addPage(RecordPage *page);

    /**
     * @brief stopProcess stop processing pages, the pages haven't been printed will be dropped
     */
    void stopProcess();

    /**
     * @brief pauseProcessing pause the processing or not
     * @param pause pause or not
     */
    void pauseProcessing(bool pause);

protected:
    void timerEvent(QTimerEvent *ev);

private:
    QScopedPointer<RecordPageProcessorPrivate> d_ptr;
};
