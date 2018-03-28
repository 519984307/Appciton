#include "PatientInfoWidget.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include <QBoxLayout>
#include "PatientManager.h"
#include <QLabel>

void PatientInfoWidget::loadPatientInfo()
{
    QString str = patientManager.getName();
    if (str.length() > 12)
    {
        str = str.left(10);
        str += "...";
    }
    _patientName->setText(str);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PatientInfoWidget::PatientInfoWidget(QWidget *parent) : IWidget("PatientInfoWidget", parent)
{
    int fontSize = fontManager.getFontSize(3);

    _bedtype = new QLabel(this);
    _bedtype->setFont(fontManager.textFont(fontSize));
    _bedtype->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _bedtype->setText(trs("BedNum"));

    _bedNum = new QLabel(this);
    _bedNum->setFont(fontManager.textFont(fontSize));
    _bedNum->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _bedNum->setText("024");

    _patientName = new QLabel(this);
    _patientName->setFont(fontManager.textFont(fontSize));
    _patientName->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->addStretch();
    hLayout->addWidget(_bedtype);
    hLayout->addWidget(_bedNum);
    hLayout->addStretch();
    hLayout->addWidget(_patientName);
    hLayout->addStretch();
    setLayout(hLayout);

    setFocusPolicy(Qt::NoFocus);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));

    loadPatientInfo();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PatientInfoWidget::~PatientInfoWidget()
{
}
