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
#include "Framework/Language/LanguageManager.h"
#include "WindowManager.h"
#include "ColorManager.h"
#include "OxyCRGWidgetLabel.h"
#include <QBoxLayout>
#include <QLabel>
#include <QMap>
#include "IWidget.h"
#include "ConfigManager.h"
#include <QPainter>
#include "Framework/UI/PopupList.h"
#include "ParamInfo.h"
#include "SystemManager.h"
#include "OxyCRGSetupWindow.h"
#include "ECGParam.h"
#include "OxyCRGTrendWaveWidget.h"
#include "OxyCRGCO2WaveWidget.h"
#include "OxyCRGSPO2TrendWidget.h"
#include "OxyCRGRESPWaveWidget.h"
#include "OxyCRGRRHRWaveWidget.h"
#include "LayoutManager.h"
#include "OxyCRGTrendWaveWidget_p.h"
#include <QStackedWidget>
#include <QTimer>
#include "OxyCRGEventWindow.h"

#define TITLE_BAR_HEIGHT    24
#define LABEL_HEIGHT    40

class OxyCRGWidgetPrivate
{
public:
    explicit OxyCRGWidgetPrivate(OxyCRGWidget *q)
        : autoLbl(NULL),
          rrHrTrend(NULL),
          spo2Trend(NULL),
          co2Wave(NULL),
          respWave(NULL),
          eventBtn(NULL),
          recordBtn(NULL),
          setupBtn(NULL),
          setupWin(NULL),
          updateTimer(NULL),
          intervalLbl(NULL),
          firstTrendStackWidget(NULL),
          secondTrendStackWidget(NULL),
          compressWaveStackWidget(NULL),
          q_ptr(q)
    {
    }

    void setInterval(OxyCRGInterval interval);

    OxyCRGInterval getInterval() const;

    OxyCRGWave getCompressWave() const;
    void setCompressWave(OxyCRGWave wave);

    /**
     * @brief moveBottomLables
     */
    void moveBottomLables(void);

    ~OxyCRGWidgetPrivate()
    {
        delete setupWin;
    }

    OxyCRGWidgetLabel *autoLbl;
    OxyCRGRRHRWaveWidget *rrHrTrend;
    OxyCRGSPO2TrendWidget *spo2Trend;
    OxyCRGCO2WaveWidget *co2Wave;
    OxyCRGRESPWaveWidget *respWave;
    OxyCRGWidgetLabel *eventBtn;
    OxyCRGWidgetLabel *recordBtn;
    OxyCRGWidgetLabel *setupBtn;
    OxyCRGSetupWindow *setupWin;
    QTimer *updateTimer;
    OxyCRGWidgetLabel *intervalLbl;
    QStackedWidget *firstTrendStackWidget;
    QStackedWidget *secondTrendStackWidget;
    QStackedWidget *compressWaveStackWidget;
    QStringList intervalShowList;
    OxyCRGWidget *q_ptr;
};

void OxyCRGWidgetPrivate::setInterval(OxyCRGInterval interval)
{
    currentConfig.setNumValue("OxyCRG|Interval", static_cast<int>(interval));

    intervalLbl->setText(OxyCRGSymbol::convert(interval));
    intervalShowList.clear();
    intervalShowList = OxyCRGSymbol::convertInterval(static_cast<OxyCRGInterval>(interval));
    intervalLbl->setText(trs(OxyCRGSymbol::convert(interval)));

    if (rrHrTrend)
    {
        rrHrTrend->setInterval(interval);
        rrHrTrend->update();
    }
    if (spo2Trend)
    {
        spo2Trend->setInterval(interval);
        spo2Trend->update();
    }
    if (respWave)
    {
        respWave->setInterval(interval);
        respWave->update();
    }
    if (co2Wave)
    {
        co2Wave->setInterval(interval);
        co2Wave->update();
    }
}

OxyCRGInterval OxyCRGWidgetPrivate::getInterval() const
{
    int index = OXYCRG_INTERVAL_1;
    currentConfig.getNumValue("OxyCRG|Interval", index);

    return (OxyCRGInterval)index;
}

OxyCRGWave OxyCRGWidgetPrivate::getCompressWave() const
{
    int index = OXYCRG_WAVE_RESP;
    currentConfig.getNumValue("OxyCRG|Wave", index);

    return static_cast<OxyCRGWave>(index);
}

