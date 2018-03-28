#include "RelievePatientWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "IButton.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "PatientInfoMenu.h"
#include "LanguageManager.h"
#include "DataStorageDirManager.h"

RelievePatientWidget *RelievePatientWidget::_selfObj = NULL;

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
RelievePatientWidget::~RelievePatientWidget()
{

}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void RelievePatientWidget::layoutExec()
{
    //setFixedWidth(windowManager.getPopMenuWidth()*2/3);

    int  submenuW = windowManager.getPopMenuWidth();

    setTitleBarText(trs("RelievePatient"));

    int itemW = submenuW - 20;

    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 4;
//    int labelWidth = btnWidth;

    _standby = new QLabel();
    _standby->setText(trs("Standby"));
    _standby->setFont(fontManager.textFont(fontSize));
    _standby->setFixedSize(btnWidth, _itemH);

    _yes = new IButton(trs("Yes"));
    _yes->setFixedSize(btnWidth, _itemH);
    _yes->setFont(fontManager.textFont(fontSize));
    _yes->setBorderEnabled(true);

    _no = new IButton(trs("No"));
    _no->setFixedSize(btnWidth, _itemH);
    _no->setFont(fontManager.textFont(fontSize));
    _no->setBorderEnabled(true);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(_yes);
    hlayout->addWidget(_no);

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setMargin(5);
    vlayout->setSpacing(5);
    vlayout->addWidget(_standby);
    vlayout->addLayout(hlayout);
    vlayout->setAlignment(Qt::AlignCenter);
    contentLayout->addLayout(vlayout);

    connect(_yes, SIGNAL(realReleased()), this, SLOT(_yesReleased()));
    connect(_no, SIGNAL(realReleased()), this, SLOT(_noReleased()));
}

/***************************************************************************************************
 * 显示事件
 **************************************************************************************************/
void RelievePatientWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
//    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
}

/**************************************************************************************************
 * 重写X退出槽函数。
 *************************************************************************************************/
void RelievePatientWidget::exit()
{
    this->hide();
    patientInfoMenu.exec();
}

/***************************************************************************************************
 * 进入待机模式槽函数
 **************************************************************************************************/
void RelievePatientWidget::_yesReleased()
{
    this->hide();
    dataStorageDirManager.createDir(true);
    patientInfoMenu.relieveStatus(false);
}

/***************************************************************************************************
 * 不进入待机模式槽函数
 **************************************************************************************************/
void RelievePatientWidget::_noReleased()
{
    this->hide();
    dataStorageDirManager.createDir(true);
    patientInfoMenu.relieveStatus(false);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
RelievePatientWidget::RelievePatientWidget() : PopupWidget()
{
    layoutExec();
}

