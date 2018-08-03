#include "RESPTrendWidget.h"
#include "ParamManager.h"
#include "ParamInfo.h"
#include <QHBoxLayout>
#include "PublicMenuManager.h"
#include "RESPParam.h"
#include "TrendWidgetLabel.h"
#include "MainMenuWindow.h"
#include "RESPMenuContent.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void RESPTrendWidget::_releaseHandle(IWidget *)
{
    MainMenuWindow *mainMenu = MainMenuWindow::getInstance();
    RESPMenuContent *RespContent = RESPMenuContent::getInstance();
    mainMenu->popup(RespContent);
}

/**************************************************************************************************
 * 设置PR的值。
 *************************************************************************************************/
void RESPTrendWidget::setRRValue(short rr, bool isRR)
{
    if (isRR)
    {
        setName(trs(paramInfo.getSubParamName(SUB_DUP_PARAM_RR)));
    }
    else
    {
        setName(trs(paramInfo.getSubParamName(SUB_DUP_PARAM_BR)));
    }

    if (rr != InvData())
    {
        _rrString = QString::number(rr);
    }
    else
    {
        _rrString = InvStr();
    }
}


/**************************************************************************************************
 * 窗口是否使能。
 *************************************************************************************************/
bool RESPTrendWidget::enable()
{
    return respParam.isEnabled();
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void RESPTrendWidget::isAlarm(bool flag)
{
    _isAlarm = flag;

    updateAlarm(flag);
}

/**************************************************************************************************
 * 显示报警。
 *************************************************************************************************/
void RESPTrendWidget::showValue(void)
{
    QPalette p;
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_RESP));
    QPalette fgColor = normalPalette(psrc);
    QPalette alaColor = alarmPalette(psrc);
    if (_isAlarm)
    {
//        if (p.window().color() != Qt::white)
//        {
//            p.setColor(QPalette::Window, Qt::white);
//            p.setColor(QPalette::WindowText, Qt::red);
//            setPalette(p);
//        }

        p = _rrValue->palette();
        if (p.windowText().color() != alaColor.windowText().color())
        {
            _rrValue->setPalette(alaColor);
        }
        else
        {
            _rrValue->setPalette(fgColor);
        }
    }
    else
    {
        p = _rrValue->palette();
        if (p.windowText().color() != fgColor.windowText().color())
        {
            _rrValue->setPalette(fgColor);
        }
    }

    _rrValue->setText(_rrString);
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void RESPTrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    // 字体。
//    int fontsize = fontManager.adjustNumFontSizeXML(r);
//    fontsize = fontManager.getFontSize(fontsize);
    int fontsize = fontManager.adjustNumFontSize(r,true);
    QFont font = fontManager.numFont(fontsize, true);
//    font.setStretch(105); // 横向放大。
    font.setWeight(QFont::Black);

    _rrValue->setFont(font);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
RESPTrendWidget::RESPTrendWidget() : TrendWidget("RESPTrendWidget")
{
    _isAlarm = false;
    _rrString = InvStr();
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_RESP));
    setPalette(palette);
    setName(trs(paramInfo.getSubParamName(SUB_PARAM_RR_BR)));
    setUnit(Unit::getSymbol(UNIT_RPM));

    // RR值。
    _rrValue = new QLabel();
    _rrValue->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _rrValue->setPalette(palette);
    _rrValue->setText(InvStr());

    // 布局。
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);
    mainLayout->addStretch(1);
    mainLayout->addWidget(_rrValue);
    mainLayout->addStretch(1);

    contentLayout->addStretch(1);
    contentLayout->addLayout(mainLayout);
    contentLayout->addStretch(1);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
RESPTrendWidget::~RESPTrendWidget()
{

}
