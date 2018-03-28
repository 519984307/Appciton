#include "MailRecipientEditor.h"
#include "LabelButton.h"
#include "FontManager.h"
#include "IMessageBox.h"
#include "KeyBoardPanel.h"
#include <QRegExp>

#define DEFAULT_WIDGET_WIDTH 500
#define FONT_SIZE  14
#define MAX_INPUT_STRING 128

class MailRecipientEditorPrivate
{
public:
    Q_DECLARE_PUBLIC(MailRecipientEditor)
    MailRecipientEditorPrivate(MailRecipientEditor * const q_ptr)
        :q_ptr(q_ptr), nameBtn(NULL), addressBtn(NULL), cancelBtn(NULL),
          commitBtn(NULL){}

    void init(const RecipientInfo &info);
    void editText();
    void onCancel();
    void onCommit();

    MailRecipientEditor * const q_ptr;
    LabelButton *nameBtn;
    LabelButton *addressBtn;
    LButtonEx *cancelBtn;
    LButtonEx *commitBtn;
};

/***************************************************************************************************
 * init : do the layout issue
 **************************************************************************************************/
void MailRecipientEditorPrivate::init(const RecipientInfo &info)
{
    Q_Q(MailRecipientEditor);
    int submenuW = DEFAULT_WIDGET_WIDTH;
    int itemW = submenuW -  20;
    int btnWidth = itemW / 2;
    int labelWidth = btnWidth / 2;

    QFont font = fontManager.textFont(FONT_SIZE);

    q->setTitleBarText(trs("RecipientInfoEdit"));

    QVBoxLayout *contentLayout = new QVBoxLayout;
    q->contentLayout->addLayout(contentLayout);
    contentLayout->setContentsMargins(10, 4, 10, 4);
    contentLayout->setSpacing(2);

    nameBtn = new LabelButton(trs("Name"));
    nameBtn->setFont(font);
    nameBtn->setValue(info.name);
    nameBtn->label->setFixedWidth(labelWidth);
    nameBtn->button->setFixedWidth(btnWidth);
    q->connect(nameBtn->button, SIGNAL(clicked()), q, SLOT(editText()));
    contentLayout->addWidget(nameBtn);

    addressBtn = new LabelButton(trs("EmailAddress"));
    addressBtn->setFont(font);
    addressBtn->setValue(info.address);
    addressBtn->label->setFixedWidth(labelWidth);
    addressBtn->button->setFixedWidth(btnWidth);
    q->connect(addressBtn->button, SIGNAL(clicked()), q, SLOT(editText()));
    contentLayout->addWidget(addressBtn);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(20, 4, 20, 4);
    hlayout->setSpacing(20);
    commitBtn = new LButtonEx();
    commitBtn->setFont(font);
    commitBtn->setText(trs("Enter"));
    hlayout->addWidget(commitBtn);
    cancelBtn = new LButtonEx();
    cancelBtn->setFont(font);
    cancelBtn->setText(trs("Cancel"));
    hlayout->addWidget(cancelBtn);

    q->connect(commitBtn, SIGNAL(realReleased()), q, SLOT(onCommit()));
    q->connect(cancelBtn, SIGNAL(realReleased()), q, SLOT(onCancel()));

    contentLayout->addLayout(hlayout);

}

/***************************************************************************************************
 * editText : handle button click singal and edit text
 **************************************************************************************************/
void MailRecipientEditorPrivate::editText()
{
    Q_Q(MailRecipientEditor);
    LButton *sender = qobject_cast<LButton*>(q->sender());

    if(!sender)
    {
        return;
    }
    KeyBoardPanel englishPanel;

    englishPanel.setInitString(sender->text());

    LabelButton *lblBtn = qobject_cast<LabelButton*>(sender->parent());

    if(lblBtn)
    {
        englishPanel.setTitleBarText(lblBtn->label->text());
    }

    englishPanel.setMaxInputLength(MAX_INPUT_STRING);
    englishPanel.setInitString(sender->text());
    if(englishPanel.exec())
    {
        sender->setText(englishPanel.getStrValue().trimmed());
    }
}

/***************************************************************************************************
 * onCancel : handle cancel button clicked signal
 **************************************************************************************************/
void MailRecipientEditorPrivate::onCancel()
{
    Q_Q(MailRecipientEditor);
    q->done(0);
}

/***************************************************************************************************
 * onCommit : handle commit button clicked signal
 **************************************************************************************************/
void MailRecipientEditorPrivate::onCommit()
{
    Q_Q(MailRecipientEditor);
    QRegExp reg("^\\w+([-+.]\\w+)*@\\w+([-.]\\w+)*\\.\\w+([-.]\\w+)*$");
    if(!reg.exactMatch(addressBtn->button->text()))
    {
        IMessageBox(QString(trs("Warn")), QString(trs("InvalidEmailAddress")), QStringList(trs("EnglishYESChineseSURE"))).exec();
        return;
    }
    q->done(1);
}

MailRecipientEditor::MailRecipientEditor(const RecipientInfo &recipientInfo)
    :PopupWidget(), d_ptr(new MailRecipientEditorPrivate(this))
{
    Q_D(MailRecipientEditor);
    d->init(recipientInfo);
}

MailRecipientEditor::~MailRecipientEditor()
{

}

RecipientInfo MailRecipientEditor::getRecipientInfo() const
{
    Q_D(const MailRecipientEditor);
    return RecipientInfo(d->addressBtn->button->text(),
                   d->nameBtn->button->text());
}

/***************************************************************************************************
 * keyPressEvent : handle child widget focus issue
 **************************************************************************************************/
void MailRecipientEditor::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusNextPrevChild(false);
            break;

        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            break;
        default:
            break;
    }
    PopupWidget::keyPressEvent(event);
}

#include "moc_MailRecipientEditor.cpp"
