/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/23
 **/

#include "FactoryImportExportMenuContent.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "Button.h"
#include <QList>
#include <QListWidgetItem>
#include <QDomDocument>
#include <QTextStream>
#include "MessageBox.h"
#include "ConfigManagerMenuWindow.h"
#include <QProcess>
#include "ImportFileSubWidget.h"
#include <QDir>
#include "ListView.h"
#include "ListDataModel.h"
#include "ListViewItemDelegate.h"
#include "LanguageManager.h"
#include "USBManager.h"
#include "ThemeManager.h"
#include <QTimer>

#define CONFIG_MAX_NUM        (6)
#define FILE_PATH             ("/media/usbdisk/etc")
#define CONFIG_DIR            ("/usr/local/nPM/etc/")
#define CONFIG_FILE_ADULT     ("AdultConfig.Original.xml")
#define CONFIG_FILE_NEO       ("NeoConfig.Original.xml")
#define CONFIG_FILE_PED       ("PedConfig.Original.xml")
#define CONFIG_FILE_SYSTEM    ("System.Original.xml")
#define CONFIG_FILE_MACHINE   ("Machine.xml")
#define LISTVIEW_MAX_VISIABLE_TIME (5)
#define HEIGHT_HINT (themeManger.getAcceptableControlHeight())

enum TransferResult
{
    TRANSFER_FAIL,
    TRANSFER_SUCCESS,
    TRANSFER_IGNORE
};

class FactoryImportExportMenuContentPrivate
{
public:
    FactoryImportExportMenuContentPrivate()
                    : configListView(NULL)
                    , configDataModel(NULL)
                    , exportBtn(NULL)
                    , importBtn(NULL)
                    , infoLab(NULL)
                    , timer(NULL)
                    , message(NULL)
    {
        fileNameList.append(CONFIG_FILE_ADULT);
        fileNameList.append(CONFIG_FILE_NEO);
        fileNameList.append(CONFIG_FILE_PED);
        fileNameList.append(CONFIG_FILE_SYSTEM);
        fileNameList.append(CONFIG_FILE_MACHINE);
    }

    /**
     * @brief exportFileToUSB 导出配置文件到usb
     * @return  返回传输结果
     */
    TransferResult exportFileToUSB();

    /**
     * @brief insertFileFromUSB    从usb设备导入配置文件
     * @return 返回传输结果
     */
    TransferResult insertFileFromUSB();

    /**
     * @brief loadConfigs  装载配置
     */
    void loadConfigs();

    /**
     * @brief updateConfigList  更新配置列表
     */
    void updateConfigList();


    QStringList fileNameList;
    QStringList  exportFileName;  // 导出的XML文件名称
    QStringList  importFileName;  // 导入的XML文件名称
    QDomDocument importXml;  // 导入文件内容
    ListView  *configListView;
    ListDataModel *configDataModel;
    Button *exportBtn;
    Button *importBtn;
    QLabel *infoLab;  // 记录usb是否存在信息
    QTimer *timer;
    MessageBox *message;
};


TransferResult FactoryImportExportMenuContentPrivate::exportFileToUSB()
{
    // 判断u盘是否存在
    if (!usbManager.isUSBExist())
    {
        MessageBox message(trs("Import"), trs("WarningNoUSB"), false);
        message.exec();
        return TRANSFER_IGNORE;
    }

    // 如果没有对应文件夹，则主动创建此文件夹
    QDir dir;
    dir.mkdir(FILE_PATH);

    // 获取所有的导出文件名称
    exportFileName.clear();
    int rowCount = configDataModel->getRowCount();
    QMap<int, bool> selectedMap = configListView->getRowsSelectMap();
    for (int i = 0; i < rowCount; i++)
    {
        if (selectedMap[i])
        {
            exportFileName.append(fileNameList.at(i));
        }
    }

    // 开始导出数据
    int count = exportFileName.count();
    bool transferSuccess = false;
    for (int i = 0; i < count; i++)
    {
        QString name = exportFileName.at(i);
        // 判断是否存在同名文件
        bool isExist = false;
        if (QFile::exists(QString("%1/%2").arg(FILE_PATH).arg(name)))
        {
            isExist = true;
        }

        // 弹出是否覆盖的提示框
        QDialog::DialogCode status = QDialog::Accepted;
        if (isExist)
        {
            MessageBox message(trs("Export"),
                               trs(QString("%1\r\n%2?").arg(trs(name)).arg(trs("FileExistsOverWrite"))),
                               QStringList() << trs("Cancel") << trs("Repeated"));
            this->message = &message;
            status = static_cast<QDialog::DialogCode>(message.exec());
            this->message = NULL;
        }

        // 复制文件到usb设备
        bool copyOk = true;
        if (status == QDialog::Accepted)
        {
            QString fileName = QString("%1%2").arg(CONFIG_DIR).arg(name);
            QString newFileName = QString("%1/%2").arg(FILE_PATH).arg(name);
            QFile::remove(newFileName);
            copyOk = QFile::copy(fileName, newFileName);
            if (copyOk == true)
            {
                transferSuccess = true;
            }
        }

        // 复制文件失败时弹出提示框
        if (copyOk == false)
        {
            MessageBox message(trs("Export"),
                               trs(QString("%1\r\n%2").arg(trs(name)).arg(trs("ExportFailed"))),
                               QStringList() << trs("Yes"));
            message.exec();
            return TRANSFER_FAIL;
        }
    }

    if (transferSuccess == true)
    {
        return TRANSFER_SUCCESS;
    }
    return TRANSFER_IGNORE;
}

