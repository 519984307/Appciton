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
#include "ParamDefine.h"


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
    virtual void updatePalette(const QPalette &pal);
     /*
     * @brief getShortTrendSubParams get the avaliable short trend subparam IDs
     * @return a list of avaliable short trend params
     */
    virtual QList<SubParamID> getShortTrendSubParams() const {return QList<SubParamID>();}

    /**
     * @brief restoreNormalStatusLater restore the normal display mode in about 500 ms
     */
    void restoreNormalStatusLater();

    // 刷新趋势参数上下限
    virtual void updateLimit(void) {}

public slots:
    /**
     * @brief doRestoreNormalStatus do the restore normal status stuff
     */
    virtual void doRestoreNormalStatus() {}

    virtual void doAlarmOff(SubParamID subParamId);
protected:
    void setName(const QString &name);
    void setUnit(const QString &unit);
    void setLimit(int up, int down, int scale);

    void setNameFont(int size, bool isBold = false);
    void setUnitFont(int size, bool isBold = false);

    QHBoxLayout *contentLayout;
    QHBoxLayout *mLayout;

    /**
     * @brief loadConfig  加载配置
     */
    virtual void loadConfig();

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

    //字体大小设置
    virtual void setTextSize(void) = 0;

    virtual void showAlarmOff();

protected:
    static const int barHeight = 21;

    TrendWidgetLabel *nameLabel;
    QLabel *unitLabel;

    QLabel *upLimit;
    QLabel *downLimit;

    QLabel *alarmOffIcon;

    QString _title;                           // 趋势控制标名

    void normalPalette(QPalette &psrc);
    QPalette alarmPalette(QPalette psrc);   // isSetName: 是否设置标题颜色
    /**
     * @brief showAlarmStatus   设置报警时状态：闪烁＋白底红字
     * @param value             趋势值
     * @param psrc              对应字体调色板
     * @param isSetName         是否设置标题颜色
     */
    void showAlarmStatus(QWidget *value);

    /**
     * @brief showAlarmParamLimit   闪烁对应的上下报警限
     * @param psrc                  参数对应的调色板
     * @param valueStr              当前参数值字符串
     */
    void showAlarmParamLimit(QWidget *value, const QString &valueStr, QPalette psrc);

    /**
     * @brief showNormalStatus  设置正常时状态：黑底＋对应颜色字体(刷新全部控件颜色)
     * @param psrc              对应字体调色板
     */
    void showNormalStatus(QPalette psrc);

    void showNormalStatus(QLayout *layout, QPalette psrc);

    /**
     * @brief showNormalStatus 设置正常时状态：黑底＋对应颜色字体(刷新某一控件颜色)
     * @param value
     * @param psrc
     */
    void showNormalStatus(QWidget* value, QPalette psrc);

    /**
     * @brief showNormalParamLimit  设置正常时参数报警限状态
     * @param psrc                  参数对应字体调色板
     */
    void showNormalParamLimit(QPalette psrc);

    /**
     * @brief darkParamLimit 使得上下限颜色变暗
     */
    void darkerPalette(QPalette &pal);

private:
    void setWidgetPalette(QLayout *layout, QPalette psrc);
};
