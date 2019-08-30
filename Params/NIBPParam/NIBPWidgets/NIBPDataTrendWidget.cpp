/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include "NIBPDataTrendWidget.h"
#include "ParamManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "Debug.h"
#include "NIBPAlarm.h"
#include "WindowManager.h"
#include "ITableWidget.h"
#include "TimeDate.h"
#include "Alarm.h"
#include "TrendWidgetLabel.h"
#include "MeasureSettingWindow.h"
#include "AlarmSourceManager.h"
#include "EventStorageManager.h"
#include "NIBPAlarm.h"
#include "IConfig.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void NIBPDataTrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("NIBPMenu"));
}

void NIBPDataTrendWidget::_releaseHandle()
{
}

QString NIBPDataTrendWidget::getPriotityColor(AlarmPriority prio)
{
    switch (prio)
    {
    case ALARM_PRIO_HIGH:
        return QString("red");
    case ALARM_PRIO_MED:
    case ALARM_PRIO_LOW:
        return QString("yellow");
    default:
        return QString("black");
    }
}

/**************************************************************************************************
 * 设置HR的值。
 *************************************************************************************************/
void NIBPDataTrendWidget::setHRValue(int16_t hr, bool isHR)
{
    if (isHR)
    {
        setName(trs(paramInfo.getSubParamName(SUB_PARAM_HR_PR)));
    }
    else
    {
        setName(trs(paramInfo.getSubParamName(SUB_PARAM_RR_BR)));
    }

    if (hr == InvData())
    {
        _hrString = InvStr();
    }
    else
    {
        _hrString = QString::number(hr);
    }
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void NIBPDataTrendWidget::isAlarm(bool isAlarm)
{
    _isAlarm = isAlarm;

    updateAlarm(isAlarm);
}

/**************************************************************************************************
 * 函数说明：收集趋势数据。
 * 函数说明：收集趋势数据。
 * 函数参数：
 *         data:保存趋势数据
 *************************************************************************************************/
void NIBPDataTrendWidget::collectNIBPTrendData(unsigned t)
{
    if (_nibpNrendCacheMap.contains(t))
    {
        return;
    }

    NIBPTrendCacheData data;
    QList<ParamID> paramIDList;
    paramManager.getParams(paramIDList);

    data.lastNibpMeasureTime = t;
    data.sys.value = paramManager.getSubParamValue(PARAM_NIBP, SUB_PARAM_NIBP_SYS);
    data.dia.value = paramManager.getSubParamValue(PARAM_NIBP, SUB_PARAM_NIBP_DIA);
    data.map.value = paramManager.getSubParamValue(PARAM_NIBP, SUB_PARAM_NIBP_MAP);
    data.valueIsDisplay = paramManager.isParamEnable(PARAM_NIBP);
    data.prvalue = nibpParam.getPR();

    if (10 <= _nibpNrendCacheMap.count())
    {
        _nibpNrendCacheMap.remove(_nibpNrendCacheMap.begin().key());
    }

    _nibpNrendCacheMap.insert(t, data);
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void NIBPDataTrendWidget::showValue(void)
{
    NIBPTrendCacheMap::iterator t = _nibpNrendCacheMap.end() - 1;
    if (_nibpNrendCacheMap.end() == _nibpNrendCacheMap.begin())
    {
        return;
    }
    QString textStr;
    QString timeStr;
    QString prStr;

    UnitType defUnitType;
    UnitType unit;
    NIBPLimitAlarm alarm;

    QColor textColor = colorManager.getColor(paramInfo.getParamName(PARAM_NIBP));

    for (int i = 0; i < _rowNR; i++)
    {
        _table->item(i, 0)->setText("");
        _table->item(i, 0)->setTextColor(textColor);
        QLabel *l = qobject_cast<QLabel *>(_table->cellWidget(i, 1));
        l->setText("");
        if (moduleStr != "BLM_N5")
        {
            l = qobject_cast<QLabel *>(_table->cellWidget(i, 2));
            l->setText("");
        }
    }

    for (int i = 0; i < _rowNR; i++)
    {
        timeDate.getTime(t.key(), timeStr);
        _table->item(i, 0)->setText(timeStr);
        bool sysAlarm = false;
        bool diaAlarm = false;
        bool mapAlarm = false;
        NIBPTrendCacheData providerBuff = t.value();
        if (providerBuff.sys.value == InvData() || providerBuff.dia.value == InvData() ||
                providerBuff.map.value == InvData())
        {
            QString color = "<font style='color:rgb(%1,%2,%3);'>%4</font>";
            textStr = QString("<center>%1/%2 (%3)</center>").arg(InvStr()).arg(InvStr()).arg(InvStr());
            textStr = color.arg(textColor.red()).arg(textColor.green()).arg(textColor.blue()).arg(textStr);
            prStr = QString("<center>%1</center>").arg(InvStr());
            prStr = color.arg(textColor.red()).arg(textColor.green()).arg(textColor.blue()).arg(prStr);
        }
        else
        {
            QString valStr;
            QString boldwrap = "<b>%1</b>";
            QString colorwrap = "<font color=%1>%2</font>";
            QString color = "<font style='color:rgb(%1,%2,%3);'>%4</font>";
            textStr = QString("<center>%1/%2 (%3)</center>");
            prStr = QString("<center>%1</center>");

            defUnitType = paramInfo.getUnitOfSubParam(SUB_PARAM_NIBP_SYS);
            unit = paramManager.getSubParamUnit(PARAM_NIBP, SUB_PARAM_NIBP_SYS);
            QString sysValue, diaValue, mapValue;
            if (unit != defUnitType)
            {
                // 非预定的压力单位（非mmHg）
                sysValue = Unit::convert(unit, defUnitType, providerBuff.sys.value);
                diaValue = Unit::convert(unit, defUnitType, providerBuff.dia.value);
                mapValue = Unit::convert(unit, defUnitType, providerBuff.map.value);
            }
            else
            {
                sysValue = QString::number(providerBuff.sys.value);
                diaValue = QString::number(providerBuff.dia.value);
                mapValue = QString::number(providerBuff.map.value);
            }
            AlarmLimitIFace *alarmSource = alarmSourceManager.getLimitAlarmSource(LIMIT_ALARMSOURCE_NIBP);
            if (alarmSource)
            {
                if (alarm.isAlarmEnable(NIBP_LIMIT_ALARM_SYS_LOW))
                {
                    if (alarmSource->getCompare(providerBuff.sys.value, NIBP_LIMIT_ALARM_SYS_LOW)
                            || alarmSource->getCompare(providerBuff.sys.value, NIBP_LIMIT_ALARM_SYS_HIGH))
                    {
                        sysAlarm = true;
                    }
                }
                if (alarm.isAlarmEnable(NIBP_LIMIT_ALARM_DIA_LOW))
                {
                    if (alarmSource->getCompare(providerBuff.dia.value, NIBP_LIMIT_ALARM_DIA_LOW)
                            || alarmSource->getCompare(providerBuff.dia.value, NIBP_LIMIT_ALARM_DIA_HIGH))
                    {
                        diaAlarm = true;
                    }
                }
                if (alarm.isAlarmEnable(NIBP_LIMIT_ALARM_MAP_LOW))
                {
                    if (alarmSource->getCompare(providerBuff.map.value, NIBP_LIMIT_ALARM_MAP_LOW)
                            || alarmSource->getCompare(providerBuff.map.value, NIBP_LIMIT_ALARM_MAP_HIGH))
                    {
                        mapAlarm = true;
                    }
                }
            }
            if (sysAlarm)
            {
                valStr = boldwrap.arg(colorwrap
                                      .arg(getPriotityColor(alarm.getAlarmPriority(NIBP_LIMIT_ALARM_SYS_LOW)))
                                      .arg(sysValue));
                textStr = textStr.arg(valStr);
            }
            else
            {
                textStr = textStr.arg(sysValue);
            }

            if (diaAlarm)
            {
                valStr = boldwrap.arg(colorwrap
                                      .arg(getPriotityColor(alarm.getAlarmPriority(NIBP_LIMIT_ALARM_DIA_LOW)))
                                      .arg(diaValue));
                textStr = textStr.arg(valStr);
            }
            else
            {
                textStr = textStr.arg(diaValue);
            }

            if (mapAlarm)
            {
                valStr = boldwrap.arg(colorwrap
                                      .arg(getPriotityColor(alarm.getAlarmPriority(NIBP_LIMIT_ALARM_MAP_LOW)))
                                      .arg(mapValue));
                textStr = textStr.arg(valStr);
            }
            else
            {
                textStr = textStr.arg(mapValue);
            }

            // PR
            if (moduleStr != "BLM_N5")
            {
                if (!providerBuff.prAlarm)
                {
                    valStr = color.arg(textColor.red())
                            .arg(textColor.green())
                            .arg(textColor.blue())
                            .arg(QString::number(providerBuff.prvalue));
                    prStr = prStr.arg(valStr);
                }
            }
            textStr = color.arg(textColor.red()).arg(textColor.green()).arg(textColor.blue()).arg(textStr);
        }
        QLabel *l = qobject_cast<QLabel *>(_table->cellWidget(i, 1));
        l->setAlignment(Qt::AlignHCenter);
        l->setText(textStr);
        l->setTextInteractionFlags(Qt::NoTextInteraction);
        if (moduleStr != "BLM_N5")
        {
            QLabel *prLbl = qobject_cast<QLabel *>(_table->cellWidget(i, 2));
            prLbl->setAlignment(Qt::AlignHCenter);
            prLbl->setText(prStr);
            prLbl->setTextInteractionFlags(Qt::NoTextInteraction);
        }
        if (t != _nibpNrendCacheMap.begin())
        {
            t = t - 1;
        }
        else
        {
            return;
        }
    }
}

/**************************************************************************************************
 * 窗体变化触发。
 *************************************************************************************************/
void NIBPDataTrendWidget::resizeEvent(QResizeEvent *e)
{
    TrendWidget::resizeEvent(e);
    _rowNR = _table->height() / _tableItemHeight - 1;
    _table->setRowCount(_rowNR);
    int eachColumnWidth = _table->width() / columnNR;

    if (moduleStr != "BLM_N5")
    {
        _table->setColumnWidth(0, eachColumnWidth);
        _table->setColumnWidth(1, eachColumnWidth *8/5);
        _table->setColumnWidth(2, eachColumnWidth *2/5);
    }
    else
    {
        _table->setColumnWidth(0, eachColumnWidth);
        _table->setColumnWidth(1, eachColumnWidth);
    }
    for (int i = 0; i < _rowNR; i++)
    {
        QColor textColor = colorManager.getColor(paramInfo.getParamName(PARAM_NIBP));
        _table->setRowHeight(i, _tableItemHeight);
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignCenter);
        item->setTextColor(textColor);
        _table->setItem(i, 0, item);

        QLabel *l = new QLabel();
        _table->setCellWidget(i, 1, l);
        l->setText("");

        if (moduleStr != "BLM_N5")
        {
            QLabel *prLbl = new QLabel();
            _table->setCellWidget(i, 2, prLbl);
            prLbl->setText("");
        }
    }
}

void NIBPDataTrendWidget::setTextSize()
{
    QRect r;
    r.setWidth((width() - nameLabel->width()) / 2.5);
    // 字体。
    int fontsize = fontManager.adjustNumFontSize(r, true, "220/150/100");
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);

    _table->setFont(font);

    _tableItemHeight = fontManager.textHeightInPixels(font);
    _table->setFixedWidth(width() - nameLabel->width());
}

