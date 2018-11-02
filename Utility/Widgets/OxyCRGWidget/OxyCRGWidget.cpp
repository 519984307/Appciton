/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/20
 **/

#include "OxyCRGWidget.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "WindowManager.h"
#include "ColorManager.h"
#include "OxyCRGWidgetLabel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMap>
#include "IWidget.h"
#include "IConfig.h"
#include <QPainter>
#include "PopupList.h"
#include "ParamInfo.h"
#include "SystemManager.h"
#include "OxyCRGSetupWindow.h"
#include "IMessageBox.h"
#include "ECGParam.h"
#include "OxyCRGTrendWaveWidget.h"
#include "OxyCRGCO2WaveWidget.h"
#include "OxyCRGSPO2TrendWidget.h"
#include "OxyCRGRESPWaveWidget.h"
#include "OxyCRGRRHRWaveWidget.h"
#include "LayoutManager.h"


#define TITLE_BAR_HEIGHT    24
#define LABEL_HEIGHT    40

class OxyCRGWidgetPrivate
{
public:
    OxyCRGWidgetPrivate()
        : setRulerAuto(NULL),
          rrHrTrend(NULL),
          spo2Trend(NULL),
          co2Wave(NULL),
          respWave(NULL),
          setupBtn(NULL),
          setupWin(NULL)
    {
    }

    ~OxyCRGWidgetPrivate()
    {
        delete setupWin;
    }

    OxyCRGWidgetLabel *setRulerAuto;
    OxyCRGRRHRWaveWidget *rrHrTrend;
    OxyCRGSPO2TrendWidget *spo2Trend;
    OxyCRGCO2WaveWidget *co2Wave;
    OxyCRGRESPWaveWidget *respWave;
    OxyCRGWidgetLabel *setupBtn;
    OxyCRGSetupWindow *setupWin;
};
/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGWidget::OxyCRGWidget(): IWidget("OxyCRGWidget"),
    _intervalList(NULL),
    _changeTrendList(NULL),
    _isShowGrid(true),
    _isShowFrame(true),
    _isShowScale(true),
    _intervalItemIndex(-1),
    d_ptr(new OxyCRGWidgetPrivate)
{
    _pixelWPitch = systemManager.getScreenPixelWPitch();
    _pixelHPitch = systemManager.getScreenPixelHPitch();

    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(152, 245, 255));
    palette.setColor(QPalette::Window, Qt::black);
