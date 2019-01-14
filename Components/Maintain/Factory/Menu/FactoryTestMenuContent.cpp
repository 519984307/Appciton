/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/25
 **/
#include <QGridLayout>
#include <QLabel>
#include "LanguageManager.h"
#include "ComboBox.h"
#include "FactoryTestMenuContent.h"
#include "Debug.h"
#include "Button.h"
#include <QProcess>
#include "MessageBox.h"
#include "USBManager.h"
#include "FactoryMaintainManager.h"
#include "SpinBox.h"

class FactoryTestMenuContentPrivate
{
public:
    FactoryTestMenuContentPrivate();

#ifdef TEST_REFRESH_RATE_CONTENT
    SpinBox *freshRateSpinBox;
#endif

    static QString btnStr[FACTORY_CONTENT_TEST_NR];

    Button *lbtn[FACTORY_CONTENT_TEST_NR];
};

FactoryTestMenuContentPrivate::FactoryTestMenuContentPrivate()
{
#ifdef TEST_REFRESH_RATE_CONTENT
    freshRateSpinBox = NULL;
#endif
    for (int i = 0; i < FACTORY_CONTENT_TEST_NR; i++)
    {
        lbtn[i] = NULL;
    }
}

#ifdef OUTPUT_TESTPACKET_THROUGH_USB_CONTENT
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <IMessageBox.h>
#include "IThread.h"


static void usbOutPutTestPacket(bool isFullSpeed)
{
#define USB_PORT_CONTROL_ADDRESS 0x2184384
#define USB_CONTROL_TEST_PACKET_DATA 0x18441205
#define USB_CONTROL_FORCE_FS_MODE 0x18461205
#define USB_CONTROL_STOP_TEST 0x18401205

    if (!usbManager.isUSBExist())
    {
        IMessageBox("Error", "Connect U disk first.", false).exec();
        return;
    }
    int64_t *pageAddr = NULL;
    int64_t *v_addr = NULL;
    off_t pageOffset = (off_t) USB_PORT_CONTROL_ADDRESS % getpagesize();
    off_t pageAddress = (off_t)(USB_PORT_CONTROL_ADDRESS - pageOffset);
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0)
    {
        IMessageBox("Error", "Open /dev/mem failed!", false).exec();
        return;
    }

    pageAddr = static_cast<int64_t *> mmap(NULL, getpagesize(),
                                           PROT_WRITE, MAP_SHARED, mem_fd, pageAddress);
    if (pageAddr == static_cast<int64_t *>(-1))
    {
        IMessageBox("Error", "memory remap failed!", false).exec();
        return;
    }

    v_addr = static_cast<int64_t *>((int64_t)pageAddr + pageOffset);

    if (isFullSpeed)
    {
        *v_addr = USB_CONTROL_FORCE_FS_MODE;
        IThread::msleep(100);
    }
    *v_addr = USB_CONTROL_TEST_PACKET_DATA;

    IMessageBox("Success", "The usb port should output continual test packets.\nClose this window to stop test.",
                false).exec();

    *v_addr = USB_CONTROL_STOP_TEST;
    munmap(pageAddr, getpagesize());
    close(mem_fd);
}
#endif


QString FactoryTestMenuContentPrivate::btnStr[FACTORY_CONTENT_TEST_NR] =
{
    "FactoryLight",
    "FactorySound",
    "FactoryKey",
    "FactoryPrinter",
#ifdef OUTPUT_TESTPACKET_THROUGH_USB_CONTENT
    "USB High Speed Test packet",
    "USB Full Speed Test Packet",
#endif
#ifndef HIDE_WIFI_FUNCTION
    "FactoryWifi",
#endif
#ifndef HIDE_WIRED_NETWORK_FUNCTION
    "FactoryEnthernet",
#endif
    "FactoryBattery"
};

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
FactoryTestMenuContent::FactoryTestMenuContent()
    : MenuContent(trs("FactoryTest"),
                  trs("FactoryTestDesc")),
      d_ptr(new FactoryTestMenuContentPrivate)
{
}

void FactoryTestMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);
    this->setFocusPolicy(Qt::NoFocus);

    int row = 0;

    for (int i = 0; i < FACTORY_CONTENT_TEST_NR; i++)
    {
        QLabel *label;
        Button *button;
        label = new QLabel(trs(d_ptr->btnStr[i]));
        layout->addWidget(label, i, 0);
        button = new Button(trs("FactoryTest"));
        button->blockSignals(true);
        button->setButtonStyle(Button::ButtonTextOnly);
        layout->addWidget(button, i, 1);
        button->setProperty("Item", qVariantFromValue(i));
        connect(button, SIGNAL(released()), this, SLOT(onBtnReleased(int)));
        button->blockSignals(false);
        d_ptr->lbtn[i] = button;
    }

    layout->setRowStretch(row + FACTORY_CONTENT_TEST_NR, 1);
}
/**************************************************************************************************
 * 按钮槽函数。
 *************************************************************************************************/
void FactoryTestMenuContent::onBtnReleased(int id)
{
    switch (id)
    {
#ifdef OUTPUT_TESTPACKET_THROUGH_USB_CONTENT
    case FACTORY_CONTENT_TEST_USB_HS:
        usbOutPutTestPacket(false);
        break;
    case FACTORY_CONTENT_TEST_USB_FS:
        usbOutPutTestPacket(true);
        break;
#endif
    default:
        break;
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
FactoryTestMenuContent::~FactoryTestMenuContent()
{
    delete d_ptr;
}

void FactoryTestMenuContent::readyShow()
{
}



