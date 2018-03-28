#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QKeyEvent>
#include "EmailAddressLayout.h"
#include "WindowManager.h"
#include "LanguageManager.h"
#include "ITableWidget.h"
#include "IButton.h"
#include "FontManager.h"
#include "MailManager.h"
#include "IMessageBox.h"
#include "IConfig.h"
#include "ECG12LeadManager.h"
#include "MenuManager.h"


MailAddressWidget *MailAddressWidget::_selfObj = NULL;


MailAddressWidget::MailAddressWidget():PopupWidget()
{
    setTitleBarText(trs("E-mailAddressList"));
    _maxWidth = windowManager.getPopMenuWidth();
    _maxHeight = windowManager.getPopMenuHeight();

    setFixedSize(_maxWidth*3/4, _maxHeight -92);
    int fontSize = fontManager.getFontSize(1);

    table = new ITableWidget();
    table->setRowCount(_rowNR);
    table->setColumnCount(_colNR);
    table->setFixedHeight(getMaxHeight()-124);// 固定尺寸。
    table->horizontalHeader()->setStyleSheet("QHeaderView::section {border:0px;height:30px;}");
    table->verticalHeader()->setVisible(false);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(table, SIGNAL(itemEnter(int)), this, SLOT(_updateCheckFlag(int)));

    QStringList hheader;
    QTableWidgetItem *item;
    int itemCheckWidth = ((_maxWidth * 3 / 4 - 10) / 10);
    int itemnameWidth = itemCheckWidth * 2;
    int itemmailWidth = itemCheckWidth*5;
    int itemNumWidth = itemCheckWidth * 2;
    // 创建Item。
    for (int i = 0; i < _rowNR; i++)
    {
        for (int j = 0; j < _colNR; j++)
        {
            switch(j)
            {
            case 0:
                table->setColumnWidth(j, itemNumWidth);
               break;
            case 1:
                table->setColumnWidth(j, itemnameWidth);
                break;
            case 2:
                table->setColumnWidth(j, itemmailWidth);
                break;
            case 3:
                table->setColumnWidth(j, itemCheckWidth);
                break;
            }
            item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, j, item);
        }

        table->setRowHeight(i, _tableItemHeight);
    }
    hheader << trs("NumID") << trs("Name") << trs("MailAddress") <<  QString("");
    table->setHorizontalHeaderLabels(hheader);


    _email = new IButton();
    _email->setFixedHeight(26);
    _email->setFont(fontManager.textFont(fontSize));
    _email->setText(trs("E-Mail"));
    _email->setFixedWidth(150);
    _email->setBorderEnabled(true);
    connect(_email, SIGNAL(realReleased()), this, SLOT(_mailReleased()));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(1);
    layout->setSpacing(2);
    layout->addWidget(table);
    layout->addWidget(_email, 0, Qt::AlignCenter);
    contentLayout->addLayout(layout);

    //默认第一个为初始选项
    _checkFlag.clear();
    _loadData();
    int index = 0;
    systemConfig.getNumValue("PrimaryCfg|ECG12Lead|EmailOffset", index);
    if(_checkFlag.count() > 0)
    {
        item = table->item(index,3);
        table->setCheck(item);
        _checkFlag[0] = true;

    }

    _addressoffest = index;
}

/***************************************************************************************************
 * 重构基类中的退出函数
 ***************************************************************************************************/
void MailAddressWidget::_exit()
{
    _checkFlag.clear();

    hide();
}

/***************************************************************************************************
 * 析构函数
 ***************************************************************************************************/
MailAddressWidget::~MailAddressWidget()
{

}

/***************************************************************************************************
 * 更新选择标志
 ***************************************************************************************************/
void MailAddressWidget::_updateCheckFlag(int index)
{
    if (index < 0 || index >= _checkFlag.count())
    {
        return;
    }
    for(int j = 0;j < _checkFlag.count();j++)
    {
        _checkFlag[j] = false;
    }
    _checkFlag[index] = !_checkFlag[index];
    for (int i = 0; i < _rowNR; i++)
    {
         QTableWidgetItem *item = table->item(i, 3);
         if(index == i)
         {
             _addressoffest = index;
             systemConfig.setNumValue("PrimaryCfg|ECG12Lead|EmailOffset", index);
             table->setCheck(item);
         }
         else
         {
             table->clearCheck(item);
         }
    }
}

/***************************************************************************************************
 * 获取E-mail地址偏移量
 ***************************************************************************************************/
int MailAddressWidget::getEmailAddressOffset()
{
    return _addressoffest;
}

/***************************************************************************************************
 *显示Email地址
 ***************************************************************************************************/
void MailAddressWidget:: _showMailPath()
{
    MailManager::Mail mail;
    QString str;

    for(int i = 0;i < _totalData; i++)
    {
        mail.recipient = mailManager.getRecipientInfos().at(i);

        table->item(i, 0)->setText(QString::number(i+1));
        table->item(i, 1)->setText(mail.recipient.name);
        table->item(i, 2)->setText(mail.recipient.address);
    }

}

/***************************************************************************************************
 *发E-mail按钮
 ***************************************************************************************************/
void MailAddressWidget:: _mailReleased()
{
    emit startToSend();

    return ;
}

/***************************************************************************************************
 * 发E-mail按钮
 ***************************************************************************************************/
void MailAddressWidget::_loadData()
{
    //清除当前页所有页数
    for (int i = 0; i < _rowNR; i++)
    {
        for (int j = 0; j < _colNR; j++)
        {
            QTableWidgetItem *item = table->item(i, j);
            if (j < (_colNR - 1))
            {
                item->setText(" ");
            }
//            else
//            {
//                table->clearCheck(item);
//            }
        }
    }
    if(mailManager.getRecipientInfos().count() == 0)
        return ;
    _totalData = mailManager.getRecipientInfos().count();
    //无数据
    if(_totalData <=0)
    {
        return;
    }
    while (_checkFlag.count() < _totalData)
    {
        _checkFlag.append(false);
    }
    //读取数据并且现实
    _showMailPath();
}

/***************************************************************************************************
 *判断是否是实时发送
 ***************************************************************************************************/
bool MailAddressWidget::_isRealTimeTransfer(const QVariant &para)
{
    QList<CompressPackage>  *pdfCompresspage = (QList<CompressPackage>  *)para.value<void *>();
    if(pdfCompresspage->count() == 0)
    {
        return FALSE;
    }
    if((pdfCompresspage->at(0).isrealTime) && pdfCompresspage->count() == 1)
    {
        return TRUE;
    }

    return FALSE;
}

/**************************************************************************************************
 * * 函数说明：焦点左右聚焦操作
 *************************************************************************************************/
void MailAddressWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusPreviousChild();
            break;
        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            break;
        default:
            break;
    }

    PopupWidget::keyPressEvent(e);
}

/**************************************************************************************************
 * *自动显示
 *************************************************************************************************/
void MailAddressWidget::showEvent(QShowEvent *e)
{
    _loadData();
    PopupWidget::showEvent(e);
    setCloseBtnFocus();
}


