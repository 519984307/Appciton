/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/12
 **/

#include "PasswordWidget.h"
#include "Button.h"
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QEvent>
#include "LanguageManager.h"
#include "ThemeManager.h"
#include <QTimer>

#define PASSWORD_LEN (8)                      // 密码长度
#define DEFAULT_SUPER_PASSWORD  "672401"

class PasswordWidgetPrivate
{
public:
    enum KeyItem
    {
        KEY_ITEM_NUM_0 = 0,
        KEY_ITEM_NUM_1,
        KEY_ITEM_NUM_2,
        KEY_ITEM_NUM_3,
        KEY_ITEM_NUM_4,
        KEY_ITEM_NUM_5,
        KEY_ITEM_NUM_6,
        KEY_ITEM_NUM_7,
        KEY_ITEM_NUM_8,
        KEY_ITEM_NUM_9,
        KEY_ITEM_ACTION_DELETE,
        KEY_ITEM_ACTION_SURE
    };

    PasswordWidgetPrivate(const QString &name, const QString &password)
        : passwordEdit(NULL), name(name), widgetPassword(password),
          superPassword(DEFAULT_SUPER_PASSWORD), timer(NULL)
    {
    }

    QMap<KeyItem, Button *> buttons;
    QLineEdit *passwordEdit;
    QString name;
    QString passwordStr;                    // 输入密码
    QString widgetPassword;                 // 窗口密码
    QString superPassword;                  // 超级密码
    QTimer *timer;                           // 定时器,用于清除警告区信息
};

PasswordWidget::PasswordWidget(const QString &name, const QString &password)
    : d_ptr(new PasswordWidgetPrivate(name, password))
{
    // 2s后清除警告信息
    d_ptr->timer = new QTimer(this);
    d_ptr->timer->setSingleShot(true);
    connect(d_ptr->timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()));

    layoutExec();
}

PasswordWidget::~PasswordWidget()
{
    delete d_ptr;
}

void PasswordWidget::layoutExec()
{
    if (layout())
    {
        return;
    }


    QVBoxLayout *vlayout = new QVBoxLayout(this);

    vlayout->addStretch(1);

    QGridLayout *layout = new QGridLayout;

    QLabel *label;
    Button *button;
    int itemID;

    label = new QLabel(trs(d_ptr->name));
    layout->addWidget(label, 0, 0, 1, 3, Qt::AlignCenter);

    d_ptr->passwordEdit = new QLineEdit();
    d_ptr->passwordEdit->setReadOnly(true);
    d_ptr->passwordEdit->setFocusPolicy(Qt::NoFocus);
    d_ptr->passwordEdit->setPlaceholderText(trs("Password"));
    QColor borderColor = themeManager.getColor(ThemeManager::ControlTypeNR, ThemeManager::ElementBorder,
                         ThemeManager::StateInactive);
    d_ptr->passwordEdit->setStyleSheet(QString("border:%1px solid %2;border-radius:%3px;padding:2px 4px;Height:30px")
                                       .arg(themeManager.getBorderWidth())
                                       .arg(borderColor.name())
                                       .arg(themeManager.getBorderRadius()));
    layout->addWidget(d_ptr->passwordEdit, 1, 0, 1, 3);

    int row = 2;
    int col = 0;
    for (int i = 1; i <= 9; i ++)
    {
        button = new Button(QString::number(i));
        button->setButtonStyle(Button::ButtonTextOnly);
        itemID = i;
        button->setProperty("Item", qVariantFromValue(itemID));
        connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));

        layout->addWidget(button, row, col, 1, 1);
        d_ptr->buttons.insert((PasswordWidgetPrivate::KeyItem)i, button);
        col++;
        if (i % 3 == 0)
        {
            row++;
            col = 0;
        }
    }

    button = new Button(trs("SupervisorDel"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(PasswordWidgetPrivate::KEY_ITEM_ACTION_DELETE);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, row, col, 1, 1);
    col++;

    button = new Button(QString::number(0));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(PasswordWidgetPrivate::KEY_ITEM_NUM_0);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, row, col, 1, 1);
    col++;

    button = new Button(trs("SupervisorOK"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(PasswordWidgetPrivate::KEY_ITEM_ACTION_SURE);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, row, col, 1, 1);

    vlayout->addLayout(layout);
    vlayout->addStretch(1);
}

void PasswordWidget::setPassword(const QString &password)
{
    d_ptr->widgetPassword = password;
}

void PasswordWidget::setSuperPassword(const QString &password)
{
    d_ptr->superPassword = password;
}

void PasswordWidget::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::FontChange)
    {
        QFontInfo fontinfo(font());
        QFont f(fontinfo.family(), fontinfo.pixelSize(), fontinfo.weight(), fontinfo.italic());
        d_ptr->passwordEdit->setFont(f);
    }
}

void PasswordWidget::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        if (d_ptr->timer->isActive())
        {
            d_ptr->timer->stop();
        }
        PasswordWidgetPrivate::KeyItem item
                = (PasswordWidgetPrivate::KeyItem)button->property("Item").toInt();
        switch (item)
        {
        case PasswordWidgetPrivate::KEY_ITEM_NUM_0:
        case PasswordWidgetPrivate::KEY_ITEM_NUM_1:
        case PasswordWidgetPrivate::KEY_ITEM_NUM_2:
        case PasswordWidgetPrivate::KEY_ITEM_NUM_3:
        case PasswordWidgetPrivate::KEY_ITEM_NUM_4:
        case PasswordWidgetPrivate::KEY_ITEM_NUM_5:
        case PasswordWidgetPrivate::KEY_ITEM_NUM_6:
        case PasswordWidgetPrivate::KEY_ITEM_NUM_7:
        case PasswordWidgetPrivate::KEY_ITEM_NUM_8:
        case PasswordWidgetPrivate::KEY_ITEM_NUM_9:
        {
            if (d_ptr->passwordStr.size() >= PASSWORD_LEN)
            {
                return;
            }
            d_ptr->passwordStr += QString::number(static_cast<int>(item));
            QString text(d_ptr->passwordStr.size(), '*');
            d_ptr->passwordEdit->setText(text);
            break;
        }
        case PasswordWidgetPrivate::KEY_ITEM_ACTION_DELETE:
        {
            if (d_ptr->passwordStr.size() == 0)
            {
                return;
            }

            d_ptr->passwordStr.remove(d_ptr->passwordStr.size() - 1, 1);
            QString text(d_ptr->passwordStr.size(), '*');
            d_ptr->passwordEdit->setText(text);
            break;
        }
        case PasswordWidgetPrivate::KEY_ITEM_ACTION_SURE:
        {
            if (d_ptr->passwordStr == d_ptr->widgetPassword || d_ptr->passwordStr == d_ptr->superPassword)
            {
                emit correctPassword();
                d_ptr->passwordStr.clear();
                d_ptr->passwordEdit->setText(d_ptr->passwordStr);
            }
            else
            {
                d_ptr->passwordEdit->setText(trs("ErrorPassWordERROR"));
                d_ptr->passwordStr.clear();
                d_ptr->timer->start(2 * 1000);
            }
            break;
        }
        default:
            break;
        }
    }
}

void PasswordWidget::timeOutSlot()
{
    d_ptr->passwordEdit->setText("");
}
