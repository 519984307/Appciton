#include "SupervisorMailMenu.h"
#include "FontManager.h"
#include "LabelButton.h"
#include "IComboList.h"
#include "IMessageBox.h"
#include "ColorManager.h"
#include "IConfig.h"
#include "MailRecipientEditor.h"
#include "KeyBoardPanel.h"
#include "IListWidget.h"
#include "SupervisorMenuManager.h"

#include "Debug.h"

#define RECIPIENT_LIST_ITEM_H 30
#define RECIPIENT_LIST_ITEM_W 200
#define RECIPIENT_MAX_NUM 5
#define MAX_INPUT_TEXT_LENGTH 128

class SupervisorMailMenuPrivate {
public:
    Q_DECLARE_PUBLIC(SupervisorMailMenu)

    SupervisorMailMenuPrivate(SupervisorMailMenu * const q_ptr)
        :q_ptr(q_ptr), smtpServerBtn(NULL), smtpServerPortBtn(NULL), smtpUserBtn(NULL),
          smtpPasswordBtn(NULL), connectionSecurityCombo(NULL),
          lastSelectItem(NULL), recipientLists(NULL), addBtn(NULL),
          editBtn(NULL), delBtn(NULL){}

    void onConnectionSecuritySwitch(int type);
    void onRecipientItemClick();
    void onListExit(bool backtab);
    void onBtnClick();
    void onEditText();
    void loadRecipients();
    void saveRecipients();
    void updateRecipientList();

    SupervisorMailMenu * const q_ptr;
    LabelButton *smtpServerBtn;
    LabelButton *smtpServerPortBtn;
    LabelButton *smtpUserBtn;
    LabelButton *smtpPasswordBtn;
    IComboList *connectionSecurityCombo;
    QListWidgetItem *lastSelectItem;
    IListWidget *recipientLists;
    LButtonEx *addBtn;
    LButtonEx *editBtn;
    LButtonEx *delBtn;
    QVector<RecipientInfo> recipients;
};

void SupervisorMailMenuPrivate::onConnectionSecuritySwitch(int type)
{
    currentConfig.setNumValue("Mail|ConnectionSecurity", type);
    currentConfig.save();
}

/***************************************************************************************************
 * onProfileItemClick :  private slot, handle the recipient list item clickk signal
 **************************************************************************************************/
void SupervisorMailMenuPrivate::onRecipientItemClick()
{
    QListWidgetItem *item = recipientLists->currentItem();
    if(lastSelectItem)
    {
        lastSelectItem->setIcon(QIcon());
    }

    if(item != lastSelectItem)
    {
        item->setIcon(QIcon("/usr/local/iDM/icons/select.png"));
        lastSelectItem = item;
    }
    else
    {
        lastSelectItem = NULL;
    }

    if(lastSelectItem)
    {
        delBtn->setEnabled(true);
        editBtn->setEnabled(true);
    }
    else
    {
        delBtn->setEnabled(false);
        editBtn->setEnabled(false);
    }
}

/***************************************************************************************************
 * onListExit : private slot, handle IListWidget exitList event
 **************************************************************************************************/
void SupervisorMailMenuPrivate::onListExit(bool backtab)
{
    Q_Q(SupervisorMailMenu);
    if(backtab)
    {
        q->focusPreviousChild();
    }
    else
    {
        q->focusNextChild();
    }
}

/***************************************************************************************************
 * caseInsensitiveLessThan, use to compare profile name
 **************************************************************************************************/
bool caseInsensitiveLessThan(const RecipientInfo &info1, const RecipientInfo &info2)
{
    if(info1.name == info2.name)
    {
        return info1.address.toLower() < info2.address.toLower();
    }
    return info1.name < info2.name;
}

/***************************************************************************************************
 * onBtnClick : handle add, edit, delete button click signal
 **************************************************************************************************/