TransferResult FactoryImportExportMenuContentPrivate::insertFileFromUSB()
{
    // 判断u盘是否存在
    if (!usbManager.isUSBExist())
    {
        MessageBox message(trs("Import"), trs("WarningNoUSB"), false);
        message.exec();
        return TRANSFER_IGNORE;
    }

    // 判断导入文件是否为空
    QDir dir(QString("%1%2").arg(FILE_PATH).arg("/"));
    QStringList files = dir.entryList(fileNameList, QDir::Files | QDir::Readable, QDir::Name);
    if (files.isEmpty())
    {
        MessageBox messageBox(trs("Import"), trs("PleaseAddImportFiles"), false);
        this->message = &messageBox;
        messageBox.exec();
        this->message = NULL;
        return TRANSFER_IGNORE;
    }
    QStringList nameList;
    for (int i = 0 ; i < files.count(); i++)
    {
        nameList.append(trs(files.at(i)));
    }

    // 获取导入文件名称
    importFileName.clear();
    ImportSubWidget subW(nameList, FILE_PATH);
    QDialog::DialogCode status = static_cast<QDialog::DialogCode>(subW.exec());
    if (status == QDialog::Accepted)
    {
        QMap<int, bool> rowsMap = subW.readRowsMap();
        for (int i = 0; i < files.count(); i++)
        {
            if (rowsMap[i])
            {
                importFileName.append(files.at(i));
            }
        }
    }

    // 未选取任何文件，直接退出
    if (importFileName.isEmpty())
    {
        return TRANSFER_IGNORE;
    }

    // 开始导入文件
    int count = importFileName.count();
    bool importSuccess = false;
    for (int i = 0; i < count; i++)
    {
        // 加载导入文件
        QString name = importFileName.at(i);
        QFile file(QString("%1/%2").arg(FILE_PATH).arg(name));
        if (false == file.open(QFile::ReadOnly | QFile::Text))
        {
            return TRANSFER_FAIL;
        }
        if (false == importXml.setContent(&file))
        {
            return TRANSFER_FAIL;
        }
        file.close();

        // 判断是否存在同名文件,如果存在则弹出选择提示框
        bool flag = true;
        bool isExist = false;
        if (true == QFile::exists(QString("%1%2").arg(CONFIG_DIR).arg(name)))
        {
            isExist = true;
        }
        if (true == isExist)
        {
            MessageBox message(trs("Import"),
                               trs(QString("%1\r\n%2?").arg(trs(name)).arg(trs("FileExistsOverWrite"))),
                               QStringList() << trs("Cancel") << trs("Repeated"));
            this->message = &message;
            flag = message.exec();
            this->message = NULL;
        }

        // 从usb设备复制文件到文件系统中
        bool copyOk = true;
        if (true == flag)
        {
            QString fileName = QString("%1/%2").arg(FILE_PATH).arg(name);
            QString newFileName = QString("%1%2").arg(CONFIG_DIR).arg(name);
            QFile::remove(newFileName);
            copyOk = QFile::copy(fileName, newFileName);
            if (copyOk == true)
            {
                importSuccess = true;
            }
        }

        // 如果文件复制不成功,弹出提示框
        if (false == copyOk)
        {
            MessageBox message(trs("Import"),
                               trs(QString("%1\r\n%2").arg(trs(name)).arg(trs("ImportFailed"))),
                               QStringList() << trs("Yes"));
            message.exec();
            return TRANSFER_IGNORE;
        }
    }

    // 更新配置列表
    updateConfigList();

    if (importSuccess == true)
    {
        return TRANSFER_SUCCESS;
    }
    return TRANSFER_IGNORE;
}

void FactoryImportExportMenuContentPrivate::loadConfigs()
{
    QStringList list;
    for (int i = 0 ; i < fileNameList.count(); i++)
    {
        list.append(trs(fileNameList.at(i)));
    }
    configDataModel->setStringList(list);
}

