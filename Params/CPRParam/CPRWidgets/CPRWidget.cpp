#include "CPRWidget.h"
#include "CPRBarWidget.h"
#include "ParamInfo.h"
#include "FontManager.h"
#include "ColorManager.h"
#include "MenuManager.h"
#include "CPRMenu.h"
#include <QLabel>
#include <QBoxLayout>
#include <QPainter>


/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void CPRWidget::_releaseHandle(IWidget *)
{
    menuManager.popup(&cprMenu);
}

/**************************************************************************************************
 * 重绘。
 *************************************************************************************************/
void CPRWidget::paintEvent(QPaintEvent *e)
{
    // 绘制标题栏。
    QRect r = rect();

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(palette().color(QPalette::Window));
    painter.drawRoundedRect(rect(), 4, 4);

    // 绘制字体。
    r.adjust(5, 0, 0, 0);
    painter.setPen(palette().color(QPalette::WindowText));
    painter.setFont(fontManager.textFont(16, true));
    painter.drawText(r, Qt::AlignLeft | Qt::AlignVCenter, "CPR");

    IWidget::paintEvent(e);
}

/**************************************************************************************************
 * 测试用，产生Bar数据。
 *************************************************************************************************/
void CPRWidget::timerEvent(QTimerEvent */*event*/)
{
    short buff[5];
    int len = sizeof(buff) / sizeof(buff[0]);
    for (int i = 0; i < len; i++)
    {
        buff[i] = rand() % 4000;
    }

    _barWidget->updateValue(buff, len);
}

/**************************************************************************************************
 * 更新数据。
 *************************************************************************************************/
void CPRWidget::updataWaveform(short waveform[], int len)
{
    _barWidget->updateValue(waveform, len);
}

/**************************************************************************************************
 * 更新CPR按压节律。
 *************************************************************************************************/
void CPRWidget::updateCompressionRate(short rate)
{
    if (rate == InvData())
    {
        _rateValue->setText(InvStr());
        return;
    }

    _rateValue->setText(QString::number(rate));
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
CPRWidget::CPRWidget() : IWidget("CPRWidget")
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_CPR));
    palette.setColor(QPalette::Window, QColor(60, 60, 60));
    setPalette(palette);

    _barWidget = new CPRBarWidget();
    _barWidget->setRange(0, 4000);
    _barWidget->setSampleRate(125);
    _barWidget->setPalette(palette);

    // rate。
    _rateLabel = new QLabel("Rate (cpm)");
    _rateLabel->setFont(fontManager.textFont(11));
    _rateLabel->setPalette(palette);
    _rateLabel->setAlignment(Qt::AlignCenter);

    _rateValue = new QLabel();
    _rateValue->setAlignment(Qt::AlignCenter);
    _rateValue->setFixedWidth(60);
    _rateValue->setFont(fontManager.numFont(40, true));
    _rateValue->setPalette(palette);
    _rateValue->setText(InvStr());

    QVBoxLayout *rateLayout = new QVBoxLayout();
    rateLayout->setContentsMargins(2, 10, 4, 10);
    rateLayout->setSpacing(2);
    rateLayout->addWidget(_rateLabel);
    rateLayout->addWidget(_rateValue);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(2, 2, 2, 2);
    mainLayout->setSpacing(2);
    mainLayout->addWidget(_barWidget, 1);
    mainLayout->addLayout(rateLayout);

    setLayout(mainLayout);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));
startTimer(40);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
CPRWidget::~CPRWidget()
{

}