void SupervisorMailMenuPrivate::onBtnClick()
{
    Q_Q(SupervisorMailMenu);
    LButtonEx *sender = qobject_cast<LButtonEx *>(q->sender());
    if(sender == addBtn)
    {
        MailRecipientEditor editor;
        while(editor.exec())
        {
            //check exist mail address
            QVector<RecipientInfo>::ConstIterator iter;
            RecipientInfo recInfo = editor.getRecipientInfo();
            bool duplicated = false;
            for(iter = recipients.constBegin(); iter != recipients.constEnd(); iter++)
            {
                if(iter->address.toUpper() == recInfo.address.toUpper())
                {
                    duplicated = true;
                    break;
                }
            }

            if(duplicated)
            {
                QString title = trs("EmailAddressConflict");
                IMessageBox msgBox(title, QString("%1 %2 %3").arg(trs("EmailAddress")).arg(recInfo.address).arg(trs("AlreadyExist")), false);
                msgBox.exec();
                continue;
            }
            else
            {
                recipients.append(editor.getRecipientInfo());
                qSort(recipients.begin(), recipients.end(), caseInsensitiveLessThan);
                updateRecipientList();
                lastSelectItem = NULL;
                saveRecipients();
                break;
            }
        }
    }
    else if(sender == editBtn)
    {
        if(lastSelectItem)
        {
            int index = recipientLists->row(lastSelectItem);
            MailRecipientEditor editor(recipients[index]);
            while (editor.exec())
            {
                bool duplicate = false;
                QString address = editor.getRecipientInfo().address;
                for(int i = 0; i<recipients.size(); i++)
                {
                    if(i != index && address.toUpper() == recipients[i].address.toUpper())
                    {
                        duplicate = true;
                        break;
                    }
                }

                if(duplicate)
                {
                    QString title = trs("MailAddressConflict");
                    IMessageBox msgBox(title, QString("%1 %2 %3").arg(trs("MailAddress")).arg(address).arg(trs("AlreadyExist")), false);
                    msgBox.exec();
                    continue;
                }

                recipients[index] = editor.getRecipientInfo();
                recipientLists->item(index)->setText(QString("%1<%2>")
                                                     .arg(recipients[index].name)
                                                     .arg(recipients[index].address));

                QString edit_name(recipients[index].name);
                QString edit_address(recipients[index].address);

                qSort(recipients.begin(), recipients.end(), caseInsensitiveLessThan);
                updateRecipientList();

                saveRecipients();

                for(int i = 0; i < recipients.size(); ++i)
                {
                    if((recipients.at(i).name == edit_name) && (recipients.at(i).address == edit_address))
                    {
                        lastSelectItem = recipientLists->item(i);
                        break;
                    }
                }

                if(lastSelectItem)
                {
                    lastSelectItem->setIcon(QIcon("/usr/local/iDM/icons/select.png"));
                }

                break;
            }
        }
    }
    else if(sender == delBtn)
    {
        if(lastSelectItem)
        {
            IMessageBox messageBox(trs("Prompt"), trs("DeleteSelectedMailAddress"));
            if(messageBox.exec() == 0)
            {
                return;
            }

            int index = recipientLists->row(lastSelectItem);
            recipients.remove(index);
            updateRecipientList();
            saveRecipients();
            lastSelectItem = NULL;
            delBtn->setEnabled(false);
            editBtn->setEnabled(false);
        }

    }
}

/***************************************************************************************************
 * onEditText: handle lable button clicked signal
 **************************************************************************************************/
void SupervisorMailMenuPrivate::onEditText()
{
    Q_Q(SupervisorMailMenu);
    LButton *sender  = qobject_cast<LButton *>(q->sender());
    if(!sender)
    {
        return ;
    }

    KeyBoardPanel englishPanel;
    englishPanel.setInitString(sender->text());

    LabelButton *lblBtn = qobject_cast<LabelButton*>(sender->parent());

    if(lblBtn)
    {
        englishPanel.setTitleBarText(lblBtn->label->text());
    }

    englishPanel.setMaxInputLength(MAX_INPUT_TEXT_LENGTH);
    if(englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();

        if(sender != smtpPasswordBtn->button)
            text = text.trimmed();

        if(sender == smtpServerPortBtn->button)
        {
            bool ok = true;
            int num;
            num = text.toInt(&ok);
            if(!ok || num<=0 || num >=65536)
            {
                IMessageBox(QString(trs("Warn")), QString(trs("InvalidPortNum")), QStringList(trs("EnglishYESChineseSURE"))).exec();
                return;
            }
        }
        sender->setText(text);

        currentConfig.setStrValue(QString("Mail|%1").arg(sender->property("xmlNode").toString()), text);
        currentConfig.save();
    }
}