void FactoryImportExportMenuContentPrivate::updateConfigList()
{
    int curSelectedRow = configListView->curCheckedRow();
    bool isEnable;
    if (curSelectedRow == -1)
    {
        isEnable = false;
    }
    else
    {
        isEnable = true;
    }
    exportBtn->setEnabled(isEnable);

    if (configDataModel->getRowCount() >= CONFIG_MAX_NUM)
    {
        isEnable = false;
    }
    else
    {
        isEnable = true;
    }
    importBtn->setEnabled(isEnable);
}

void FactoryImportExportMenuContent::onTimeOut()
{
    bool isEnable;
    if (usbManager.isUSBExist())
    {
        isEnable = true;
        d_ptr->infoLab->hide();
    }
    else
    {
        if (d_ptr->message != NULL)
        {
            QMetaObject::invokeMethod(d_ptr->message, "done", Q_ARG(int, 0));
            d_ptr->message = NULL;
        }
        isEnable = false;
        d_ptr->infoLab->show();
    }
    updateBtnStatus();
    d_ptr->importBtn->setEnabled(isEnable);
}

FactoryImportExportMenuContent::FactoryImportExportMenuContent()
    : MenuContent(trs("ExportImport"), trs("ExportImportDesc")),
      d_ptr(new FactoryImportExportMenuContentPrivate)

{
}

FactoryImportExportMenuContent::~FactoryImportExportMenuContent()
{
    delete d_ptr;
}

void FactoryImportExportMenuContent::layoutExec()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignTop);

    ListView *listView = new ListView();
    listView->setItemDelegate(new ListViewItemDelegate(listView));
    layout->addWidget(listView);
    ListDataModel *model = new ListDataModel(this);
    d_ptr->configDataModel = model;
    listView->setFixedHeight(LISTVIEW_MAX_VISIABLE_TIME * model->getRowHeightHint()
                             + listView->spacing() * (LISTVIEW_MAX_VISIABLE_TIME * 2));
    listView->setModel(model);
    listView->setRowsSelectMode(true);
    connect(listView, SIGNAL(enterSignal()), this, SLOT(updateBtnStatus()));
    d_ptr->configListView = listView;

    QHBoxLayout *hl = new QHBoxLayout;
    Button *button;

    button = new Button(trs("Export"));
    button->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(button);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    d_ptr->exportBtn = button;

    button = new Button(trs("Import"));
    button->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(button);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    d_ptr->importBtn = button;

    layout->addLayout(hl);
    layout->addStretch(1);

    QLabel *label = new QLabel(trs("WarningNoUSB"));
    label->setFixedHeight(HEIGHT_HINT);
    d_ptr->infoLab = label;
    layout->addWidget(label, 1, Qt::AlignRight);

    layout->addStretch(1);

    // 需要加入this，绑定父子关系，以便自动释放空间,避免造成内存泄露
    QTimer *timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    timer->start();
    d_ptr->timer = timer;
}

void FactoryImportExportMenuContent::readyShow()
{
    d_ptr->loadConfigs();
    d_ptr->updateConfigList();
}

void FactoryImportExportMenuContent::showEvent(QShowEvent *ev)
{
    MenuContent::showEvent(ev);

    d_ptr->timer->start();
    onTimeOut();
}

void FactoryImportExportMenuContent::hideEvent(QHideEvent *ev)
{
    MenuContent::hideEvent(ev);

    d_ptr->timer->stop();
}



void FactoryImportExportMenuContent::onBtnClick()
{
    Button *btn = qobject_cast<Button *>(sender());

    if (btn == d_ptr->exportBtn)
    {
        TransferResult  result =  d_ptr->exportFileToUSB();
        if (result == TRANSFER_SUCCESS)
        {
            MessageBox messageBox(trs("Export"), trs("ExportFileCompleted"), false);
            d_ptr->message = &messageBox;
            messageBox.exec();
            d_ptr->message = NULL;
        }
    }
    else if (btn == d_ptr->importBtn)
    {
        TransferResult result = d_ptr->insertFileFromUSB();
        switch (result)
        {
            case TRANSFER_FAIL:
            {
                MessageBox messageBox(trs("Import"), trs("ImportFileFailed"), false);
                messageBox.exec();
            }
            break;
            case TRANSFER_IGNORE:
            break;
            case TRANSFER_SUCCESS:
            {
                MessageBox messageBox(trs("Import"), trs("ImportFileCompleted"), false);
                d_ptr->message = &messageBox;
                messageBox.exec();
                d_ptr->message = NULL;
            }
            break;
        }
    }
}


void FactoryImportExportMenuContent::updateBtnStatus()
{
    int curSelectedRow = d_ptr->configListView->curCheckedRow();
    bool isEnable;
    if (curSelectedRow == -1
            || !usbManager.isUSBExist())
    {
        isEnable = false;
    }
    else
    {
        isEnable = true;
    }
    d_ptr->exportBtn->setEnabled(isEnable);
}
