/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/11
 **/
#include "ScreenLayoutEditor.h"
#include <QGridLayout>
#include <ComboBox.h>
#include <Button.h>
#include <QLabel>
#include "LanguageManager.h"

class ScreenLayoutEditorPrivate
{
public:
    ScreenLayoutEditorPrivate()
        : replaceCbo(NULL),
          insertCbo(NULL),
          removeBtn(NULL)
    {}
    ComboBox *replaceCbo;
    ComboBox *insertCbo;
    Button *removeBtn;
    QPoint displayPos;
};

ScreenLayoutEditor::ScreenLayoutEditor(const QString &title)
    : Window(), d_ptr(new ScreenLayoutEditorPrivate())
{
    QGridLayout *gridLayout = new QGridLayout();

    QLabel *label = new QLabel(trs("Replace"));
    gridLayout->addWidget(label, 0, 0);
    d_ptr->replaceCbo = new ComboBox;
    gridLayout->addWidget(d_ptr->replaceCbo, 0, 1);

    label = new QLabel(trs("Insert"));
    gridLayout->addWidget(label, 1, 0);
    d_ptr->insertCbo = new ComboBox();
    gridLayout->addWidget(d_ptr->insertCbo, 1, 1);

    d_ptr->removeBtn = new Button(trs("Remove"));
    d_ptr->removeBtn->setButtonStyle(Button::ButtonTextOnly);
    gridLayout->addWidget(d_ptr->removeBtn, 2, 1);

    gridLayout->setColumnStretch(0, 2);
    gridLayout->setColumnStretch(1, 3);

    setWindowLayout(gridLayout);
    setWindowTitle(title);

    this->setFixedWidth(250);

    // initialize to disable
    d_ptr->replaceCbo->setEnabled(false);
    d_ptr->insertCbo->setEnabled(false);

    connect(d_ptr->replaceCbo, SIGNAL(currentIndexChanged(QString)), this, SLOT(onComboIndexChanged(QString)));
    connect(d_ptr->insertCbo, SIGNAL(currentIndexChanged(QString)), this, SLOT(onComboIndexChanged(QString)));
    connect(d_ptr->removeBtn, SIGNAL(clicked(bool)), this, SLOT(onRemoveBtnClicked()));
}

ScreenLayoutEditor::~ScreenLayoutEditor()
{
    delete d_ptr;
}

void ScreenLayoutEditor::setReplaceList(const QStringList &list)
{
    d_ptr->replaceCbo->blockSignals(true);
    d_ptr->replaceCbo->clear();
    d_ptr->replaceCbo->addItems(list);
    d_ptr->replaceCbo->blockSignals(false);
    // if the combox has not item, set to disable
    if (list.isEmpty())
    {
        d_ptr->replaceCbo->setEnabled(false);
    }
    else
    {
        d_ptr->replaceCbo->setEnabled(true);
    }
}

void ScreenLayoutEditor::setInsertList(const QStringList &list)
{
    d_ptr->insertCbo->blockSignals(true);
    d_ptr->insertCbo->clear();
    d_ptr->insertCbo->addItems(list);
    d_ptr->insertCbo->blockSignals(false);
    // if the combox has not item, set to disable
    if (list.isEmpty())
    {
        d_ptr->insertCbo->setEnabled(false);
    }
    else
    {
        d_ptr->insertCbo->setEnabled(true);
    }
}

void ScreenLayoutEditor::setDisplayPosition(const QPoint &p)
{
    d_ptr->displayPos = p;
}

void ScreenLayoutEditor::setRemoveable(bool flag)
{
    d_ptr->removeBtn->setEnabled(flag);
}

void ScreenLayoutEditor::onComboIndexChanged(const QString &text)
{
    ComboBox *cbo = qobject_cast<ComboBox *>(sender());
    if (d_ptr->replaceCbo == cbo)
    {
        emit commitChanged(ReplaceRole, text);
    }
    else if (d_ptr->insertCbo == cbo && !text.isEmpty()) // do nothing if the text is empty
    {
        emit commitChanged(InsertRole, text);

        // close after changed
        close();
    }
}

void ScreenLayoutEditor::onRemoveBtnClicked()
{
    emit commitChanged(RemoveRole, QString());
    // close after changed
    close();
}

void ScreenLayoutEditor::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);
    if (!d_ptr->displayPos.isNull())
    {
        this->move(d_ptr->displayPos);
    }
}