//    palette.setColor(QPalette::Foreground, Qt::black);
    setPalette(palette);
    _mainLayout = new QVBoxLayout();
    _mainLayout->setMargin(0);
    _mainLayout->setSpacing(0);

    _hLayoutWave = new QVBoxLayout();
    _hLayoutWave->setMargin(1);
    _hLayoutWave->setSpacing(0);

    // 标题栏。
    _titleLabel = new QLabel(trs("UserFaceOxyCRG"));
    _titleLabel->setAlignment(Qt::AlignCenter);
    _titleLabel->setFixedHeight(TITLE_BAR_HEIGHT);
    int fontSize = 24;
    _titleLabel->setFont(fontManager.textFont(fontSize));
    _titleLabel->setWordWrap(true);
    QPalette p;
    p.setColor(QPalette::WindowText, Qt::black);
    p.setColor(QPalette::Window, QColor(255, 255, 0));
    _titleLabel->setPalette(p);

    bottomLayout = new QHBoxLayout();
    bottomLayout->setMargin(2);
    bottomLayout->setSpacing(1);
    int labelWidth = 80;

    fontSize = 18;
    _interval = new OxyCRGWidgetLabel("", Qt::AlignCenter, this);
    _interval->setFont(fontManager.textFont(fontSize));
    _interval->setFixedSize(labelWidth, LABEL_HEIGHT);
    _interval->setText("");
    connect(_interval, SIGNAL(released(IWidget *)), this, SLOT(_intervalSlot(IWidget *)));

    _changeTrend = new OxyCRGWidgetLabel("", Qt::AlignCenter, this);
    _changeTrend->setFont(fontManager.textFont(fontSize));
    _changeTrend->setFixedSize(labelWidth, LABEL_HEIGHT);
    _changeTrend->setText("");
    connect(_changeTrend, SIGNAL(released(IWidget *)), this, SLOT(_changeTrendSlot(IWidget *)));

    OxyCRGWidgetLabel *setRulerAuto = new OxyCRGWidgetLabel("", Qt::AlignCenter, this);
    setRulerAuto->setFont(fontManager.textFont(fontSize));
    setRulerAuto->setFixedSize(labelWidth, LABEL_HEIGHT);
    setRulerAuto->setText(trs("Auto"));
    connect(setRulerAuto, SIGNAL(released(IWidget *)), this, SLOT(_autoSetRuler()));
    d_ptr->setRulerAuto = setRulerAuto;

    d_ptr->setupBtn = new OxyCRGWidgetLabel("", Qt::AlignCenter, this);
    d_ptr->setupBtn->setFont(fontManager.textFont(fontSize));
    d_ptr->setupBtn->setFixedSize(labelWidth, LABEL_HEIGHT);
    d_ptr->setupBtn->setText(trs("SetUp"));
    connect(d_ptr->setupBtn, SIGNAL(released(IWidget *)), this, SLOT(_onSetupUpdated(IWidget *)));

    int rWidth = rect().width() / 4;
    int addWidth = (rWidth - labelWidth) / 2;
    bottomLayout->addSpacing(addWidth);
    bottomLayout->addWidget(_interval);
    bottomLayout->addSpacing(addWidth);
    bottomLayout->addWidget(_changeTrend);
    bottomLayout->addSpacing(addWidth);
    bottomLayout->addWidget(setRulerAuto);
    bottomLayout->addSpacing(addWidth);
    bottomLayout->addWidget(d_ptr->setupBtn);
    bottomLayout->addSpacing(addWidth);

    _mainLayout->addWidget(_titleLabel, 0, Qt::AlignCenter);
    _mainLayout->addLayout(_hLayoutWave);
    _mainLayout->addLayout(bottomLayout);

    // 设置布局。
    setLayout(_mainLayout);

    setFocusPolicy(Qt::NoFocus);

    d_ptr->setupWin =  new OxyCRGSetupWindow;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGWidget::~OxyCRGWidget()
{
    delete d_ptr;
}

/**************************************************************************************************
 * 获取配置列表
 * 布局
 *************************************************************************************************/
void OxyCRGWidget::_trendLayout(void)
{
    QStringList nodeWidgets;
    QString widgets;
    systemConfig.getStrValue("PrimaryCfg|UILayout|WidgetsOrder|OxyCRGWidget", widgets);
    nodeWidgets = widgets.split(",");

    for (int i = 0; i < nodeWidgets.size(); i++)
    {
        IWidget *w = layoutManager.getLayoutWidget(nodeWidgets[i]);
        if (w == NULL)
        {
            continue;
        }
        w->setParent(this);            // 设置父窗体。
        OxyCRGTrendWaveWidget *p = qobject_cast<OxyCRGTrendWaveWidget *>(w);
        p->setClearWaveDataStatus(false);
        w->setVisible(true);           // 可见。
        _hLayoutWave->addWidget(w, 1);
    }
}

/**************************************************************************************************
 * 清除显示控件
 *************************************************************************************************/
void OxyCRGWidget::_clearLayout()
{
    // 移除_hLayoutWave之前的窗体。
    int trendcount = _hLayoutWave->count();
    for (int i = 0; i < trendcount; i++)
    {
        QLayoutItem *item = _hLayoutWave->takeAt(0);
        IWidget *widget = qobject_cast<IWidget *>(item->widget());
        if (widget != NULL)
        {
            OxyCRGTrendWaveWidget *w = qobject_cast<OxyCRGTrendWaveWidget *>(widget);
            w->setClearWaveDataStatus(false);
            widget->setVisible(false);
            widget->setParent(NULL);
        }
    }
}