void NIBPDataTrendWidget::updatePalette(const QPalette &pal)
{
    setPalette(pal);
    // 刷新背景
    QColor color = colorManager.getColor(paramInfo.getParamName(PARAM_NIBP));
    QString headStyle = QString("QHeaderView::section{color:rgb(%1,%2,%3);"
                                "border:0px solid black;"
                                "background-color:black;}")
                        .arg(color.red()).arg(color.green()).arg(color.blue());
    _table->horizontalHeader()->setStyleSheet(headStyle);
}

void NIBPDataTrendWidget::updateWidgetConfig()
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    updatePalette(palette);
}

void NIBPDataTrendWidget::clearListData()
{
    _nibpNrendCacheMap.clear();
    for (int i = 0; i < _rowNR; i++)
    {
        _table->item(i, 0)->setText("");
        QLabel *l = qobject_cast<QLabel *>(_table->cellWidget(i, 1));
        l->setText("");
        if (moduleStr != "BLM_N5")
        {
            l = qobject_cast<QLabel *>(_table->cellWidget(i, 2));
            l->setText("");
        }
    }
}

void NIBPDataTrendWidget::updateUnit(UnitType unit)
{
    setUnit(Unit::getSymbol(unit));
}

void NIBPDataTrendWidget::getTrendNIBPlist()
{
    int eventNum = backend->getBlockNR();
    TrendDataType value;
    SubParamID subId;
    unsigned t = 0;
    NIBPLimitAlarm alarm;
    for (int i = eventNum - 1; i >= 0; i--)
    {
        NIBPTrendCacheData nibpTrendCacheData;
        if (parseEventData(i) && ctx.infoSegment->type == EventNIBPMeasurement)
        {
            t = ctx.infoSegment->timestamp;
            nibpTrendCacheData.lastNibpMeasureTime = t;
            int paramNum = ctx.trendSegment->trendValueNum;
            for (int i = 0; i < paramNum; i++)
            {
                subId = (SubParamID)ctx.trendSegment->values[i].subParamId;
                value = ctx.trendSegment->values[i].value;
                switch (subId)
                {
                case SUB_PARAM_NIBP_SYS:
                    nibpTrendCacheData.sys.value = value;
                    break;
                case SUB_PARAM_NIBP_DIA:
                    nibpTrendCacheData.dia.value = value;
                    break;
                case SUB_PARAM_NIBP_MAP:
                    nibpTrendCacheData.map.value = value;
                    break;
                case SUB_PARAM_NIBP_PR:
                    nibpTrendCacheData.prvalue = value;
                    break;
                default:
                    break;
                }
            }
            AlarmLimitIFace *alarmSource = alarmSourceManager.getLimitAlarmSource(LIMIT_ALARMSOURCE_NIBP);
            if (alarmSource)
            {
                int completeResult = alarmSource->getCompare(nibpTrendCacheData.sys.value, NIBP_LIMIT_ALARM_SYS_LOW);
                if (completeResult != 0 && alarm.isAlarmEnable(NIBP_LIMIT_ALARM_SYS_LOW))
                {
                    nibpTrendCacheData.sys.isAlarm = true;
                }
                completeResult = alarmSource->getCompare(nibpTrendCacheData.sys.value, NIBP_LIMIT_ALARM_SYS_HIGH);
                if (completeResult != 0 && alarm.isAlarmEnable(NIBP_LIMIT_ALARM_SYS_HIGH))
                {
                    nibpTrendCacheData.sys.isAlarm = true;
                }

                completeResult = alarmSource->getCompare(nibpTrendCacheData.dia.value, NIBP_LIMIT_ALARM_DIA_LOW);
                if (completeResult != 0 && alarm.isAlarmEnable(NIBP_LIMIT_ALARM_DIA_LOW))
                {
                    nibpTrendCacheData.dia.isAlarm = true;
                }
                completeResult = alarmSource->getCompare(nibpTrendCacheData.dia.value, NIBP_LIMIT_ALARM_DIA_HIGH);
                if (completeResult != 0 && alarm.isAlarmEnable(NIBP_LIMIT_ALARM_DIA_HIGH))
                {
                    nibpTrendCacheData.dia.isAlarm = true;
                }

                completeResult = alarmSource->getCompare(nibpTrendCacheData.map.value, NIBP_LIMIT_ALARM_MAP_LOW);
                if (completeResult != 0 && alarm.isAlarmEnable(NIBP_LIMIT_ALARM_MAP_LOW))
                {
                    nibpTrendCacheData.map.isAlarm = true;
                }
                completeResult = alarmSource->getCompare(nibpTrendCacheData.map.value, NIBP_LIMIT_ALARM_MAP_HIGH);
                if (completeResult != 0 && alarm.isAlarmEnable(NIBP_LIMIT_ALARM_MAP_HIGH))
                {
                    nibpTrendCacheData.map.isAlarm = true;
                }

                // 优先级
                nibpTrendCacheData.sys.priority = alarmSource->getAlarmPriority(NIBP_LIMIT_ALARM_SYS_HIGH);
                nibpTrendCacheData.dia.priority = alarmSource->getAlarmPriority(NIBP_LIMIT_ALARM_DIA_HIGH);
                nibpTrendCacheData.map.priority = alarmSource->getAlarmPriority(NIBP_LIMIT_ALARM_MAP_HIGH);

                _nibpNrendCacheMap.insert(t, nibpTrendCacheData);
            }
        }
    }
}
bool NIBPDataTrendWidget::parseEventData(int dataIndex)
{
    ctx.reset();
    return ctx.parse(backend, dataIndex);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPDataTrendWidget::NIBPDataTrendWidget()
    : TrendWidget("NIBPDataTrendWidget"),
      _hrString(InvStr()),
      _isAlarm(false),
      _rowNR(0),
      _tableItemHeight(20),
      backend(NULL),
      moduleStr("BLM_N5"),
      columnNR(3)
{
    machineConfig.getStrValue("NIBP", moduleStr);
    if (moduleStr != "BLM_N5")
    {
        columnNR = 3;
    }
    else
    {
        columnNR = 2;
    }
    _nibpNrendCacheMap.clear();
    // 设置标题栏的相关信息。
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    setPalette(palette);
    setName(trs(paramInfo.getParamName(PARAM_NIBP)));
    setUnit(Unit::getSymbol(nibpParam.getUnit()));

    QColor color = colorManager.getColor(paramInfo.getParamName(PARAM_NIBP));
    QString Style = QString("background-color:transparent;"
                            "color:rgb(%1,%2,%3);")
            .arg(color.red()).arg(color.green()).arg(color.blue());
    QString headStyle = QString("QHeaderView::section{color:rgb(%1,%2,%3);"
                                "border:0px solid black;"
                                "background-color:black;}")
            .arg(color.red()).arg(color.green()).arg(color.blue());
    // 开始布局。
    _table = new ITableWidget();
    _table->setFocusPolicy(Qt::NoFocus);                                  // 不聚焦。
    _table->setColumnCount(columnNR);
    _table->verticalHeader()->setVisible(false);                          // 列首隐藏
    _table->horizontalHeader()->setVisible(true);                        // 列首隐藏
    _table->setShowGrid(false);                                           //显示表格线
    _table->setStyleSheet(Style);
    _table->horizontalHeader()->setStyleSheet(headStyle);
    QStringList titleList = QStringList() << trs("Time")
                                          << trs("NIBPList")
                                          << trs("PR");
    _table->setHorizontalHeaderLabels(titleList);

    connect(_table, SIGNAL(released(QMouseEvent *)), this, SLOT(mouseReleaseEvent(QMouseEvent *)));

    _table->setSelectionMode(QAbstractItemView::NoSelection);

    // 布局。
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(_table);
    contentLayout->addLayout(mainLayout);
    contentLayout->addStretch(1);
    nameLabel->setFixedWidth(76);
    // 释放事件。
    //    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));

    setFocusPolicy(Qt::NoFocus);

    backend = eventStorageManager.backend();

    getTrendNIBPlist();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPDataTrendWidget::~NIBPDataTrendWidget()
{
    _nibpNrendCacheMap.clear();
}
