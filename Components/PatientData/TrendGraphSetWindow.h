/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/30
 **/

#pragma once
#include "Framework/UI/Dialog.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/UI/SpinBox.h"
#include <QScopedPointer>
#include <QHBoxLayout>
#include <QLabel>
#include "ParamInfo.h"
#include "TrendDataDefine.h"

class RulerItem : public QWidget
{
    Q_OBJECT
public:
    RulerItem() : QWidget()
    {
        label = new QLabel();
        label->setAlignment(Qt::AlignCenter);
        combo = new ComboBox();
        downRuler = new SpinBox();
        upRuler = new SpinBox();
        downRuler->setArrow(false);
        upRuler->setArrow(false);

        QHBoxLayout *mainlayout = new QHBoxLayout();

        mainlayout->addWidget(label);
        mainlayout->addWidget(combo);
        mainlayout->addWidget(downRuler);
        mainlayout->addWidget(upRuler);

        setLayout(mainlayout);

        setFocusPolicy(Qt::StrongFocus);
        setFocusProxy(combo);
    }
    ~RulerItem()
    {
    }

public:
    SubParamID sid;

    QLabel *label;
    ComboBox *combo;
    SpinBox *downRuler;
    SpinBox *upRuler;
};

class TrendGraphSetWindowPrivate;
class TrendGraphSetWindow : public Dialog
{
    Q_OBJECT
public:
    static TrendGraphSetWindow &construction(void)
    {
        if (selfObj == NULL)
        {
            selfObj = new TrendGraphSetWindow();
        }
        return *selfObj;
    }
    ~TrendGraphSetWindow();

public:
    /**
     * @brief getTrendGroup 获取当前趋势组
     * @return
     */
    TrendGroup getTrendGroup();

    /**
     * @brief upDateTrendGroup 更新对应趋势组的数据
     */
    void upDateTrendGroup(void);


    /**
     * @brief setCurParam 设置当前显示参数的标尺更新窗口
     * @param subList
     */
    void setCurParam(QList<SubParamID> subList);

protected:
    void showEvent(QShowEvent *ev);

private slots:
    /**
     * @brief _allAutoReleased all ruler auto slot funtion
     */
    void allAutoReleased();

    /**
     * @brief _trendGroupReleased set trend group slot funtion
     */
    void trendGroupReleased(int trendGroup);

    /**
     * @brief _timeIntervalReleased set time interval slot funtion
     */
    void timeIntervalReleased(int timeInterval);

    /**
     * @brief _waveNumberReleased set waveform number slot funtion
     */
    void waveNumberReleased(int waveNum);

    void onComboBoxChanged(int index);

    /**
     * @brief upDownRulerChange 上下限标尺改变
     * @param value 值
     * @param scale 比例
     */
    void upDownRulerChange(int value, int scale);

private:
    TrendGraphSetWindow();
    static TrendGraphSetWindow *selfObj;
    QScopedPointer<TrendGraphSetWindowPrivate> d_ptr;
};
#define trendGraphSetWindow         (TrendGraphSetWindow::construction())
#define deleteTrendGraphSetWindow   (delete TrendGraphSetWindow::selfObj)
