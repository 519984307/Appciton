#include <QBoxLayout>
#include "ShortTrendWidgetSelectMenu.h"
#include "ShortTrendManager.h"
#include "ParamManager.h"
#include "WindowManager.h"
#include "ECGParam.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "IComboList.h"
#include "IButton.h"
#include "SystemManager.h"
#include "Debug.h"
#include "IConfig.h"

ShortTrendWidgetSelectMenu *ShortTrendWidgetSelectMenu::_selfObj = NULL;

/**************************************************************************************************
 * 替换波形。
 *************************************************************************************************/
void ShortTrendWidgetSelectMenu::_replaceListSlot(int index)
{
    shortTrendManager.replaceTrendform(widgetName, replaceWidget[index]);
    widgetName = replaceWidget[index];
    _loadWidget();
}

/**************************************************************************************************
 * 插入新波形。
 *************************************************************************************************/
void ShortTrendWidgetSelectMenu::_insertListSlot(int index)
{
    if (_insertList->combolist->itemText(index).isEmpty())
    {
        _insertList->setCurrentIndex(-1);
        return;
    }

    shortTrendManager.insertTrendform(widgetName, insertWidget[index]);
    _loadWidget();
    hide();
}

/**************************************************************************************************
 * 移除。
 *************************************************************************************************/
void ShortTrendWidgetSelectMenu::_removeButtonSlot(void)
{
    shortTrendManager.removeTrendform(widgetName);
    hide();
}


/**************************************************************************************************
 * 载入波形控件的名称。
 *************************************************************************************************/
void ShortTrendWidgetSelectMenu::_loadWidget(void)
{
    QStringList displayedWaveform;
    shortTrendManager.getSubParamList(displayedWaveform);

    replaceWidget.clear();
    replaceWidgetTitles.clear();
    insertWidget.clear();
    insertWidgetTitles.clear();

    // 获取ECG当前支持的波形窗体。
//    ecgParam.getAvailableWaveforms(replaceWidget, replaceWidgetTitles, _isTopWaveform);
    insertWidget = replaceWidget;
    insertWidgetTitles = replaceWidgetTitles;

    // 其他参数支持的波形窗体。
    QList<Param*> params;
    paramManager.getParams(params);

    QStringList waveforms;
    QStringList waveformTitles;
    QStringList otherWaveforms;
    QStringList otherWaveformTitles;
    for (int i = 0; i < params.size(); i++)
    {
        if (!params[i]->isEnabled())
        {
            continue;
        }

        QList<SubParamID> SubParam;
        paramInfo.getSubParams(params[i]->getParamID(),SubParam);
        for (int j = 0; j < SubParam.count(); j++)
        {
            waveforms += paramInfo.getSubParamName(SubParam[j]);
            waveformTitles += paramInfo.getSubParamName(SubParam[j]);
        }
        otherWaveforms += waveforms;
        otherWaveformTitles += waveformTitles;
        waveforms.clear();
        waveformTitles.clear();
    }

    // 根据是否为Top Waveform将选项添加到替换和插入列表。
    insertWidget += otherWaveforms;
    insertWidgetTitles += otherWaveformTitles;
    replaceWidget += otherWaveforms;
    replaceWidgetTitles += otherWaveformTitles;

    // 排除已经显示的波形。
    int pos = -1;
    for (int i = 0; i < displayedWaveform.size(); i++)
    {
        if ((pos = replaceWidget.indexOf(displayedWaveform[i])) != -1)
        {
            if (replaceWidget[pos] != widgetName)  // 不移除自己。
            {
                replaceWidget.removeAt(pos);
                replaceWidgetTitles.removeAt(pos);
            }
        }

        if ((pos = insertWidget.indexOf(displayedWaveform[i])) != -1)
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
    int trendNR = shortTrendManager.getTrendNR(widgetName);
    if (trendNR <= 0)
    {
        _insertList->setEnabled(false);
    }
    else
    {
        _insertList->setEnabled(true);
    }

    if (trendNR < 3)
    {
        _removeButton->setEnabled(true);
    }
    else
    {
        _removeButton->setEnabled(false);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ShortTrendWidgetSelectMenu::ShortTrendWidgetSelectMenu() : WidgetSelectMenu(trs("WaveformSources"))
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ShortTrendWidgetSelectMenu::~ShortTrendWidgetSelectMenu()
{

}
