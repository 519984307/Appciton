#pragma once
#include "PopupWidget.h"
#include <QScopedPointer>

struct RecipientInfo {
    RecipientInfo(){}
    RecipientInfo(const QString &address, const QString &name):address(address), name(name){}
    QString address;
    QString name;
};

class MailRecipientEditorPrivate;
class MailRecipientEditor: public PopupWidget
{
    Q_OBJECT
public:
    MailRecipientEditor(const RecipientInfo &recipientInfo = RecipientInfo());
    ~MailRecipientEditor();
    RecipientInfo getRecipientInfo() const;
protected:
    void keyPressEvent(QKeyEvent *event);
    const QScopedPointer<MailRecipientEditorPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MailRecipientEditor)
    Q_DECLARE_PRIVATE(MailRecipientEditor)
    Q_PRIVATE_SLOT(d_func(), void editText())
    Q_PRIVATE_SLOT(d_func(), void onCancel())
    Q_PRIVATE_SLOT(d_func(), void onCommit())
};