/**************************************************************************************************
 * 设置时间间隔。
 *************************************************************************************************/
void OxyCRGWidget::_setInterval(OxyCRGInterval index)
{
    currentConfig.setNumValue("OxyCRG|Interval", static_cast<int>(index));

    _interval->setText(OxyCRGSymbol::convert(index));

    if (d_ptr->rrHrTrend)
    {
        d_ptr->rrHrTrend->setInterval(index);
        d_ptr->rrHrTrend->update();
    }
    if (d_ptr->spo2Trend)
    {
        d_ptr->spo2Trend->setInterval(index);
        d_ptr->spo2Trend->update();
    }
    if (d_ptr->respWave)
    {
        d_ptr->respWave->setInterval(index);
        d_ptr->respWave->update();
    }
    if (d_ptr->co2Wave)
    {
        d_ptr->co2Wave->setInterval(index);
        d_ptr->co2Wave->update();
    }
}

/**************************************************************************************************
 * 获取时间间隔。
 *************************************************************************************************/
OxyCRGInterval OxyCRGWidget::_getInterval()
{
    int index = OxyCRG_Interval_1;
    currentConfig.getNumValue("OxyCRG|Interval", index);

    return (OxyCRGInterval)index;
}

/**************************************************************************************************
 * 设置RESP/CO2。
 *************************************************************************************************/
void OxyCRGWidget::_setTrend(OxyCRGTrend index)
{
    if (index >= OxyCRG_Trend_NR)
    {
        return;
    }
    currentConfig.setNumValue("OxyCRG|Wave", static_cast<int>(index));

    _changeTrend->setText(OxyCRGSymbol::convert(OxyCRGTrend(index)));

    QStringList nodeWidgets;
    QString widgets;
    QString path = "PrimaryCfg|UILayout|WidgetsOrder|OxyCRGWidget";
    systemConfig.getStrValue(path, widgets);
    nodeWidgets = widgets.split(",");

    int i = 0;
    if (index == OxyCRG_Trend_RESP)
    {

        for (; i < nodeWidgets.size(); i++)
        {
            if (nodeWidgets[i] == "OxyCRGCO2Widget")
            {
                nodeWidgets[i] = "OxyCRGRESPWidget";

                break;
            }
        }
    }
    else
    {
        for (; i < nodeWidgets.size(); i++)
        {
            if (nodeWidgets[i] == "OxyCRGRESPWidget")
            {
                nodeWidgets[i] = "OxyCRGCO2Widget";

                break;
            }
        }
    }
    widgets = nodeWidgets.join(",");
    systemConfig.setStrValue(path, widgets);
}

/**************************************************************************************************
 * 获取RESP/RR。
 *************************************************************************************************/
OxyCRGTrend OxyCRGWidget::_getTrend()
{
    int index = OxyCRG_Trend_RESP;
    currentConfig.getNumValue("OxyCRG|Wave", index);

    return (OxyCRGTrend)index;
}

/**************************************************************************************************
 * 对于给定的数值value, 计算大于等于value且能整除step的数值
 *************************************************************************************************/
int OxyCRGWidget::_roundUp(int value, int step)
{
    while (value % step)
    {
        value++;
    }
    return value;
}

/**************************************************************************************************
 * 对于给定的数值value, 计算小于等于value且能整除step的数值
 *************************************************************************************************/
int OxyCRGWidget::_roundDown(int value, int step)
{
    while (value % step)
    {
        value--;
    }
    return value;
}

/**************************************************************************************************
 * 布局器内部的Widget删除与显示。
 *************************************************************************************************/
void OxyCRGWidget::setVisible(bool visible)
{
    QWidget::setVisible(visible);

    if (!visible)
    {
        _clearLayout();
    }
    else
    {
        _setInterval(_getInterval());
        _setTrend(_getTrend());

        _trendLayout();
    }
}

