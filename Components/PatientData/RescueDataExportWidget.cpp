#include <QHBoxLayout>
#include "RescueDataListWidget.h"
#include "RescueDataExportWidget.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "WindowManager.h"
#include "IButton.h"
#include "IComboList.h"
#include "ExportDataWidget.h"
#include "DataStorageDirManager.h"
#include "USBManager.h"
#include "IMessageBox.h"
#include "LabelButton.h"
#include "NetworkManager.h"
#include "Debug.h"

#define ITEM_HEIGHT (31)

RescueDataExportWidget *RescueDataExportWidget::_selfObj = NULL;

/**********************************************************************************************************************
 * 构造。
 **********************************************************************************************************************/
RescueDataExportWidget::RescueDataExportWidget() : PopupWidget(),
    _type(TRANSFER_TYPE_USB)
{

    int maxw = windowManager.getPopMenuWidth();
    int maxh = windowManager.getPopMenuHeight();

    int fontSize = fontManager.getFontSize(1);
    _listWidget = new RescueDataListWidget(maxw - 20, maxh - 30 - 36);
    _listWidget->setShowCurRescue(false);

    _transferType = new IComboList(trs("TransferType"));
    _transferType->label->setFixedHeight(ITEM_HEIGHT);
    _transferType->combolist->setFixedHeight(ITEM_HEIGHT);
    _transferType->setFont(fontManager.textFont(fontSize));
    _transferType->addItem(trs("USB"));
    if (systemManager.isSupport(CONFIG_WIFI))
    {
        _transferType->addItem(trs("SFTP"));
    }
    _transferType->setCurrentIndex(0);
    connect(_transferType, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_transferTypeReleased(int)));

    _transferSelect = new LButtonEx();
    _transferSelect->setFixedHeight(ITEM_HEIGHT);
    _transferSelect->setFont(fontManager.textFont(fontSize));
    _transferSelect->setText(trs("TransferSelect"));
    connect(_transferSelect, SIGNAL(realReleased()), this, SLOT(_transferSelectReleased()));

    _transferAll = new LButtonEx();
    _transferAll->setFixedHeight(ITEM_HEIGHT);
    _transferAll->setFont(fontManager.textFont(fontSize));
    _transferAll->setText(trs("TransferAll"));
    connect(_transferAll, SIGNAL(realReleased()), this, SLOT(_transferAllReleased()));

    // space label
    _spaceLabel = new QLabel();
    _spaceLabel->setVisible(false);

    _up = new IButton();
    _up->setFixedSize(ITEM_HEIGHT - 1, ITEM_HEIGHT - 1);
    _up->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(_up, SIGNAL(realReleased()), this, SLOT(_upReleased()));

    _down = new IButton();
    _down->setFixedSize(ITEM_HEIGHT - 1, ITEM_HEIGHT - 1);
    _down->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(_down, SIGNAL(realReleased()), this, SLOT(_downReleased()));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(2);
    layout->addWidget(_transferType, 3);
    layout->addWidget(_transferSelect, 2);
    layout->addWidget(_transferAll, 2);
    layout->addWidget(_spaceLabel, 2);
    layout->addWidget(_up, 1);
    layout->addWidget(_down, 1);

    contentLayout->setSpacing(1);
    contentLayout->addWidget(_listWidget);
    contentLayout->addLayout(layout);

    _listWidget->setScrollbarVisiable(false);
    connect(_listWidget, SIGNAL(pageInfoChange()), this, SLOT(_updateWindowTitle()));

    setFixedSize(maxw, maxh);
}

/**********************************************************************************************************************
 * 析构。
 **********************************************************************************************************************/
RescueDataExportWidget::~RescueDataExportWidget()
{

}

/**********************************************************************************************************************
 * 按键事件。
 **********************************************************************************************************************/
void RescueDataExportWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Left:
        case Qt::Key_Up:
            focusNextPrevChild(false);
            break;
        case Qt::Key_Right:
        case Qt::Key_Down:
            focusNextChild();
            break;
        default:
            break;
    }
}