/***************************************************************************************************
 * loadRecipients, load the Recipients from the config file
 **************************************************************************************************/
void SupervisorMailMenuPrivate::loadRecipients()
{
    QString tmpStr;
    int count;
    bool ok;

    if(!currentConfig.getStrAttr("Mail|Recipients", "Count", tmpStr))
    {
        return;
    }

    count = tmpStr.toInt(&ok);
    if(!ok)
    {
        return;
    }

    recipients.clear();

    for(int i = 0; i< count; i++)
    {
        RecipientInfo recipientInfo;
        QString prefix = QString("Mail|Recipients|Recipient%1|").arg(i);
        currentConfig.getStrValue(prefix+"Address", recipientInfo.address);
        currentConfig.getStrValue(prefix+"Name", recipientInfo.name);
        recipients.append(recipientInfo);
    }
}

/***************************************************************************************************
 * saveRecipients: save the recipient, it will delete all the old recipientes and add the new ones.
 **************************************************************************************************/
void SupervisorMailMenuPrivate::saveRecipients()
{
    bool ok;
    QString tmpStr;
    if(!currentConfig.getStrAttr("Mail|Recipients", "Count", tmpStr))
    {
        return;
    }

    int count = tmpStr.toInt(&ok);
    if(!ok)
    {
        return;
    }

    //remove old recipient
    for(int i = 0; i <count; i++)
    {
        QString prefix = QString("Mail|Recipients|Recipient%1").arg(i);
        currentConfig.removeNode(prefix);
    }

    //add net profile
    for(int j = 0; j<recipients.count(); j++)
    {
        QString prefix("Mail|Recipients");
        QString nodeName = QString("Recipient%1").arg(j);
        currentConfig.addNode(prefix, nodeName);
        prefix = prefix + "|" + nodeName;
        currentConfig.addNode(prefix, "Address", recipients.at(j).address);
        currentConfig.addNode(prefix, "Name", recipients.at(j).name);
    }

    currentConfig.setStrAttr("Mail|Recipients", "Count", QString::number(recipients.count()));

    currentConfig.save();
}

/***************************************************************************************************
 * updateReciptentList : Update the recipient List item from config
 **************************************************************************************************/
