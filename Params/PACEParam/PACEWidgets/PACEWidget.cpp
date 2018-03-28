#include "PACEWidget.h"
#include "ParamInfo.h"
#include "PACEMenu.h"
#include "FontManager.h"
#include "ColorManager.h"
#include "MenuManager.h"
#include <QLabel>
#include <QBoxLayout>
#include <QPainter>


/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void PACEWidget::_releaseHandle(IWidget *)
{
    menuManager.popup(&paceMenu);
}

/**************************************************************************************************
 * 定时器事件，用于闪烁脉冲释放。
 *************************************************************************************************/
void PACEWidget::timerEvent(QTimerEvent *)
{
    if (_pulseDeliver == 0)
    {
        return;
    }

    _pulseDeliver--;
    update(_flushArea);
}

/**************************************************************************************************
 * 重绘。
 *************************************************************************************************/
void PACEWidget::paintEvent(QPaintEvent *e)
{
    // 绘制背景。
    QRect r = rect();
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(palette().color(QPalette::Shadow));
    painter.drawRoundedRect(r, 4, 4);

    // 绘制标题栏。
    r.setBottom(_barHeight);
    painter.setBrush(palette().color(QPalette::Window));
    painter.drawRoundedRect(r, 4, 4);

    // 绘制字体。
    r.adjust(10, 0, 0, 0);
    painter.setPen(Qt::black);
    painter.setFont(fontManager.textFont(14, true));
    painter.drawText(r, Qt::AlignLeft | Qt::AlignVCenter, "PACING");

    // 绘制pulse Delivered.
    // 边框
    r = rect();
    r.setX(r.width() - _barHeight);
    r.setY(3);
    r.setHeight(_barHeight - 6);
    r.setWidth(_barHeight - 6);
    painter.setPen(Qt::white);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(r);
    _flushArea = r;

    // 内部实体。
    r.setX(r.x() + 2);
    r.setY(5);
    r.setHeight(_barHeight - 10);
    r.setWidth(_barHeight - 10);
    painter.setPen(Qt::NoPen);
    if (_pulseDeliver % 2)
    {
        painter.setBrush(QColor(255, 255, 255));

    }
    else
    {
        painter.setBrush(QColor(163, 205, 57));
    }
    painter.drawEllipse(r);

    IWidget::paintEvent(e);
}

/**************************************************************************************************
 * 闪烁。
 *************************************************************************************************/
void PACEWidget::pulseDelivered(void)
{
    if (_pulseDeliver == 0)
    {
        _pulseDeliver = 6;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PACEWidget::PACEWidget(): IWidget("PaceWidget")
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_PACE));
    palette.setColor(QPalette::Window, QColor(2, 167, 158));
    palette.setColor(QPalette::Shadow, QColor(60, 60, 60));
    setPalette(palette);

    // rate。
    _rateLabel = new QLabel("Rate");
    _rateLabel->setFont(fontManager.textFont(15));
    _rateLabel->setPalette(palette);
    _rateLabel->setAlignment(Qt::AlignCenter);

    _rateValue = new QLabel();
    _rateValue->setAlignment(Qt::AlignCenter);
    _rateValue->setFixedWidth(60);
    _rateValue->setFont(fontManager.numFont(40, true));
    _rateValue->setPalette(palette);
    _rateValue->setText(InvStr());

    _rateUnit = new QLabel("PPM");
    _rateUnit->setFont(fontManager.textFont(11));
    _rateUnit->setPalette(palette);
    _rateUnit->setAlignment(Qt::AlignCenter);

    // Current Output.
    _currentLabel = new QLabel("Output");
    _currentLabel->setFont(fontManager.textFont(15));
    _currentLabel->setPalette(palette);
    _currentLabel->setAlignment(Qt::AlignCenter);

    _currentValue = new QLabel();
    _currentValue->setAlignment(Qt::AlignCenter);
    _currentValue->setFixedWidth(60);
    _currentValue->setFont(fontManager.numFont(40, true));
    _currentValue->setPalette(palette);
    _currentValue->setText(InvStr());

    _currentUnit = new QLabel("mA");
    _currentUnit->setFont(fontManager.textFont(11));
    _currentUnit->setPalette(palette);
    _currentUnit->setAlignment(Qt::AlignCenter);

    // Mode
    _modeLabel = new QLabel("Mode");
    _modeLabel->setFont(fontManager.textFont(15));
    _modeLabel->setPalette(palette);
    _modeLabel->setAlignment(Qt::AlignCenter);

    _modeValue = new QLabel("Demand");
    _modeValue->setFont(fontManager.textFont(18));
    _modeValue->setPalette(palette);
    _modeValue->setAlignment(Qt::AlignCenter);

    // 布局。
    // Rate
    QVBoxLayout *rateLayout = new QVBoxLayout();
    rateLayout->setMargin(0);
    rateLayout->setSpacing(1);
    rateLayout->addWidget(_rateLabel);
    rateLayout->addWidget(_rateValue);

    QVBoxLayout *rateUnitLayout = new QVBoxLayout();
    rateUnitLayout->setMargin(0);
    rateUnitLayout->setSpacing(1);
    rateUnitLayout->addStretch(6);
    rateUnitLayout->addWidget(_rateUnit);
    rateUnitLayout->addStretch(1);

    // Output
    QVBoxLayout *outputLayout = new QVBoxLayout();
    outputLayout->setMargin(0);
    outputLayout->setSpacing(1);
    outputLayout->addWidget(_currentLabel);
    outputLayout->addWidget(_currentValue);

    QVBoxLayout *outputUnitLayout = new QVBoxLayout();
    outputUnitLayout->setMargin(0);
    outputUnitLayout->setSpacing(1);
    outputUnitLayout->addStretch(6);
    outputUnitLayout->addWidget(_currentUnit);
    outputUnitLayout->addStretch(1);

    // Mode
    QVBoxLayout *modeLayout = new QVBoxLayout();
    modeLayout->setMargin(0);
    modeLayout->setSpacing(1);
    modeLayout->addWidget(_modeLabel);
    modeLayout->addWidget(_modeValue);

    // 主布局。
    QHBoxLayout *main0Layout = new QHBoxLayout();
    main0Layout->setMargin(0);
    main0Layout->setSpacing(1);
    main0Layout->addLayout(rateLayout);
    main0Layout->addLayout(rateUnitLayout);
    main0Layout->addStretch();
    main0Layout->addLayout(outputLayout);
    main0Layout->addLayout(outputUnitLayout);
    main0Layout->addStretch();
    main0Layout->addLayout(modeLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(10, _barHeight, 10, 0);
    mainLayout->setSpacing(1);
    mainLayout->addLayout(main0Layout);

    setLayout(mainLayout);
    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));

    startTimer(100);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PACEWidget::~PACEWidget()
{

}