/**************************************************************************************************
 * 绘图。
 *************************************************************************************************/
void OxyCRGWidget::paintEvent(QPaintEvent *event)
{
    IWidget::paintEvent(event);

    QPainter barPainter(this);

    // 绘制边框。
    QPen pen;
    pen.setColor(Qt::white);
    pen.setWidth(2);
    barPainter.setPen(pen);
    barPainter.drawRect(rect());

    QRect rectAdjust = rect().adjusted(1, TITLE_BAR_HEIGHT - 1, -1, -LABEL_HEIGHT - 4);
//    barPainter.drawRect(rectAdjust);
    barPainter.drawLine(rectAdjust.bottomLeft(), rectAdjust.bottomRight());

    // 画上x轴刻度线
    barPainter.setPen(QPen(Qt::white, 1, Qt::SolidLine));

    int xStep = (rect().width() - WX_SHIFT - WX_SHIFT) / 4;
    barPainter.setFont(fontManager.textFont(fontManager.getFontSize(0)));
    for (int i = 1; i < 4; i++)
    {
        barPainter.drawText(rect().bottomRight().x() - WX_SHIFT - i * xStep,
                            rectAdjust.bottomRight().y() + 12,
                            QString::number(i));
    }


    QRect r = rect();
    r.setBottom(TITLE_BAR_HEIGHT);
    barPainter.fillRect(r, QColor(152, 245, 255));

    int rWidth = rect().width() / 4;
    int addWidth = (rWidth - _interval->width()) / 2;
    int rHeight = rect().bottom() - LABEL_HEIGHT;

    _interval->move(addWidth, rHeight);
    _changeTrend->move((addWidth + rWidth), rHeight);
    d_ptr->setRulerAuto->move((addWidth + rWidth * 2), rHeight);
    d_ptr->setupBtn->move((addWidth + rWidth * 3), rHeight);
}

/**************************************************************************************************
 * 调整大小。
 *************************************************************************************************/
void OxyCRGWidget::resizeEvent(QResizeEvent *e)
{
    IWidget::resizeEvent(e);
}

void OxyCRGWidget::showEvent(QShowEvent *e)
{
    IWidget::showEvent(e);
    if (d_ptr->rrHrTrend)
    {
        d_ptr->rrHrTrend->setClearWaveDataStatus(true);
    }
    if (d_ptr->spo2Trend)
    {
        d_ptr->spo2Trend->setClearWaveDataStatus(true);
    }
    if (d_ptr->respWave)
    {
        d_ptr->respWave->setClearWaveDataStatus(true);
    }
    if (d_ptr->co2Wave)
    {
        d_ptr->co2Wave->setClearWaveDataStatus(true);
    }
}

void OxyCRGWidget::hideEvent(QHideEvent *e)
{
    IWidget::hideEvent(e);
    if (d_ptr->rrHrTrend)
    {
        d_ptr->rrHrTrend->setClearWaveDataStatus(true);
    }
    if (d_ptr->spo2Trend)
    {
        d_ptr->spo2Trend->setClearWaveDataStatus(true);
    }
    if (d_ptr->respWave)
    {
        d_ptr->respWave->setClearWaveDataStatus(true);
    }
    if (d_ptr->co2Wave)
    {
        d_ptr->co2Wave->setClearWaveDataStatus(true);
    }
}

void OxyCRGWidget::setOxyCRGRespWidget(OxyCRGRESPWaveWidget *p)
{
    if (p != NULL)
    {
        d_ptr->respWave = p;
    }
}


void OxyCRGWidget::setOxyCRGSPO2Trend(OxyCRGSPO2TrendWidget *p)
{
    if (p != NULL)
    {
        d_ptr->spo2Trend = p;
    }
}

