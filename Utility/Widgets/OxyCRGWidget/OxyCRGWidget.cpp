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
#include "OxyCRGSetupWidget.h"
#include "OxyCRGSetupWindow.h"
#include "IMessageBox.h"
#include "ECGParam.h"
#include "OxyCRGRRWidget.h"

class OxyCRGWidgetPrivate
{
public:
    OxyCRGWidgetPrivate()
            : setRulerAuto(NULL),
              oxycrgRrWidget(NULL)
    {
    }

    OxyCRGWidgetLabel *setRulerAuto;
    OxyCRGRRWidget *oxycrgRrWidget;
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
    _titleLabel->setFixedHeight(_titleBarHeight);
    int fontSize = fontManager.getFontSize(7);
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

    _interval = new OxyCRGWidgetLabel("", Qt::AlignCenter, this);
    _interval->setFont(fontManager.textFont(fontSize));
    _interval->setFixedSize(labelWidth, _labelHeight);
    _interval->setText("");
    connect(_interval, SIGNAL(released(IWidget *)), this, SLOT(_intervalSlot(IWidget *)));

    _changeTrend = new OxyCRGWidgetLabel("", Qt::AlignCenter, this);
    _changeTrend->setFont(fontManager.textFont(fontSize));
    _changeTrend->setFixedSize(labelWidth, _labelHeight);
    _changeTrend->setText("");
    connect(_changeTrend, SIGNAL(released(IWidget *)), this, SLOT(_changeTrendSlot(IWidget *)));

    OxyCRGWidgetLabel *setRulerAuto = new OxyCRGWidgetLabel("", Qt::AlignCenter, this);
    setRulerAuto->setFont(fontManager.textFont(fontSize));
    setRulerAuto->setFixedSize(labelWidth, _labelHeight);
    setRulerAuto->setText(trs("Auto"));
    connect(setRulerAuto, SIGNAL(released(IWidget *)), this, SLOT(_autoSetRuler()));
    d_ptr->setRulerAuto = setRulerAuto;

    _setUp = new OxyCRGWidgetLabel("", Qt::AlignCenter, this);
    _setUp->setFont(fontManager.textFont(fontSize));
    _setUp->setFixedSize(labelWidth, _labelHeight);
    _setUp->setText(trs("SetUp"));
    connect(_setUp, SIGNAL(released(IWidget *)), this, SLOT(_onSetupUpdated(IWidget *)));

    int rWidth = rect().width() / 4;
    int addWidth = (rWidth - labelWidth) / 2;
    bottomLayout->addSpacing(addWidth);
    bottomLayout->addWidget(_interval);
    bottomLayout->addSpacing(addWidth);
    bottomLayout->addWidget(_changeTrend);
    bottomLayout->addSpacing(addWidth);
    bottomLayout->addWidget(setRulerAuto);
    bottomLayout->addSpacing(addWidth);
    bottomLayout->addWidget(_setUp);
    bottomLayout->addSpacing(addWidth);

    _mainLayout->addWidget(_titleLabel, 0, Qt::AlignCenter);
    _mainLayout->addLayout(_hLayoutWave);
    _mainLayout->addLayout(bottomLayout);

    // 设置布局。
    setLayout(_mainLayout);

