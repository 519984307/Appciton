/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/7
 **/


#include <QHBoxLayout>
#include <QVBoxLayout>
#include "ECGSTTrendWidget.h"
#include "ParamManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "Debug.h"
#include "ECGSymbol.h"
#include "TrendWidgetLabel.h"
#include "MeasureSettingWindow.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void ECGSTTrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("ECGMenu"));
}

/**************************************************************************************************
 * 设置HR的值。
 *************************************************************************************************/
void ECGSTTrendWidget::setSTValue(ECGST lead, int16_t st)
{
    if (st == InvData())
    {
        _stString[lead] = InvStr();
    }
    else
    {
//        _stString[lead].sprintf("%.1f", st / 10.0);
        _stString[lead] = QString("%1").number(st / 10.0 , 'f' , 1);
    }
    _stValue[lead]->setText(_stString[lead]);
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void ECGSTTrendWidget::isAlarm(int id, bool flag)
{
    _isAlarm[id] = flag;

    updateAlarm(_isAlarm[ECG_ST_I] || _isAlarm[ECG_ST_II] || _isAlarm[ECG_ST_III] ||
                _isAlarm[ECG_ST_aVR] || _isAlarm[ECG_ST_aVL] || _isAlarm[ECG_ST_aVF] ||
                _isAlarm[ECG_ST_V1] || _isAlarm[ECG_ST_V2] || _isAlarm[ECG_ST_V3] ||
                _isAlarm[ECG_ST_V4] || _isAlarm[ECG_ST_V5] || _isAlarm[ECG_ST_V6]);
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void ECGSTTrendWidget::showValue(void)
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    if (_isAlarm[ECG_ST_I] || _isAlarm[ECG_ST_II] || _isAlarm[ECG_ST_III] ||
            _isAlarm[ECG_ST_aVR] || _isAlarm[ECG_ST_aVL] || _isAlarm[ECG_ST_aVF] ||
            _isAlarm[ECG_ST_V1] || _isAlarm[ECG_ST_V2] || _isAlarm[ECG_ST_V3] ||
            _isAlarm[ECG_ST_V4] || _isAlarm[ECG_ST_V5] || _isAlarm[ECG_ST_V6])
    {
        for (int i = ECG_ST_I; i < ECG_ST_NR; i++)
        {
            if (_isAlarm[i])
            {
                showAlarmStatus(_stLabel[i]);
                showAlarmStatus(_stValue[i]);
            }
            else
            {
                showNormalStatus(_stLabel[i], psrc);
                showNormalStatus(_stValue[i], psrc);
            }
        }
        restoreNormalStatusLater();
    }
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void ECGSTTrendWidget::setTextSize(void)
{
//    QRect r = this->rect();
//    r.adjust(nameLabel->width(), 0, 0, 0);
    QRect r;
    r.setSize(QSize(((width() - nameLabel->width()) / 6), (height() / 4)));
    // 字体。
//    int fontsize = fontManager.adjustNumFontSizeXML(r);
//    fontsize = fontManager.getFontSize(fontsize);
    int fontsize = fontManager.adjustNumFontSize(r, true, "2222");
    QFont font = fontManager.numFont(fontsize, true);
//    font.setStretch(105); // 横向放大。
    font.setWeight(QFont::Black);

    QFont font1 = fontManager.numFont(fontsize);
    font1.setWeight(QFont::Black);

    for (int i = ECG_ST_I; i < ECG_ST_NR; i++)
    {
        _stLabel[i]->setFont(font1);

        _stValue[i]->setFont(font);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGSTTrendWidget::ECGSTTrendWidget() : TrendWidget("ECGSTTrendWidget")
{
    // 设置标题栏的相关信息。
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    setPalette(palette);
    setName(trs("ST"));
    setUnit(Unit::getSymbol(UNIT_MV));

    // 设置报警关闭标志
    showAlarmOff();

    QHBoxLayout *hlayout[ECG_ST_NR];
    for (int i = ECG_ST_I; i < ECG_ST_NR; i++)
    {
        _stLabel[i] = new QLabel();
        _stLabel[i]->setPalette(palette);
        _stLabel[i]->setAlignment(Qt::AlignCenter);
        _stLabel[i]->setText(trs(ECGSymbol::convert((ECGST)i)));

        _stValue[i] = new QLabel();
        _stValue[i]->setPalette(palette);
        _stValue[i]->setAlignment(Qt::AlignCenter);
        _stValue[i]->setMinimumWidth(30);
        _stValue[i]->setText(InvStr());

        hlayout[i] = new QHBoxLayout();
        hlayout[i]->setMargin(1);
        hlayout[i]->setSpacing(1);
        hlayout[i]->addWidget(_stLabel[i]);
        hlayout[i]->addSpacing(2);
        hlayout[i]->addWidget(_stValue[i]);

        _isAlarm[i] = false;
        _stString[i] = InvStr();
    }

    QVBoxLayout *vlayout0 = new QVBoxLayout();
    vlayout0->setMargin(0);
    vlayout0->setSpacing(0);
    vlayout0->addLayout(hlayout[ECG_ST_I]);
    vlayout0->addLayout(hlayout[ECG_ST_II]);
    vlayout0->addLayout(hlayout[ECG_ST_III]);
    vlayout0->addLayout(hlayout[ECG_ST_aVR]);


    QVBoxLayout *vlayout1 = new QVBoxLayout();
    vlayout1->setMargin(1);
    vlayout1->setSpacing(1);
    vlayout1->addLayout(hlayout[ECG_ST_aVL]);
    vlayout1->addLayout(hlayout[ECG_ST_aVF]);
    vlayout1->addLayout(hlayout[ECG_ST_V1]);
    vlayout1->addLayout(hlayout[ECG_ST_V2]);

    QVBoxLayout *vlayout2 = new QVBoxLayout();
    vlayout2->setMargin(1);
    vlayout2->setSpacing(1);
    vlayout2->addLayout(hlayout[ECG_ST_V3]);
    vlayout2->addLayout(hlayout[ECG_ST_V4]);
    vlayout2->addLayout(hlayout[ECG_ST_V5]);
    vlayout2->addLayout(hlayout[ECG_ST_V6]);

    QHBoxLayout *mainlayout = new QHBoxLayout();
    mainlayout->setMargin(1);
    mainlayout->setSpacing(1);
    mainlayout->addLayout(vlayout0, 1);
    mainlayout->addLayout(vlayout1, 1);
    mainlayout->addLayout(vlayout2, 1);

//    contentLayout->addStretch(1);
    contentLayout->addLayout(mainlayout);
//    contentLayout->addStretch(1);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGSTTrendWidget::~ECGSTTrendWidget()
{
}

void ECGSTTrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    showNormalStatus(psrc);
}

QList<SubParamID> ECGSTTrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list.append(SUB_PARAM_ST_I);
    list.append(SUB_PARAM_ST_II);
    list.append(SUB_PARAM_ST_III);
    list.append(SUB_PARAM_ST_aVR);
    list.append(SUB_PARAM_ST_aVL);
    list.append(SUB_PARAM_ST_aVF);
    list.append(SUB_PARAM_ST_V1);
    list.append(SUB_PARAM_ST_V2);
    list.append(SUB_PARAM_ST_V3);
    list.append(SUB_PARAM_ST_V4);
    list.append(SUB_PARAM_ST_V5);
    list.append(SUB_PARAM_ST_V6);
    return list;
}