/**********************************************************************************************************************
 * 传输类型回调。
 **********************************************************************************************************************/
void RescueDataExportWidget::_transferTypeReleased(int index)
{
    _type = (TransferType)index;

    if (_type == TRANSFER_TYPE_WIFI)
    {
        _transferAll->setVisible(false);
        _spaceLabel->setVisible(true);
    }
    else
    {
        _transferAll->setVisible(true);
        _spaceLabel->setVisible(false);
    }
}

/**********************************************************************************************************************
 * 传输回调。
 **********************************************************************************************************************/
void RescueDataExportWidget::_transferSelectReleased()
{
    QStringList list;

    _listWidget->getStrList(list);
    if(list.isEmpty())
    {
        IMessageBox msgbox(trs("Prompt"), trs("NoIncidents"), QStringList(trs("EnglishYESChineseSURE")));
        msgbox.exec();
        return;
    }

    _listWidget->getCheckList(list);
    if (list.empty())
    {
        IMessageBox msgbox(trs("Prompt"), trs("SelectIncidents"), QStringList(trs("EnglishYESChineseSURE")));
        msgbox.exec();
        return;
    }

    _transferData(list);
}

/**********************************************************************************************************************
 * 传输回调。
 **********************************************************************************************************************/
void RescueDataExportWidget::_transferAllReleased()
{
    QStringList list;
    _listWidget->getStrList(list);
    if (list.empty())
    {
        IMessageBox msgbox(trs("Prompt"), trs("NoIncidents"), QStringList(trs("EnglishYESChineseSURE")));
        msgbox.exec();
        return;
    }

    _transferData(list);
}

/**********************************************************************************************************************
 * 上翻页。
 **********************************************************************************************************************/
void RescueDataExportWidget::_upReleased()
{
    if (NULL != _listWidget)
    {
        _listWidget->pageChange(true);
    }
}

/**********************************************************************************************************************
 * 下翻页。
 **********************************************************************************************************************/
void RescueDataExportWidget::_downReleased()
{
    if (NULL != _listWidget)
    {
        _listWidget->pageChange(false);
    }
}

void RescueDataExportWidget::_updateWindowTitle()
{
    QString str;
    if (languageManager.getCurLanguage() == LanguageManager::English)
    {
        str = QString("%1 (page %2 of %3)")
                .arg(trs("TransferSelectedRescue"))
                .arg(_listWidget->getCurPage() + 1)
                .arg(_listWidget->getTotalPage() == 0 ? 1 : _listWidget->getTotalPage());
    }
    else
    {
        str = QString("%1 (%2/%3)")
                .arg(trs("TransferSelectedRescue"))
                .arg(_listWidget->getCurPage() + 1)
                .arg(_listWidget->getTotalPage() == 0 ? 1 : _listWidget->getTotalPage());
    }
    setTitleBarText(str);
}

/**********************************************************************************************************************
 * 下翻页。
 **********************************************************************************************************************/
