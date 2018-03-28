#include "PatientBarWidget.h"
#include "PatientTypeSelectWidget.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include <QBoxLayout>

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void PatientBarWidget::_releaseHandle(IWidget *)
{
    QPoint p = this->mapToGlobal(this->rect().bottomLeft());
    QRect r = geometry();
    _patientTypeSelWidget->autoShow(p.x(), p.y() + 1, r.width() + 10, 3 * r.height() + 18);
}

/**************************************************************************************************
 * 功能：病人类型改变。
 * 参数：
 *      newType: 病人类型。
 *************************************************************************************************/
void PatientBarWidget::setPatientType(PatientType newType)
{
    QString str(PatientSymbol::convert(newType));
    _patientTypeLabel->setText(trs(str));
}

/**************************************************************************************************
 * 功能：设置病人信息。
 * 参数：
 *      info: 病人信息。
 *************************************************************************************************/
void PatientBarWidget::setPatientInfo(const PatientInfo &info)
{
    QString str(PatientSymbol::convert(info.type));
    _patientTypeLabel->setText(trs(str));
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PatientBarWidget::PatientBarWidget(QWidget *parent) : IWidget("PatientBarWidget", parent)
{
    _patientTypeSelWidget = new PatientSelectWidget();

    _patientTypeLabel = new QLabel("", this);
    _patientTypeLabel->setFont(fontManager.textFont(fontManager.getFontSize(3)));
    _patientTypeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
//    _patientTypeLabel->setFixedWidth(140);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->addWidget(_patientTypeLabel);
    setLayout(hLayout);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PatientBarWidget::~PatientBarWidget()
{
    delete _patientTypeSelWidget;
}