    setFocusPolicy(Qt::NoFocus);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGWidget::~OxyCRGWidget()
{
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

//    IWidget *w = NULL;
    for (int i = 0; i < nodeWidgets.size(); i++)
    {
        IWidget *w = windowManager.getWidget(nodeWidgets[i]);
        if (w == NULL)
        {
            continue;
        }
        w->setParent(this);            // 设置父窗体。
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
    currentConfig.setNumValue("RESP|Interval", static_cast<int>(index));

    _interval->setText(OxyCRGSymbol::convert(OxyCRGInterval(index)));

    int len = _oxycrgHrWidget->getRulerWidth();
    float pixel = _oxycrgHrWidget->getRulerPixWidth();
    int speedPara;
    switch (index)
    {
    case OxyCRG_Interval_1:
        speedPara = 1;
        break;
    case OxyCRG_Interval_2:
        speedPara = 2;
        break;
    case OxyCRG_Interval_4:
        speedPara = 4;
        break;
    case OxyCRG_Interval_8:
        speedPara = 8;
        break;
    default:
        speedPara = 1;
        break;
    };
    float speed = len * pixel / (60 * speedPara);

    if (_oxycrgWidget != NULL)
    {
        _oxycrgWidget->setWaveSpeed(speed);
    }
    if (_oxycrgCo2Widget != NULL)
    {
        _oxycrgCo2Widget->setWaveSpeed(speed);
    }
    if (_oxycrgHrWidget != NULL)
    {
        _oxycrgHrWidget->setWaveSpeed(speed);
    }

    if (_oxycrgSpo2Widget != NULL)
    {
        _oxycrgSpo2Widget->setWaveSpeed(speed);
    }

    ecgParam.clearOxyCRGWaveNum();
}

/**************************************************************************************************
 * 获取时间间隔。
 *************************************************************************************************/
OxyCRGInterval OxyCRGWidget::_getInterval()
{
    int index = OxyCRG_Interval_1;
    currentConfig.getNumValue("RESP|Interval", index);

    return (OxyCRGInterval)index;
}

/**************************************************************************************************
 * 设置RESP/CO2。
 *************************************************************************************************/
void OxyCRGWidget::_setTrend(OxyCRGTrend index)
{
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

    QWidget::setVisible(visible);
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

    QRect rectAdjust = rect().adjusted(1, _titleBarHeight - 1, -1, -_labelHeight - 4);
//    barPainter.drawRect(rectAdjust);
    barPainter.drawLine(rectAdjust.bottomLeft(), rectAdjust.bottomRight());

    // 画上x轴刻度线
    barPainter.setPen(QPen(Qt::white, 1, Qt::SolidLine));
    for (int i = 0; i < 4; i++) // 分成10份
    {
        barPainter.setFont(fontManager.textFont(fontManager.getFontSize(0)));
        // 选取合适的坐标，绘制一段长度为4的直线，用于表示刻度
//        barPainter.drawLine(rectAdjust.bottomRight().x() - (i * (rectAdjust.bottomRight().x() / 4)),rectAdjust.bottomRight().y(),
//                         rectAdjust.bottomRight().x() - (i * (rectAdjust.bottomRight().x() / 4)),(rectAdjust.bottomRight().y() + 4));

        barPainter.drawText((rectAdjust.bottomRight().x() - (i * (rectAdjust.bottomRight().x() / 4))) + 2,
                            (rectAdjust.bottomRight().y() + 12)
                            , QString::number(i));
    }

    QRect r = rect();
    r.setBottom(_titleBarHeight);
    barPainter.fillRect(r, QColor(152, 245, 255));

    int rWidth = rect().width() / 4;
    int addWidth = (rWidth - _interval->width()) / 2;
    int rHeight = rect().bottom() - _labelHeight;

    _interval->move(addWidth, rHeight);
    _changeTrend->move((addWidth + rWidth), rHeight);
    d_ptr->setRulerAuto->move((addWidth + rWidth * 2), rHeight);
    _setUp->move((addWidth + rWidth * 3), rHeight);
}

/**************************************************************************************************
 * 调整大小。
 *************************************************************************************************/
void OxyCRGWidget::resizeEvent(QResizeEvent *e)
{
    IWidget::resizeEvent(e);
}

void OxyCRGWidget::setOxyCrgRespWidget(OxyCRGRESPWidget *p)
{
    if (p != NULL)
    {
        _oxycrgWidget = p;
    }
}

void OxyCRGWidget::setOxyCrgHrWidget(OxyCRGHRWidget *p)
{
    if (p != NULL)
    {
        _oxycrgHrWidget = p;
    }
}

void OxyCRGWidget::setOxyCrgSpo2Widget(OxyCRGSPO2Widget *p)
{
    if (p != NULL)
    {
        _oxycrgSpo2Widget = p;
    }
}

void OxyCRGWidget::setOxyCrgCo2Widget(OxyCRGCO2Widget *p)
{
    if (p != NULL)
    {
        _oxycrgCo2Widget = p;
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

/**************************************************************************************************
 * 时间时间触发。
 *************************************************************************************************/
void OxyCRGWidget::_intervalSlot(IWidget *widget)
{
    if (NULL == _intervalList)
    {
        _intervalList = new PopupList(_interval, false);
        for (int i = 0; i < OxyCRG_Interval_NR; i++)
        {
            _intervalList->addItemText(OxyCRGSymbol::convert(OxyCRGInterval(i)));
        }
        _intervalList->setFont(fontManager.textFont(fontManager.getFontSize(3)));
        connect(_intervalList, SIGNAL(destroyed()), this, SLOT(_intervalDestroyed()));
        connect(_intervalList, SIGNAL(selectItemChanged(int)), this , SLOT(_getIntervalIndex(int)));
    }

    _intervalList->show();
}

/**************************************************************************************************
 * 时间间隔更改操作。
 *************************************************************************************************/
void OxyCRGWidget::_changeTrendSlot(IWidget *widget)
{
    if (NULL == _changeTrendList)
    {
        _changeTrendList = new PopupList(_changeTrend, false);
        for (int i = 0; i < OxyCRG_Trend_NR; i++)
        {
            _changeTrendList->addItemText(OxyCRGSymbol::convert(OxyCRGTrend(i)));
        }
        _changeTrendList->setFont(fontManager.textFont(fontManager.getFontSize(3)));
        connect(_changeTrendList, SIGNAL(destroyed()), this, SLOT(_changeTrendDestroyed()));
        connect(_changeTrendList, SIGNAL(selectItemChanged(int)), this , SLOT(_getChangeTrendIndex(int)));
    }

    _changeTrendList->show();
}

/**************************************************************************************************
 * 设置更新操作。
 *************************************************************************************************/
void OxyCRGWidget::_onSetupUpdated(IWidget *widget)
{
    if (widget == _setUp)
    {
        OxyCRGSetupWindow setupWidget;

        windowManager.showWindow(&setupWidget, WindowManager::ShowBehaviorModal);

        int index = setupWidget.getWaveTypeIndex();
        if (index != (getWaveType()) && (index == 0 || index == 1))
        {
            ecgParam.clearOxyCRGWaveNum();

            setWaveType(index);

            _setTrend((OxyCRGTrend)index);
            _clearLayout();
            _trendLayout();
        }

        bool status = false;
        int valueLow = 0;
        int valueHight = 0;
        int valueMid = 0;

        valueLow = setupWidget.getCO2Low(status);
        valueHight = setupWidget.getCO2High(status);
        valueMid = (valueHight + valueLow) / 2;
        if (status)
        {
            status = false;
            _oxycrgCo2Widget->setRuler(valueHight, valueMid, valueLow);
            _oxycrgCo2Widget->setValueRange(valueLow, valueHight);
        }

        valueLow = setupWidget.getHRLow(status);
        valueHight = setupWidget.getHRHigh(status);
        valueMid = (valueHight + valueLow) / 2;
        if (status)
        {
            status = false;
            _oxycrgHrWidget->setRuler(valueHight, valueMid, valueLow);
            _oxycrgHrWidget->setValueRange(valueLow, valueHight);
        }

        valueLow = setupWidget.getSPO2Low(status);
        valueHight = setupWidget.getSPO2High(status);
        valueMid = (valueHight + valueLow) / 2;
        if (status)
        {
            _oxycrgSpo2Widget->setRuler(valueHight, valueMid, valueLow);
            _oxycrgSpo2Widget->setValueRange(valueLow, valueHight);
        }
    }
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

    ecgParam.clearOxyCRGWaveNum();

    setWaveType(_changeTrendItemIndex);

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
    if (_oxycrgHrWidget)
    {
        QString strValueHigh = OxyCRGSymbol::convert(HR_HIGH_200);
        int valueHigh = strValueHigh.toInt();
        QString strValueLow = OxyCRGSymbol::convert(HR_LOW_40);
        int valueLow = strValueLow.toInt();

        _oxycrgHrWidget->setRuler(valueHigh,
                                  (valueHigh + valueLow) / 2, valueLow);
        _oxycrgHrWidget->setValueRange(valueLow, valueHigh);
        currentConfig.setNumValue("OxyCRG|Ruler|HRLow", static_cast<int>(HR_LOW_40));
        currentConfig.setNumValue("OxyCRG|Ruler|HRHigh", static_cast<int>(HR_HIGH_200));
    }

    if (d_ptr->oxycrgRrWidget)
    {
        QString strValueHigh = OxyCRGSymbol::convert(RR_HIGH_40);
        int valueHigh = strValueHigh.toInt();
        QString strValueLow = OxyCRGSymbol::convert(RR_LOW_0);
        int valueLow = strValueLow.toInt();

        d_ptr->oxycrgRrWidget->setRuler(valueHigh,
                                  (valueHigh + valueLow) / 2, valueLow);
        d_ptr->oxycrgRrWidget->setValueRange(valueLow, valueHigh);
        currentConfig.setNumValue("OxyCRG|Ruler|RRLow", static_cast<int>(RR_LOW_0));
        currentConfig.setNumValue("OxyCRG|Ruler|RRHigh", static_cast<int>(RR_HIGH_40));
    }

    if (_oxycrgSpo2Widget)
    {
        QString strValueHigh = OxyCRGSymbol::convert(SPO2_HIGH_100);
        int valueHigh = strValueHigh.toInt();
        QString strValueLow = OxyCRGSymbol::convert(SPO2_LOW_92);
        int valueLow = strValueLow.toInt();

        _oxycrgSpo2Widget->setRuler(valueHigh,
                                  (valueHigh + valueLow) / 2, valueLow);
        _oxycrgSpo2Widget->setValueRange(valueLow, valueHigh);
        currentConfig.setNumValue("OxyCRG|Ruler|SPO2Low", static_cast<int>(SPO2_LOW_92));
        currentConfig.setNumValue("OxyCRG|Ruler|SPO2High", static_cast<int>(SPO2_HIGH_100));
    }

    if (_oxycrgCo2Widget)
    {
        QString strValueHigh = OxyCRGSymbol::convert(CO2_HIGH_15);
        int valueHigh = strValueHigh.toInt();
        QString strValueLow = OxyCRGSymbol::convert(CO2_LOW_0);
        int valueLow = strValueLow.toInt();

        _oxycrgCo2Widget->setRuler(valueHigh,
                                  (valueHigh + valueLow) / 2, valueLow);
        _oxycrgCo2Widget->setValueRange(valueLow, valueHigh);
        currentConfig.setNumValue("OxyCRG|Ruler|CO2Low", static_cast<int>(CO2_LOW_0));
        currentConfig.setNumValue("OxyCRG|Ruler|CO2High", static_cast<int>(CO2_HIGH_15));
    }
}