#if 0
bool RescueDataExportWidget::_transferData(QStringList &list)
{
    //wifi传输暂未实现
    if (_type == TRANSFER_TYPE_WIFI)
    {
        return false;
    }

    if (!udiskManager.isUSBExist())
    {
        IMessageBox messageBox(trs("Warn"), trs("WarningNoUSB"), QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
        return false;
    }

    if (list.empty())
    {
        return false;
    }

    QStringList strList;
    _listWidget->getStrList(strList);
    if (strList.empty())
    {
        return false;
    }

    int selectCount = list.count();
    int totalCount = strList.count();
    QStringList incidentDirList;
    int index = 0;
    QString srcDir;
    QString tmpStr;
    for (int i = 0; i < selectCount; ++i)
    {
        tmpStr = list.at(i);
        index = strList.indexOf(tmpStr);
        if ((-1 == index) || (index >= totalCount))
        {
            continue;
        }

        //strList does not include the current folder. must plus 1 to skip the current folder
        srcDir = dataStorageDirManager.getRescueDataDir(index + 1, tmpStr);
        if (srcDir.isEmpty())
        {
            continue;
        }

        incidentDirList.append(srcDir);

    }

    if(_type == TRANSFER_TYPE_USB)
    {
        ExportDataWidget exportDataWidget(EXPORT_RESCUE_DATA_BY_USB);
        connect(&udiskManager, SIGNAL(exportProcessChanged(unsigned char)), &exportDataWidget, SLOT(setBarValue(unsigned char)));
        connect(&udiskManager, SIGNAL(exportError()), &exportDataWidget, SLOT(reject()));
        connect(&exportDataWidget, SIGNAL(cancel()), &udiskManager, SLOT(cancelExport()));

        //start exporting
        udiskManager.exportFullDisclosureItems(incidentDirList);

        if (0 == exportDataWidget.exec())
        {
            QString msg;
            UdiskManager::ExportStatus status = udiskManager.getExportStatus();
            if (status == UdiskManager::Cancel)
            {
                msg = trs("TransferCancel");
            }
            else if (status == UdiskManager::Disconnect)
            {
                msg = trs("TransferDisconnect");
            }
            else if (status == UdiskManager::NoSpace)
            {
                msg = trs("WarnLessUSBFreeSpace");
            }
            else
            {
                msg = trs("TransferFailed");
            }
            IMessageBox messageBox(trs("Warn"), msg, QStringList(trs("EnglishYESChineseSURE")));
            messageBox.exec();
        }
    }
    else if (_type == TRANSFER_TYPE_WIFI)
    {
        //TODO: add wifi export function
    }
    else
    {
        qdebug("Unknow Export Type.");
    }

    return true;
}
#else
bool RescueDataExportWidget::_transferData(QStringList &list)
{
    if (list.empty())
    {
        return false;
    }

    QStringList strList;
    _listWidget->getStrList(strList);
    if (strList.empty())
    {
        return false;
    }

    int selectCount = list.count();
    int totalCount = strList.count();
    QStringList incidentDirList;
    int index = 0;
    QString srcDir;
    QString tmpStr;
    for (int i = 0; i < selectCount; ++i)
    {
        tmpStr = list.at(i);
        index = strList.indexOf(tmpStr);
        if ((-1 == index) || (index >= totalCount))
        {
            continue;
        }

        //strList does not include the current folder. must plus 1 to skip the current folder
        srcDir = dataStorageDirManager.getRescueDataDir(index + 1, tmpStr);
        if (srcDir.isEmpty())
        {
            continue;
        }

        incidentDirList.append(srcDir);

    }

    if(_type == TRANSFER_TYPE_USB)
    {
        if (!usbManager.isUSBExist())
        {
            IMessageBox messageBox(trs("Warn"), trs("WarningNoUSB"), QStringList(trs("EnglishYESChineseSURE")));
            messageBox.exec();
            return false;
        }

        ExportDataWidget exportDataWidget(EXPORT_RESCUE_DATA_BY_USB);
        connect(&usbManager, SIGNAL(exportProcessChanged(unsigned char)), &exportDataWidget, SLOT(setBarValue(unsigned char)));
        connect(&usbManager, SIGNAL(exportError()), &exportDataWidget, SLOT(reject()));
        connect(&exportDataWidget, SIGNAL(cancel()), &usbManager, SLOT(cancelExport()));
    }
    else if (_type == TRANSFER_TYPE_WIFI)
    {
        if(!networkManager.isWifiWorking() || (!networkManager.isWiFiConnected()))
        {
            IMessageBox messageBox(trs("Warn"), trs("NetworkDisconnected"), QStringList(trs("Yes")));
            messageBox.exec();
            return false;
        }
    }
    else
    {
        qdebug("Unknow Export Type.");
        return false;
    }

    return true;
}
#endif
