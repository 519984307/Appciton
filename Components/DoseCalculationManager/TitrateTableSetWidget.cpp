#include "TitrateTableSetWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "TitrateTableManager.h"
#include "TitrateTableWidget.h"
#include "IComboList.h"
#include "IButton.h"

TitrateTableSetWidget *TitrateTableSetWidget::_selfObj = NULL;

/**************************************************************************************************
 * 名称： 析构函数
 * 功能：
 *************************************************************************************************/
TitrateTableSetWidget::~TitrateTableSetWidget()
{

}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void TitrateTableSetWidget::layoutExec()
{
    setFixedWidth(windowManager.getPopMenuWidth()/2);
    setFixedHeight(windowManager.getPopMenuHeight()/2);

    int  submenuW = windowManager.getPopMenuWidth();

    setTitleBarText(trs("TitrateTableSet"));

    int itemW = submenuW - 20;

    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 4;
    int labelWidth = btnWidth;

    _step = new IComboList(trs("Step"));
    _step->setFont(fontManager.textFont(fontSize));
    for (int i = 0; i < STEP_OPTION_NR; i ++)
    {
        _step->addItem(QString::number(TitrateTableDefine::convert(static_cast<StepOption>(i))));
    }
    _step->label->setFixedSize(labelWidth/2, _itemH);
    _step->combolist->setFixedSize(btnWidth, _itemH);
    _step->label->setAlignment(Qt::AlignLeft);
    _step->combolist->setCurrentIndex(titrateTableManager.getSetTableParam().step);

    _datumTerm = new IComboList(trs("DatumTerm"));
    _datumTerm->setFont(fontManager.textFont(fontSize));
    for (int j = 0; j < DATUM_TERM_NR; j ++)
    {
        _datumTerm->addItem(trs(TitrateTableDefine::convert(static_cast<DatumTerm>(j))));
    }
    _datumTerm->label->setFixedSize(labelWidth/2, _itemH);
    _datumTerm->combolist->setFixedSize(btnWidth, _itemH);
    _datumTerm->label->setAlignment(Qt::AlignLeft);
    _datumTerm->combolist->setCurrentIndex(titrateTableManager.getSetTableParam().datumTerm);

    _doseType = new IComboList(trs("DoseType"));
    _doseType->setFont(fontManager.textFont(fontSize));
    for (int m = 0; m < DOSE_TYPE_NR; m ++)
    {
        _doseType->addItem(trs(TitrateTableDefine::convert(static_cast<DoseType>(m))));
    }
    _doseType->label->setFixedSize(labelWidth/2, _itemH);
    _doseType->combolist->setFixedSize(btnWidth, _itemH);
    _doseType->label->setAlignment(Qt::AlignLeft);
    _doseType->combolist->setCurrentIndex(titrateTableManager.getSetTableParam().doseType);

    _yes = new IButton(trs("EnglishYESChineseSURE"));
    _yes->setFixedSize(btnWidth, _itemH);
    _yes->setFont(fontManager.textFont(fontSize));
    _yes->setBorderEnabled(true);
    connect(_yes, SIGNAL(realReleased()), this, SLOT(_yesReleased()));

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget(_yes);
    hlayout->addStretch();

    contentLayout->addWidget(_step);
    contentLayout->addWidget(_datumTerm);
    contentLayout->addWidget(_doseType);
    contentLayout->addStretch();
    contentLayout->addLayout(hlayout);
    contentLayout->addStretch();

    contentLayout->setSpacing(3);

}

/**************************************************************************************************
 * 名称： 显示事件
 * 功能：
 *************************************************************************************************/
void TitrateTableSetWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
//    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);

}

void TitrateTableSetWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusPreviousChild();
            break;
        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            break;
        default:
            break;
    }

    QWidget::keyPressEvent(e);
}

void TitrateTableSetWidget::_yesReleased()
{
    titrateTableManager.setStep(static_cast<StepOption>(_step->combolist->currentIndex()));
    titrateTableManager.setDatumTerm(static_cast<DatumTerm>(_datumTerm->combolist->currentIndex()));
    titrateTableManager.setDoseType(static_cast<DoseType>(_doseType->combolist->currentIndex()));

    titrateTableWidget.updateTitrateTableData();
    hide();
}

/**************************************************************************************************
 * 名称： 构造函数
 * 功能：
 *************************************************************************************************/
TitrateTableSetWidget::TitrateTableSetWidget()
{
    layoutExec();
}