void OxyCRGWidgetPrivate::setCompressWave(OxyCRGWave wave)
{
    int index = wave;
    currentConfig.setNumValue("OxyCRG|Wave", index);

    if (index < compressWaveStackWidget->count())
    {
        compressWaveStackWidget->setCurrentIndex(index);
        moveBottomLables();
    }
}

void OxyCRGWidgetPrivate::moveBottomLables()
{
    QRect r = q_ptr->rect();
    int rHeight = r.bottom() - LABEL_HEIGHT;
    int xShift = r.x();
    int xStep = (r.width() - WX_SHIFT - WX_SHIFT) / 4;

    eventBtn->move(xShift, rHeight);

    xShift = r.right() - WX_SHIFT - 3 * xStep;
    xShift -= intervalLbl->width() / 2;
    intervalLbl->move(xShift, rHeight);

    xShift = r.right() - WX_SHIFT - 2 * xStep;
    xShift -= setupBtn->width() / 2;
    setupBtn->move(xShift, rHeight);

    xShift = r.right() - WX_SHIFT - 1 * xStep;
    xShift -= autoLbl->width() / 2;
    autoLbl->move(xShift, rHeight);

    xShift = r.right() - recordBtn->width();
    recordBtn->move(xShift, rHeight);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGWidget::OxyCRGWidget(): IWidget("OxyCRGWidget"),
    d_ptr(new OxyCRGWidgetPrivate(this))
{
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(152, 245, 255));
    palette.setColor(QPalette::Window, Qt::black);
    setPalette(palette);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    QVBoxLayout *hLayoutWave = new QVBoxLayout();
    hLayoutWave->setMargin(1);
    hLayoutWave->setSpacing(0);
    d_ptr->firstTrendStackWidget = new QStackedWidget;
    hLayoutWave->addWidget(d_ptr->firstTrendStackWidget, 1);
    d_ptr->secondTrendStackWidget = new QStackedWidget;
    hLayoutWave->addWidget(d_ptr->secondTrendStackWidget, 1);
    d_ptr->compressWaveStackWidget = new QStackedWidget;
    hLayoutWave->addWidget(d_ptr->compressWaveStackWidget, 1);

    // 标题栏。
    QLabel *titleLabel = new QLabel(trs("UserFaceOxyCRG"));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFixedHeight(TITLE_BAR_HEIGHT);
    int fontSize = 24;
    titleLabel->setFont(fontManager.textFont(fontSize));
    QPalette p = titleLabel->palette();
    p.setColor(QPalette::WindowText, Qt::black);
    p.setColor(QPalette::Window, QColor(255, 255, 0));
    titleLabel->setPalette(p);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setMargin(2);
    bottomLayout->setSpacing(1);
    int labelWidth = 80;
    fontSize = 18;

    d_ptr->eventBtn = new OxyCRGWidgetLabel(trs("Event"), Qt::AlignCenter, this);
    d_ptr->eventBtn->setFont(fontManager.textFont(fontSize));
    d_ptr->eventBtn->setFixedSize(labelWidth, LABEL_HEIGHT);
    connect(d_ptr->eventBtn, SIGNAL(released(IWidget *)), this, SLOT(onEventClicked(IWidget*)));

    d_ptr->intervalLbl = new OxyCRGWidgetLabel(QString(), Qt::AlignCenter, this);
    d_ptr->intervalLbl->setFont(fontManager.textFont(fontSize));
    d_ptr->intervalLbl->setFixedSize(labelWidth, LABEL_HEIGHT);
    int interval = OXYCRG_INTERVAL_1;
    currentConfig.getNumValue("OxyCRG|Interval", interval);
    d_ptr->intervalShowList = OxyCRGSymbol::convertInterval(static_cast<OxyCRGInterval>(interval));
    d_ptr->intervalLbl->setText(OxyCRGSymbol::convert(static_cast<OxyCRGInterval>(interval)));
    d_ptr->intervalLbl->setText(trs(OxyCRGSymbol::convert(static_cast<OxyCRGInterval>(interval))));
    connect(d_ptr->intervalLbl, SIGNAL(released(IWidget *)), this, SLOT(onIntervalClicked(IWidget *)));

    d_ptr->setupBtn = new OxyCRGWidgetLabel(trs("SetUp"), Qt::AlignCenter, this);
    d_ptr->setupBtn->setFont(fontManager.textFont(fontSize));
    d_ptr->setupBtn->setFixedSize(labelWidth, LABEL_HEIGHT);
    connect(d_ptr->setupBtn, SIGNAL(released(IWidget *)), this, SLOT(onSetupClicked(IWidget *)));

    d_ptr->autoLbl = new OxyCRGWidgetLabel(trs("AutoRuler"), Qt::AlignCenter, this);
    d_ptr->autoLbl->setFont(fontManager.textFont(fontSize));
    d_ptr->autoLbl->setFixedSize(labelWidth * 3 / 2, LABEL_HEIGHT);
    connect(d_ptr->autoLbl, SIGNAL(released(IWidget *)), this, SLOT(onAutoClicked()));

    d_ptr->recordBtn = new OxyCRGWidgetLabel(trs("Record"), Qt::AlignCenter, this);
    d_ptr->recordBtn->setFont(fontManager.textFont(fontSize));
    d_ptr->recordBtn->setFixedSize(labelWidth, LABEL_HEIGHT);
    connect(d_ptr->recordBtn, SIGNAL(released(IWidget *)), this, SLOT(onRecordClicked(IWidget*)));

    bottomLayout->addWidget(d_ptr->eventBtn);
    bottomLayout->addWidget(d_ptr->intervalLbl);
    bottomLayout->addWidget(d_ptr->setupBtn);
    bottomLayout->addWidget(d_ptr->autoLbl);
    bottomLayout->addWidget(d_ptr->recordBtn);

    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    mainLayout->addLayout(hLayoutWave);
    mainLayout->addLayout(bottomLayout);

    // 设置布局。
    setLayout(mainLayout);

    setFocusPolicy(Qt::NoFocus);

    d_ptr->setupWin =  new OxyCRGSetupWindow;
    d_ptr->updateTimer = new QTimer(this);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGWidget::~OxyCRGWidget()
{
    delete d_ptr;
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
    QRect r = rect();
    pen.setColor(Qt::white);
    pen.setWidth(2);
    barPainter.setPen(pen);
    barPainter.drawRect(rect());

    r.setBottom(TITLE_BAR_HEIGHT);
    barPainter.fillRect(r, QColor(152, 245, 255));

    QRect rectAdjust = rect().adjusted(1, TITLE_BAR_HEIGHT - 1, -1, -LABEL_HEIGHT - 4);
    barPainter.drawLine(rectAdjust.bottomLeft(), rectAdjust.bottomRight());

    // 画时间间隔
    if (d_ptr->intervalShowList.count() != 4)
    {
        return;
    }
    int xStep = (rect().width() - WX_SHIFT - WX_SHIFT) / 4;
    barPainter.setPen(QPen(Qt::white, 1, Qt::SolidLine));
    barPainter.setFont(fontManager.textFont(fontManager.getFontSize(0)));
    for (int i = 0; i < 4; i++)
    {
        barPainter.drawText(rect().bottomRight().x() - WX_SHIFT - xStep / 2 - i * xStep,
                            rectAdjust.bottomRight().y() + 12,
                            d_ptr->intervalShowList.at(i));
    }
}

/**************************************************************************************************
 * 调整大小。
 *************************************************************************************************/
void OxyCRGWidget::resizeEvent(QResizeEvent *e)
{
    IWidget::resizeEvent(e);

    d_ptr->moveBottomLables();
}

void OxyCRGWidget::showEvent(QShowEvent *e)
{
    IWidget::showEvent(e);

    d_ptr->setCompressWave(d_ptr->getCompressWave());

    if (d_ptr->rrHrTrend)
    {
        int index = 0;
        d_ptr->rrHrTrend->clearData();
        currentConfig.getNumValue("OxyCRG|Trend1", index);
        d_ptr->rrHrTrend->setRrTrendShowStatus(!!index);
    }
    if (d_ptr->spo2Trend)
    {
        d_ptr->spo2Trend->clearData();
    }
    if (d_ptr->respWave)
    {
        d_ptr->respWave->clearData();
    }
    if (d_ptr->co2Wave)
    {
        d_ptr->co2Wave->clearData();
    }
    d_ptr->updateTimer->start(1000);
}

void OxyCRGWidget::hideEvent(QHideEvent *e)
{
    IWidget::hideEvent(e);
    d_ptr->updateTimer->stop();
}

void OxyCRGWidget::setOxyCRGRespWidget(OxyCRGRESPWaveWidget *p)
{
    if (p != NULL)
    {
        d_ptr->respWave = p;
        connect(d_ptr->updateTimer, SIGNAL(timeout()), p, SLOT(update()));
        d_ptr->compressWaveStackWidget->addWidget(p);
    }
}

void OxyCRGWidget::setOxyCRGSPO2Trend(OxyCRGSPO2TrendWidget *p)
{
    if (p != NULL)
    {
        d_ptr->spo2Trend = p;
        connect(d_ptr->updateTimer, SIGNAL(timeout()), p, SLOT(update()));
        d_ptr->secondTrendStackWidget->addWidget(p);
    }
}

void OxyCRGWidget::setOxyCRGCO2Widget(OxyCRGCO2WaveWidget *p)
{
    if (p != NULL)
    {
        d_ptr->co2Wave = p;
        connect(d_ptr->updateTimer, SIGNAL(timeout()), p, SLOT(update()));
        d_ptr->compressWaveStackWidget->addWidget(p);
    }
}

void OxyCRGWidget::getSubFocusWidget(QList<QWidget *> &subWidget) const
{
    subWidget.clear();
    subWidget << d_ptr->eventBtn
              << d_ptr->intervalLbl
              << d_ptr->setupBtn
              << d_ptr->autoLbl
              << d_ptr->recordBtn;
}

void OxyCRGWidget::setOxyCRGRrHrWidget(OxyCRGRRHRWaveWidget *p)
{
    if (p)
    {
        d_ptr->rrHrTrend = p;
        connect(d_ptr->updateTimer, SIGNAL(timeout()), p, SLOT(update()));
        d_ptr->firstTrendStackWidget->addWidget(p);
    }
}

void OxyCRGWidget::onIntervalClicked(IWidget *widget)
{
    Q_UNUSED(widget)

    PopupList *popup = new PopupList(d_ptr->intervalLbl, false);

    for (int i = 0; i < OXYCRG_INTERVAL_NR; i++)
    {
        popup->addItemText(trs(OxyCRGSymbol::convert(static_cast<OxyCRGInterval>(i))));
    }

    popup->setFont(fontManager.textFont(fontManager.getFontSize(3)));
    int index = OXYCRG_INTERVAL_1;
    currentConfig.getNumValue("OxyCRG|Interval", index);
    popup->setCurrentIndex(index);
    connect(popup, SIGNAL(selectItemChanged(int)), this, SLOT(onIntervalChanged(int)));
    popup->show();
}

/**************************************************************************************************
 * 设置更新操作。
 *************************************************************************************************/
void OxyCRGWidget::onSetupClicked(IWidget *widget)
{
    if (widget != d_ptr->setupBtn)
    {
        return;
    }

    windowManager.showWindow(d_ptr->setupWin, WindowManager::ShowBehaviorModal);

    int index = d_ptr->setupWin->getWaveTypeIndex();
    d_ptr->setCompressWave(static_cast<OxyCRGWave>(index));

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

void OxyCRGWidget::onEventClicked(IWidget *widget)
{
    if (widget != d_ptr->eventBtn)
    {
        return;
    }
    OxyCRGEventWindow::getInstance()->setHistoryData(false);
    windowManager.showWindow(OxyCRGEventWindow::getInstance(),
                             WindowManager::
                             ShowBehaviorCloseOthers);
}

void OxyCRGWidget::onRecordClicked(IWidget *widget)
{
    if (widget != d_ptr->recordBtn)
    {
        return;
    }
    /* TODO */
}

void OxyCRGWidget::onIntervalChanged(int index)
{
    d_ptr->setInterval(static_cast<OxyCRGInterval>(index));
    update();
}

void OxyCRGWidget::onAutoClicked()
{
    /* TODO:  this is not a auto ruler setting function, fixed it */
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
        QString strValueHigh = OxyCRGSymbol::convert(CO2_HIGH_40);
        int valueHigh = strValueHigh.toInt();
        QString strValueLow = OxyCRGSymbol::convert(CO2_LOW_0);
        int valueLow = strValueLow.toInt();

        d_ptr->co2Wave->setRulerValue(valueHigh, valueLow);

        currentConfig.setNumValue("OxyCRG|Ruler|CO2Low", static_cast<int>(CO2_LOW_0));
        currentConfig.setNumValue("OxyCRG|Ruler|CO2High", static_cast<int>(CO2_HIGH_40));
    }
}
