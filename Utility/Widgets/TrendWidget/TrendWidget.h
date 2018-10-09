/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/21
 **/


#pragma once
#include "IWidget.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "ColorManager.h"
#include "UnitManager.h"
#include <QString>
#include <QLabel>
#include <QPalette>

class QHBoxLayout;
class QVBoxLayout;
class TrendWidgetLabel;
class TrendWidget: public IWidget
{
    Q_OBJECT
public:
    explicit TrendWidget(const QString &widgetName, bool vertical = false);
    virtual ~TrendWidget();

    // 窗口是否使能
    virtual bool enable() {return true;}

    // 更新报警标志
    void updateAlarm(bool alarmFlag);

    // 获取显示的名称。
    const QString &getTitle(void) const
    {
        return _title;
    }

    /**
     * @brief updateBackground 更新背景
     * @param pal
     */
    virtual void updatePalette(const QPalette pal);

protected:
    void setName(const QString &name);
    void setUnit(const QString &unit);
    void setCalcLeadName(const QString &calcLead);

    void setNameFont(int size, bool isBold = false);
    void setUnitFont(int size, bool isBold = false);

    QHBoxLayout *contentLayout;
    QHBoxLayout *mLayout;

protected:
    void paintEvent(QPaintEvent *e);
    void showEvent(QShowEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

    //字体大小设置
    virtual void setTextSize(void) = 0;

protected:
    static const int barHeight = 21;

    TrendWidgetLabel *nameLabel;
    QLabel *calcLeadLabel;
    QLabel *unitLabel;

    QString _title;                           // 趋势控制标名

    QPalette normalPalette(QPalette psrc);
    QPalette alarmPalette(QPalette psrc, bool isSetName = true);   // isSetName: 是否设置标题颜色
    /**
     * @brief showAlarmStatus   设置报警时状态：闪烁＋白底红字
     * @param value             趋势值
     * @param psrc              对应字体调色板
     * @param isSetName         是否设置标题颜色
     */
    void showAlarmStatus(QWidget *value, QPalette psrc, bool isSetName = true);

    /**
     * @brief showNormalStatus  设置正常时状态：黑底＋对应颜色字体
     * @param value             趋势值
     * @param psrc              对应字体调色板
     */
    void showNormalStatus(QWidget *value, QPalette psrc);
};