void OxyCRGWidget::setOxyCRGCO2Widget(OxyCRGCO2WaveWidget *p)
{
    if (p != NULL)
    {
        d_ptr->co2Wave = p;
    }
}

void OxyCRGWidget::setWaveType(int index)
{
    if (index == 0 || index == 1)
    {
        _waveType = index;
    }
}

int OxyCRGWidget::getWaveType()const
{
    return _waveType;
}

void OxyCRGWidget::getSubFocusWidget(QList<QWidget *> &subWidget) const
{
    subWidget.clear();
    subWidget << _interval << _changeTrend << d_ptr->setRulerAuto << d_ptr->setupBtn;
}

void OxyCRGWidget::setOxyCRGRrHrWidget(OxyCRGRRHRWaveWidget *p)
{
    if (p)
    {
        d_ptr->rrHrTrend = p;
    }
}

/**************************************************************************************************
 * 时间时间触发。
 *************************************************************************************************/
void OxyCRGWidget::_intervalSlot(IWidget *widget)
{
    Q_UNUSED(widget)
    if (NULL == _intervalList)
    {
        _intervalList = new PopupList(_interval, false);
        for (int i = 0; i < OxyCRG_Interval_NR; i++)
        {
            _intervalList->addItemText(OxyCRGSymbol::convert(OxyCRGInterval(i)));
        }
        _intervalList->setFont(fontManager.textFont(fontManager.getFontSize(3)));
        connect(_intervalList, SIGNAL(destroyed()), this, SLOT(_intervalDestroyed()));
        connect(_intervalList, SIGNAL(selectItemChanged(int)), this, SLOT(_getIntervalIndex(int)));
    }

    _intervalList->show();
}

/**************************************************************************************************
 * 时间间隔更改操作。
 *************************************************************************************************/
void OxyCRGWidget::_changeTrendSlot(IWidget *widget)
{
    Q_UNUSED(widget)
    if (NULL == _changeTrendList)
    {
        _changeTrendList = new PopupList(_changeTrend, false);
        for (int i = 0; i < OxyCRG_Trend_NR; i++)
        {
            _changeTrendList->addItemText(OxyCRGSymbol::convert(OxyCRGTrend(i)));
        }
        _changeTrendList->setFont(fontManager.textFont(fontManager.getFontSize(3)));
        connect(_changeTrendList, SIGNAL(destroyed()), this, SLOT(_changeTrendDestroyed()));
        connect(_changeTrendList, SIGNAL(selectItemChanged(int)), this, SLOT(_getChangeTrendIndex(int)));
    }

    _changeTrendList->show();
}

/**************************************************************************************************
 * 设置更新操作。
 *************************************************************************************************/
void OxyCRGWidget::_onSetupUpdated(IWidget *widget)
{
    if (widget != d_ptr->setupBtn)
    {
        return;
    }

    windowManager.showWindow(d_ptr->setupWin, WindowManager::ShowBehaviorModal);

    int index = d_ptr->setupWin->getWaveTypeIndex();
    if (index != (getWaveType()) && (index == 0 || index == 1))
    {
        setWaveType(index);
        _setTrend((OxyCRGTrend)index);
        _clearLayout();
        _trendLayout();
    }
    index = 0;
    currentConfig.getNumValue("OxyCRG|Trend1", index);
    d_ptr->rrHrTrend->setRrTrendShowStatus(!!index);

    int valueLow;
    int valueHight;

    valueLow = d_ptr->setupWin->getCO2Low();
    valueHight = d_ptr->setupWin->getCO2High();
    d_ptr->co2Wave->setRulerValue(valueHight, valueLow);

    valueLow = d_ptr->setupWin->getHRLow();
    valueHight = d_ptr->setupWin->getHRHigh();
    d_ptr->rrHrTrend->setHrRulerValue(valueHight, valueLow);

    valueLow = d_ptr->setupWin->getRRLow();
    valueHight = d_ptr->setupWin->getRRHigh();
    d_ptr->rrHrTrend->setRrRulerValue(valueHight, valueLow);

    valueLow = d_ptr->setupWin->getSPO2Low();
    valueHight = d_ptr->setupWin->getSPO2High();
    d_ptr->spo2Trend->setRulerValue(valueHight, valueLow);
}