void SupervisorMailMenuPrivate::updateRecipientList()
{
    //remove old item
    while(recipientLists->count())
    {
        QListWidgetItem *item = recipientLists->takeItem(0);
        delete item;
    }

    for(int i = 0; i < recipients.count(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(
                    QString("%1<%2>").arg(recipients.at(i).name).arg(recipients.at(i).address),
                    recipientLists);
        item->setSizeHint(QSize(RECIPIENT_LIST_ITEM_W, RECIPIENT_LIST_ITEM_H));
    }

    int count = recipientLists->count();
    if(count)
    {
        recipientLists->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        recipientLists->setFocusPolicy(Qt::NoFocus);
    }

    if(count >= RECIPIENT_MAX_NUM)
    {
        addBtn->setEnabled(false);
    }
    else
    {
        addBtn->setEnabled(true);
    }
}

/***************************************************************************************************
 * Singleton mode, get a instance
 **************************************************************************************************/
SupervisorMailMenu &SupervisorMailMenu::construction()
{
    static SupervisorMailMenu *_instance = NULL;
    if(_instance == NULL)
    {
        _instance =  new SupervisorMailMenu;
    }
    return *_instance;
}

SupervisorMailMenu::~SupervisorMailMenu()
{

}

/***************************************************************************************************
 * focusFirstItem : set the first widget to get focus
 **************************************************************************************************/
void SupervisorMailMenu::focusFirstItem()
{
    //has set the focus proxy, just set this widget foucs will make the proxy get focus
    this->setFocus();
}

/***************************************************************************************************
 * eventFilter : handle the profile list focus in event, the operation of the list will be more natrual.
 *              If we don't handle the focus in event, the focus item will be the last item of the list
 *              when the focus reason is tab, or the first item when the focus reason is back tab.
 *
 *              Handle the profile list hide event, clear the select item when the widget is hidden.
 **************************************************************************************************/
bool SupervisorMailMenu::eventFilter(QObject *obj, QEvent *ev)
{
    Q_D(SupervisorMailMenu);
    if(obj == d->recipientLists)
    {
        if (ev->type() == QEvent::FocusIn)
        {
            QFocusEvent *e = static_cast<QFocusEvent *>(ev);
            if(e->reason() == Qt::TabFocusReason)
            {
                d->recipientLists->setCurrentRow(0);
            }
            else if (e->reason() == Qt::BacktabFocusReason)
            {
                d->recipientLists->setCurrentRow(d->recipientLists->count() - 1);
            }
        }

        if (ev->type() == QEvent::Hide)
        {
            if(d->lastSelectItem)
            {
                d->lastSelectItem->setIcon(QIcon());
                d->delBtn->setEnabled(false);
                d->editBtn->setEnabled(false);
                d->lastSelectItem = NULL;
            }
        }
    }
    return false;
}

/***************************************************************************************************
 * layoutExec : layout issue
 **************************************************************************************************/
void SupervisorMailMenu::layoutExec()
{
    Q_D(SupervisorMailMenu);
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    QFont font = fontManager.textFont(15);

    int itemW = submenuW - ICOMBOLIST_SPACE - SCROLL_BAR_WIDTH; //need some space for the vertical scroll bar
    int btnWidth  = itemW / 2;
    int labelWidth = itemW - btnWidth;

    //smtp server
    d->smtpServerBtn = new LabelButton(trs("SmtpServer"));
    d->smtpServerBtn->setParent(this);
    this->setFocusProxy(d->smtpServerBtn);
    d->smtpServerBtn->setFont(font);
    d->smtpServerBtn->label->setFixedSize(labelWidth, ITEM_H);
    d->smtpServerBtn->button->setFixedSize(btnWidth, ITEM_H);
    d->smtpServerBtn->button->setProperty("xmlNode", "SmtpServer");
    connect(d->smtpServerBtn->button, SIGNAL(clicked()), this, SLOT(onEditText()));
    mainLayout->addWidget(d->smtpServerBtn);

    //port
    d->smtpServerPortBtn = new LabelButton(trs("SmtpServerPort"));
    d->smtpServerPortBtn->setFont(font);
    d->smtpServerPortBtn->label->setFixedSize(labelWidth, ITEM_H);
    d->smtpServerPortBtn->button->setFixedSize(btnWidth, ITEM_H);
    d->smtpServerPortBtn->button->setProperty("xmlNode", "SmtpServerPort");
    connect(d->smtpServerPortBtn->button, SIGNAL(clicked()), this, SLOT(onEditText()));
    mainLayout->addWidget(d->smtpServerPortBtn);

    //user
    d->smtpUserBtn = new LabelButton(trs("Username"));
    d->smtpUserBtn->setFont(font);
    d->smtpUserBtn->label->setFixedSize(labelWidth, ITEM_H);
    d->smtpUserBtn->button->setFixedSize(btnWidth, ITEM_H);
    d->smtpUserBtn->button->setProperty("xmlNode", "SmtpUsername");
    connect(d->smtpUserBtn->button, SIGNAL(clicked()), this, SLOT(onEditText()));
    mainLayout->addWidget(d->smtpUserBtn);

    //password
    d->smtpPasswordBtn = new LabelButton(trs("Password"));
    d->smtpPasswordBtn->setFont(font);
    d->smtpPasswordBtn->label->setFixedSize(labelWidth, ITEM_H);
    d->smtpPasswordBtn->button->setFixedSize(btnWidth, ITEM_H);
    d->smtpPasswordBtn->button->setProperty("xmlNode", "Password");
    connect(d->smtpPasswordBtn->button, SIGNAL(clicked()), this, SLOT(onEditText()));
    mainLayout->addWidget(d->smtpPasswordBtn);

    //connection security
    d->connectionSecurityCombo = new IComboList(trs("ConnectionSecurity"));
    d->connectionSecurityCombo->setFont(font);
    d->connectionSecurityCombo->addItem(trs("None"));
    d->connectionSecurityCombo->addItem(trs("TLS"));
    d->connectionSecurityCombo->addItem(trs("TTL"));
    d->connectionSecurityCombo->label->setFixedSize(labelWidth, ITEM_H);
    d->connectionSecurityCombo->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(d->connectionSecurityCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onConnectionSecuritySwitch(int)));
    mainLayout->addWidget(d->connectionSecurityCombo);


    //label
    QLabel *label = new QLabel();
    label->setFont(font);
    label->setContentsMargins(QMargins(15, 10, 0, 10));
    label->setText(trs("ConfiguredMailRecipients"));
    mainLayout->addWidget(label);

    //recipient List
    d->recipientLists = new IListWidget;
    d->recipientLists->setFont(font);
    d->recipientLists->setSelectionMode(QAbstractItemView::SingleSelection);
    d->recipientLists->setFrameStyle(QFrame::Plain);
    d->recipientLists->setSpacing(2);
    d->recipientLists->setUniformItemSizes(true);
    d->recipientLists->setIconSize(QSize(16,16));

    QString recipientListStyleSheet = QString("QListWidget { margin-left: 15px; border:1px solid #808080; border-radius: 2px; background-color: transparent; outline: none; }\n "
    "QListWidget::item {padding: 5px; border-radius: 2px; border: none; background-color: %1;}\n"
    "QListWidget::item:focus {background-color: %2;}").arg("white").arg(colorManager.getHighlight().name());

    d->recipientLists->setStyleSheet(recipientListStyleSheet);
    connect(d->recipientLists, SIGNAL(exitList(bool)), this, SLOT(onListExit(bool)));
    connect(d->recipientLists, SIGNAL(realRelease()), this, SLOT(onRecipientItemClick()));
    d->recipientLists->installEventFilter(this);
    d->recipientLists->setFixedHeight(174); //size for 5 items
    mainLayout->addWidget(d->recipientLists);

    //buttons
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setContentsMargins(QMargins(15, 10, 0, 40));
    hlayout->setSpacing(10);
    d->addBtn = new LButtonEx();
    d->addBtn->setText(trs("Add"));
    d->addBtn->setFont(font);
    hlayout->addWidget(d->addBtn);
    connect(d->addBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));
    d->editBtn = new LButtonEx();
    d->editBtn->setText(trs("Edit"));
    d->editBtn->setFont(font);
    d->editBtn->setEnabled(false);
    hlayout->addWidget(d->editBtn);
    connect(d->editBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));
    d->delBtn =  new LButtonEx();
    d->delBtn->setText((trs("Delete")));
    d->delBtn->setFont(font);
    d->delBtn->setEnabled(false);
    hlayout->addWidget(d->delBtn);
    connect(d->delBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));

    mainLayout->addLayout(hlayout);
}

