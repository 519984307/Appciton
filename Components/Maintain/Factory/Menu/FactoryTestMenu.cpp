#include <QVBoxLayout>
#include <QLabel>
#include "FontManager.h"
#include "LanguageManager.h"
#include "IComboList.h"
#include "FactoryTestMenu.h"
#include "Debug.h"
#include "IButton.h"
#include "FactoryWindowManager.h"
#include <QProcess>
#include "IMessageBox.h"
#include "USBManager.h"


#ifdef OUTPUT_TESTPACKET_THROUGH_USB
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

    if(!usbManager.isUSBExist())
    {
        IMessageBox("Error", "Connect U disk first.", false).exec();
        return;
    }
    unsigned long *pageAddr = NULL;
    unsigned long *v_addr = NULL;
    off_t pageOffset = (off_t) USB_PORT_CONTROL_ADDRESS % getpagesize();
    off_t pageAddress = (off_t) (USB_PORT_CONTROL_ADDRESS - pageOffset);
    int mem_fd = open("/dev/mem", O_RDWR|O_SYNC);
    if (mem_fd < 0)
    {
        IMessageBox("Error", "Open /dev/mem failed!", false).exec();
        return;
    }

    pageAddr = (unsigned long *) mmap(NULL, getpagesize(),
                                    PROT_WRITE, MAP_SHARED, mem_fd, pageAddress);
    if (pageAddr == (unsigned long *)-1)
    {
        IMessageBox("Error", "memory remap failed!", false).exec();
        return;
    }

    v_addr = (unsigned long *)((unsigned long)pageAddr + pageOffset);

    if(isFullSpeed)
    {
        *v_addr = USB_CONTROL_FORCE_FS_MODE;
        IThread::msleep(100);
    }
    *v_addr = USB_CONTROL_TEST_PACKET_DATA;

    IMessageBox("Success", "The usb port should output continual test packets.\nClose this window to stop test.", false).exec();

    *v_addr = USB_CONTROL_STOP_TEST;
    munmap(pageAddr, getpagesize());
    close(mem_fd);
}
#endif

FactoryTestMenu *FactoryTestMenu::_selfObj = NULL;

QString FactoryTestMenu::_btnStr[FACTORY_TEST_NR] =
{
    "FactoryLight",
    "FactorySound",
    "FactoryKey",
    "FactoryPrinter",
    #ifdef OUTPUT_TESTPACKET_THROUGH_USB
    "USB High Speed Test packet",
    "USB Full Speed Test Packet",
    #endif
    "FactoryWifi",
    "FactoryEnthernet",
    "FactoryBattery"
};

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
FactoryTestMenu::FactoryTestMenu() : PopupWidget()
{
    setCloseBtnTxt("");
    setCloseBtnPic(QImage("/usr/local/nPM/icons/main.png"));
    setCloseBtnColor(Qt::transparent);

//    setDesc(trs("FactoryTest"));

//    startLayout();
    layoutExec();
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void FactoryTestMenu::layoutExec()
{
    int submenuW = factoryWindowManager.getSubmenuWidth();
    int submenuH = factoryWindowManager.getSubmenuHeight();
    setFixedSize(submenuW, submenuH);

    setTitleBarText(trs("FactoryTest"));

    int itemW = submenuW - 200;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;

    for (int i = 0; i < FACTORY_TEST_NR; ++i)
    {
        lbtn[i] = new LabelButton(trs(_btnStr[i]));
        lbtn[i]->button->setText(trs("FactoryTest"));
        lbtn[i]->setFont(fontManager.textFont(fontSize));
        lbtn[i]->label->setFixedSize(btnWidth, ITEM_H);
        lbtn[i]->label->setAlignment(Qt::AlignCenter);
        lbtn[i]->button->setFixedSize(btnWidth, ITEM_H);
        lbtn[i]->button->setID(i);
        connect(lbtn[i]->button, SIGNAL(released(int)), this, SLOT(_btnReleased(int)));
        contentLayout->addWidget(lbtn[i]);
    }

#ifdef TEST_REFRESH_RATE
    _freshRateSpinBox = new ISpinBox(trs("RefreshRate"));
    _freshRateSpinBox->enableCycle(false);
    _freshRateSpinBox->setFixedHeight(ITEM_H);
    _freshRateSpinBox->setLabelWidth(btnWidth);
    _freshRateSpinBox->setRange(50, 70);
    _freshRateSpinBox->setStep(1);
    _freshRateSpinBox->setMode(ISPIN_MODE_INT);
    _freshRateSpinBox->setValueWidth(btnWidth);
    _freshRateSpinBox->setLabelAlignment(Qt::AlignCenter);
    _freshRateSpinBox->setLayoutSpacing(ICOMBOLIST_SPACE);
    _freshRateSpinBox->enableArrow(false);
    _freshRateSpinBox->setFont(fontManager.textFont(fontSize));
    connect(_freshRateSpinBox, SIGNAL(valueChange(QString,int)), this, SLOT(_onFreshRateChanged(QString)));
    contentLayout->addWidget(_freshRateSpinBox);

    int curRate = 60;
    QProcess process;
    process.start("fw_printenv");

    if(process.waitForFinished(2000))
    {
        QString output = process.readAll();
        int index = output.indexOf("800x480M@");
        if(index >= 0)
        {
            index += 9;
            QString rateStr = output.mid(index, 2);
            bool ok = false;
            int rate;
            rate = rateStr.toInt(&ok);
            if(ok)
            {
                curRate = rate;
            }
        }
    }

    _freshRateSpinBox->setValue(curRate);
#endif

    contentLayout->addStretch();

    contentLayout->setSpacing(10);
}

void FactoryTestMenu::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusNextPrevChild(false);
            return;
        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            return;
        default:
            break;
    }

    PopupWidget::keyPressEvent(e);
}

/**************************************************************************************************
 * 按钮槽函数。
 *************************************************************************************************/
void FactoryTestMenu::_btnReleased(int id)
{
    switch(id)
    {
#ifdef OUTPUT_TESTPACKET_THROUGH_USB
    case FACTORY_TEST_USB_HS:
        usbOutPutTestPacket(false);
        break;
    case FACTORY_TEST_USB_FS:
        usbOutPutTestPacket(true);
        break;
#endif
    default:
        break;
    }
}

#ifdef TEST_REFRESH_RATE
void FactoryTestMenu::_onFreshRateChanged(QString valStr)
{
    QString cmdStr = QString("fw_setenv vidargs video=mxcfb0:dev=ldb,800x480M@%1,if=RGB666 video=mxcfb1:off fbmem=8M").arg(valStr);
    QProcess::execute(cmdStr);
    QProcess::execute("sync");
    IMessageBox msgBox(trs("Note"), trs("RefreshRateUpdatedRebootNow"), true);
    if(msgBox.exec() == 1)
    {
        QProcess::execute("reboot");
    }
}
#endif

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
FactoryTestMenu::~FactoryTestMenu()
{

}

void FactoryTestMenu::readyShow()
{
    lbtn[0]->setFocus();
}



