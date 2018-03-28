#pragma once
#include "IWidget.h"
#include <QLabel>
#include <QList>
#include <QTimer>

class PromptInfoBarWidget : public IWidget
{
    Q_OBJECT

public:
    //返回对象本身
    static PromptInfoBarWidget &getSelf();

    // 设置报警提示信息。
    void display(QString &info);

    // 构造与析构
    PromptInfoBarWidget(const QString &name);
    ~PromptInfoBarWidget();
protected:
    void paintEvent(QPaintEvent *e);

private slots:
    void onTimeout();

private:
    void _drawText(void);

    QString _text;
    QTimer *_timer;
    QList<QString> _stringList;

};
#define promptInfoBarWidget (PromptInfoBarWidget::getSelf())
