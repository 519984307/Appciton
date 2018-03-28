#pragma once
#include "PopupWidget.h"
#include "ErrorLogItem.h"
#include <QTextEdit>

class ErrorLogViewer : public PopupWidget
{
    Q_OBJECT
public:
    ErrorLogViewer();
    ErrorLogViewer(ErrorLogItemBase *item);

    void setText(const QString &text);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

private:
    ErrorLogItemBase * const _item;
    QTextEdit *_textEdit;
};