/***************************************************************************************************
 * load configurations before show
 **************************************************************************************************/
void SupervisorMailMenu::readyShow()
{
    Q_D(SupervisorMailMenu);
    int securityType = 0;
    QString tmpStr;
    currentConfig.getNumValue("Mail|ConnectionSecurity", securityType);
    d->connectionSecurityCombo->setCurrentIndex(securityType);


    currentConfig.getStrValue("Mail|SmtpServer", tmpStr);
    d->smtpServerBtn->button->setText(tmpStr);
    tmpStr.clear();

    currentConfig.getStrValue("Mail|SmtpServerPort", tmpStr);
    d->smtpServerPortBtn->button->setText(tmpStr);
    tmpStr.clear();

    currentConfig.getStrValue("Mail|SmtpUsername", tmpStr);
    d->smtpUserBtn->button->setText(tmpStr);
    tmpStr.clear();

    currentConfig.getStrValue("Mail|Password", tmpStr);
    d->smtpPasswordBtn->button->setText(tmpStr);

    d->loadRecipients();
    d->updateRecipientList();
}

SupervisorMailMenu::SupervisorMailMenu()
    :SubMenu(trs("Mail")), d_ptr(new SupervisorMailMenuPrivate(this))
{
    setFocusPolicy(Qt::StrongFocus);
    setDesc(trs("MailSetting"));
    startLayout();
}

#include "moc_SupervisorMailMenu.cpp"
