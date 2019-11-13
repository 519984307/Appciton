/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/1
 **/
#include "TitrateTableSetWindow.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/UI/Button.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include "TitrateTableManager.h"
#include "TitrateTableWindow.h"
#include "WindowManager.h"
#include <QGridLayout>
#include "TitrateTableDefine.h"
#include "LayoutManager.h"

class TitrateTableSetWindowPrivate
{
public:
    TitrateTableSetWindowPrivate();

    ComboBox *stepCombo;
    ComboBox *datumTermCombo;
    ComboBox *doseTypeCombo;
    Button *yesBtn;
};


TitrateTableSetWindowPrivate::TitrateTableSetWindowPrivate()
    : stepCombo(NULL),
      datumTermCombo(NULL),
      doseTypeCombo(NULL),
      yesBtn(NULL)
{
}


TitrateTableSetWindow::TitrateTableSetWindow()
    : Dialog(),
      d_ptr(new TitrateTableSetWindowPrivate)
{
    layoutExec();
}

TitrateTableSetWindow *TitrateTableSetWindow::getInstance()
{
    static TitrateTableSetWindow *instance = NULL;
    if (!instance)
    {
        instance = new TitrateTableSetWindow;
    }
    return instance;
}

TitrateTableSetWindow::~TitrateTableSetWindow()
{
    delete d_ptr;
}

void TitrateTableSetWindow::onBtnYesReleased()
{
    titrateTableManager.setStep(static_cast<StepOption>(
                                    d_ptr->stepCombo->currentIndex()));
    titrateTableManager.setDatumTerm(static_cast<DatumTerm>(
                                         d_ptr->datumTermCombo->currentIndex()));
    titrateTableManager.setDoseType(static_cast<DoseType>(
                                        d_ptr->doseTypeCombo->currentIndex()));

    TitrateTableWindow::getInstance()->updateTitrateTableData();
    hide();
}

void TitrateTableSetWindow::showEvent(QShowEvent *e)
{
    Dialog::showEvent(e);

    QRect r = layoutManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
}


void TitrateTableSetWindow::layoutExec()
{
    setWindowTitle(trs("TitrateTableSet"));

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    ComboBox *combo;

    label = new QLabel(trs("Step"));
    layout->addWidget(label, 0, 0);
    combo = new ComboBox;
    for (int i = 0; i < STEP_OPTION_NR; i++)
    {
        combo->addItem(QString::number(i+1));
    }
    layout->addWidget(combo, 0, 1);
    combo->setCurrentIndex(titrateTableManager.
                           getSetTableParam().step);
    d_ptr->stepCombo = combo;

    label = new QLabel(trs("DatumTerm"));
    layout->addWidget(label, 1, 0);
    const char *symbolDatuTerm[DATUM_TERM_NR] =
    {
        "Dosemin",
        "InfusionRate",
        "DripRate"
    };
    combo = new ComboBox;
    for (int i = 0; i < DATUM_TERM_NR; i++)
    {
        combo->addItem(trs(symbolDatuTerm[i]));
    }
    layout->addWidget(combo, 1, 1);
    combo->setCurrentIndex(titrateTableManager.
                           getSetTableParam().datumTerm);
    d_ptr->datumTermCombo = combo;

    label = new QLabel(trs("Dosetype"));
    layout->addWidget(label, 2, 0);
    const char *symbolDose[DOSE_TYPE_NR] =
    {
        "minDose",
        "hDose",
        "kgminDose",
        "kghDose"
    };
    combo = new ComboBox;
    for (int i = 0; i < DOSE_TYPE_NR; i++)
    {
        combo->addItem(trs(symbolDose[i]));
    }
    layout->addWidget(combo, 2, 1);
    combo->setCurrentIndex(titrateTableManager.
                           getSetTableParam().doseType);
    d_ptr->doseTypeCombo = combo;

    Button *button = new Button(trs("EnglishYESChineseSURE"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnYesReleased()));
    layout->addWidget(button, 3, 1);
    d_ptr->yesBtn = button;

    layout->setRowStretch(4, 1);
    setWindowLayout(layout);
}
















