#include <QBoxLayout>
#include "TrendWidgetSelectMenu.h"
#include "ParamManager.h"
#include "WindowManager.h"
#include "ECGParam.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "IComboList.h"
#include "IButton.h"
#include "SystemManager.h"
#include "Debug.h"

TrendWidgetSelectMenu *TrendWidgetSelectMenu::_selfObj = NULL;

/**************************************************************************************************
 * 替换波形。
 *************************************************************************************************/
void TrendWidgetSelectMenu::_replaceListSlot(int index)
{
    windowManager.replacebigWidgetform(widgetName, replaceWidget[index]);
    widgetName = replaceWidget[index];
    _loadWidget();
}

/**************************************************************************************************
 * 插入新波形。
 *************************************************************************************************/
void TrendWidgetSelectMenu::_insertListSlot(int index)
{
    if (_insertList->combolist->itemText(index).isEmpty())
    {
        _insertList->setCurrentIndex(-1);
        return;
    }

    windowManager.insertWaveform(widgetName, insertWidget[index]);
    _loadWidget();
    hide();
}

/**************************************************************************************************
 * 移除。
 *************************************************************************************************/
void TrendWidgetSelectMenu::_removeButtonSlot(void)
{
    windowManager.removeWaveform(widgetName);
    hide();
}

/**************************************************************************************************
 * 载入波形控件的名称。
 *************************************************************************************************/
void TrendWidgetSelectMenu::_loadWidget(void)
{
    QStringList displayedTrend;
    windowManager.getDisplayTrendWindow(displayedTrend);

    replaceWidget.clear();
    replaceWidgetTitles.clear();
    insertWidget.clear();
    insertWidgetTitles.clear();

    int index = -1;
    if (ecgParam.getLeadMode() == ECG_LEAD_MODE_3)
    {
        QStringList ecgLead3WaveName;
        ecgLead3WaveName.clear();
        ecgLead3WaveName << ecgParam.getWaveWidgetName(ECG_LEAD_I);
        ecgLead3WaveName << ecgParam.getWaveWidgetName(ECG_LEAD_II);
        ecgLead3WaveName << ecgParam.getWaveWidgetName(ECG_LEAD_III);

        for (index = 0; index <= 2; ++index)
        {
            if (-1 != displayedTrend.indexOf(ecgLead3WaveName.at(index)))
            {
                break;
            }
        }

        // 只显示一道ECG
        if (index <= 2)
        {
            int pos = -1;
            for (int i = 0; i <= 2; ++i)
            {
                pos = insertWidget.indexOf(ecgLead3WaveName.at(i));
                if (-1 != pos)
                {
                    insertWidget.removeAt(pos);
                    insertWidgetTitles.removeAt(pos);
                }

                if (-1 == ecgLead3WaveName.indexOf(widgetName))
                {
                    pos = replaceWidget.indexOf(ecgLead3WaveName.at(i));
                    if (-1 != pos)
                    {
                        replaceWidget.removeAt(pos);
                        replaceWidgetTitles.removeAt(pos);
                    }
                }
            }
        }
    }

    // 其他参数支持的波形窗体。
    QList<Param*> params;
    paramManager.getParams(params);

    QStringList trend;
    QStringList trendTitles;
    QStringList otherTrend;
    QStringList otherTrendTitles;
    for (int i = 0; i < params.size(); i++)
    {
        if (!params[i]->isEnabled())
        {
            continue;
        }

//        params[i]->getTrendWindow(trend);
//        otherTrend += trend;
//        otherTrendTitles += trendTitles;
//        trend.clear();
//        trendTitles.clear();
    }

    // 根据是否为Top Waveform将选项添加到替换和插入列表。
    insertWidget += otherTrend;
    insertWidgetTitles += otherTrendTitles;
    replaceWidget += otherTrend;
    replaceWidgetTitles += otherTrendTitles;

    // 排除已经显示的波形。
    int pos = -1;
    for (int i = 0; i < displayedTrend.size(); i++)
    {
        if ((pos = replaceWidget.indexOf(displayedTrend[i])) != -1)
        {
            if (replaceWidget[pos] != widgetName)  // 不移除自己。
            {
                replaceWidget.removeAt(pos);
                replaceWidgetTitles.removeAt(pos);
            }
        }

        if ((pos = insertWidget.indexOf(displayedTrend[i])) != -1)
        {
            insertWidget.removeAt(pos);
            insertWidgetTitles.removeAt(pos);
        }
    }

    // 将波形名称添加到下拉框。
    _replaceList->clear();
    _replaceList->addItems(replaceWidgetTitles);
    _insertList->clear();
    _insertList->addItems(insertWidgetTitles);

    // 添加一项空值，以便放弃插入波形
    _insertList->addItem(QString());

    // 定位到当前项。
    for (int i = 0; i < replaceWidget.size(); i++)
    {
        if (replaceWidget[i] == widgetName)
        {
            _replaceList->setCurrentIndex(i);
            break;
        }
    }

    // 风格统一，默认没有当前选择项
    _insertList->setCurrentIndex(-1);

    // 根据目前界面的波形道数情况禁用_insertList。
    if (windowManager.getLeftWaveformChannelNR() <= 0)
    {
        _insertList->setEnabled(false);
    }
    else
    {
        _insertList->setEnabled(true);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TrendWidgetSelectMenu::TrendWidgetSelectMenu() : WidgetSelectMenu(trs("WaveformSources"), true)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TrendWidgetSelectMenu::~TrendWidgetSelectMenu()
{

}