void OxyCRGWidget::_intervalDestroyed()
{
    if (_intervalItemIndex == -1)
    {
        _intervalList = NULL;
        return;
    }

    _setInterval((OxyCRGInterval)_intervalItemIndex);

    _intervalList = NULL;
}

/**************************************************************************************************
 * 切换显示操作。
 *************************************************************************************************/
void OxyCRGWidget::_changeTrendDestroyed()
{
    if (_changeTrendItemIndex == -1)
    {
        _changeTrendList = NULL;
        return;
    }

    _setTrend((OxyCRGTrend)_changeTrendItemIndex);

    _clearLayout();

    _trendLayout();

    _changeTrendList = NULL;
}

void OxyCRGWidget::_getIntervalIndex(int index)
{
    _intervalItemIndex = index;
}

void OxyCRGWidget::_getChangeTrendIndex(int index)
{
    _changeTrendItemIndex = index;
}

void OxyCRGWidget::_autoSetRuler()
{
    if (d_ptr->rrHrTrend)
    {
        QString strValueHigh = OxyCRGSymbol::convert(RR_HIGH_40);
        int valueHigh = strValueHigh.toInt();
        QString strValueLow = OxyCRGSymbol::convert(RR_LOW_0);
        int valueLow = strValueLow.toInt();

        d_ptr->rrHrTrend->setRrRulerValue(valueHigh, valueLow);
        currentConfig.setNumValue("OxyCRG|Ruler|RRLow", static_cast<int>(RR_LOW_0));
        currentConfig.setNumValue("OxyCRG|Ruler|RRHigh", static_cast<int>(RR_HIGH_40));
    }

    if (d_ptr->rrHrTrend)
    {
        QString strValueHigh = OxyCRGSymbol::convert(HR_HIGH_200);
        int valueHigh = strValueHigh.toInt();
        QString strValueLow = OxyCRGSymbol::convert(HR_LOW_40);
        int valueLow = strValueLow.toInt();

        d_ptr->rrHrTrend->setHrRulerValue(valueHigh, valueLow);
        currentConfig.setNumValue("OxyCRG|Ruler|HRLow", static_cast<int>(HR_LOW_40));
        currentConfig.setNumValue("OxyCRG|Ruler|HRHigh", static_cast<int>(HR_HIGH_200));
    }

    if (d_ptr->spo2Trend)
    {
        QString strValueHigh = OxyCRGSymbol::convert(SPO2_HIGH_100);
        int valueHigh = strValueHigh.toInt();
        QString strValueLow = OxyCRGSymbol::convert(SPO2_LOW_92);
        int valueLow = strValueLow.toInt();

        d_ptr->spo2Trend->setRulerValue(valueHigh, valueLow);
        currentConfig.setNumValue("OxyCRG|Ruler|SPO2Low", static_cast<int>(SPO2_LOW_92));
        currentConfig.setNumValue("OxyCRG|Ruler|SPO2High", static_cast<int>(SPO2_HIGH_100));
    }

    if (d_ptr->co2Wave)
    {
        QString strValueHigh = OxyCRGSymbol::convert(CO2_HIGH_15);
        int valueHigh = strValueHigh.toInt();
        QString strValueLow = OxyCRGSymbol::convert(CO2_LOW_0);
        int valueLow = strValueLow.toInt();

        d_ptr->co2Wave->setRulerValue(valueHigh, valueLow);

        currentConfig.setNumValue("OxyCRG|Ruler|CO2Low", static_cast<int>(CO2_LOW_0));
        currentConfig.setNumValue("OxyCRG|Ruler|CO2High", static_cast<int>(CO2_HIGH_15));
    }
}
