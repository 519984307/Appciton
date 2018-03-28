#include <QBoxLayout>
#include "WaveWidgetSelectMenu.h"
#include "ParamManager.h"
#include "WindowManager.h"
#include "ECGParam.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "IComboList.h"
#include "IButton.h"
#include "SystemManager.h"
#include "Debug.h"

WaveWidgetSelectMenu *WaveWidgetSelectMenu::_selfObj = NULL;

/**************************************************************************************************
 * 载入波形。
 *************************************************************************************************/
void WaveWidgetSelectMenu::_loadWaveformsSlot(void)
{
    _loadWaveforms();
}

/**************************************************************************************************
 * 替换波形。
 *************************************************************************************************/
void WaveWidgetSelectMenu::_replaceListSlot(int index)
{
    if (_isTopWaveform)
    {
        ecgParam.setCalcLead(_replaceWaveforms[index]);
    }

    ecgParam.setLeadMode3DisplayLead(_replaceWaveforms[index]);
    windowManager.replaceWaveform(_waveformName, _replaceWaveforms[index]);
    _waveformName = _replaceWaveforms[index];
    QTimer::singleShot(0, this, SLOT(_loadWaveformsSlot()));
}

/**************************************************************************************************
 * 插入新波形。
 *************************************************************************************************/
void WaveWidgetSelectMenu::_insertListSlot(int index)
{
    if (_insertList->combolist->itemText(index).isEmpty())
    {
        _insertList->setCurrentIndex(-1);
        return;
    }

    ecgParam.setLeadMode3DisplayLead(_insertWaveforms[index]);
    windowManager.insertWaveform(_waveformName, _insertWaveforms[index]);
//    _loadWaveforms();
    hide();
}

/**************************************************************************************************
 * 移除。
 *************************************************************************************************/
void WaveWidgetSelectMenu::_removeButtonSlot(void)
{
    windowManager.removeWaveform(_waveformName);
    hide();
}

/**************************************************************************************************
 * 载入波形控件的名称。
 *************************************************************************************************/
void WaveWidgetSelectMenu::_loadWaveforms(void)
{
    QStringList displayedWaveform;
    windowManager.getDisplayedWaveform(displayedWaveform);

    _replaceWaveforms.clear();
    _replaceWaveformTitles.clear();
    _insertWaveforms.clear();
    _insertWaveformTitles.clear();

    // 获取ECG当前支持的波形窗体。
    ecgParam.getAvailableWaveforms(_replaceWaveforms, _replaceWaveformTitles, _isTopWaveform);
    _insertWaveforms = _replaceWaveforms;
    _insertWaveformTitles = _replaceWaveformTitles;

    if (_isTopWaveform)
    {
        // 如果是Top Waveform则从insert菜单中剔除PADS;
        _insertWaveforms.removeFirst();
        _insertWaveformTitles.removeFirst();
    }

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
            if (-1 != displayedWaveform.indexOf(ecgLead3WaveName.at(index)))
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
                pos = _insertWaveforms.indexOf(ecgLead3WaveName.at(i));
                if (-1 != pos)
                {
                    _insertWaveforms.removeAt(pos);
                    _insertWaveformTitles.removeAt(pos);
                }

                if (-1 == ecgLead3WaveName.indexOf(_waveformName))
                {
                    pos = _replaceWaveforms.indexOf(ecgLead3WaveName.at(i));
                    if (-1 != pos)
                    {
                        _replaceWaveforms.removeAt(pos);
                        _replaceWaveformTitles.removeAt(pos);
                    }
                }
            }
        }
    }

    // 其他参数支持的波形窗体。
    QList<Param*> params;
    paramManager.getParams(params);

    QStringList waveforms;
    QStringList waveformTitles;
    QStringList otherWaveforms;
    QStringList otherWaveformTitles;
    for (int i = 0; i < params.size(); i++)
    {
        if (params[i]->getParamID() != PARAM_ECG)
        {
            if (!params[i]->isEnabled())
            {
                continue;
            }

            params[i]->getAvailableWaveforms(waveforms, waveformTitles);
            otherWaveforms += waveforms;
            otherWaveformTitles += waveformTitles;
            waveforms.clear();
            waveformTitles.clear();
        }
    }

    // 根据是否为Top Waveform将选项添加到替换和插入列表。
    _insertWaveforms += otherWaveforms;
    _insertWaveformTitles += otherWaveformTitles;
    if (!_isTopWaveform)  // 只有非Top Waveform才能替换成非ECG窗体。
    {
        _replaceWaveforms += otherWaveforms;
        _replaceWaveformTitles += otherWaveformTitles;
    }

    // 排除已经显示的波形。
    int pos = -1;
    for (int i = 0; i < displayedWaveform.size(); i++)
    {
        if ((pos = _replaceWaveforms.indexOf(displayedWaveform[i])) != -1)
        {
            if (_replaceWaveforms[pos] != _waveformName)  // 不移除自己。
            {
                _replaceWaveforms.removeAt(pos);
                _replaceWaveformTitles.removeAt(pos);
            }
        }

        if ((pos = _insertWaveforms.indexOf(displayedWaveform[i])) != -1)
        {
            _insertWaveforms.removeAt(pos);
            _insertWaveformTitles.removeAt(pos);
        }
    }


    // 将波形名称添加到下拉框。
    _replaceList->clear();
    _replaceList->addItems(_replaceWaveformTitles);
    _insertList->clear();
    _insertList->addItems(_insertWaveformTitles);

    // 添加一项空值，以便放弃插入波形
    _insertList->addItem(QString());


    // 定位到当前项。
    for (int i = 0; i < _replaceWaveforms.size(); i++)
    {
        if (_replaceWaveforms[i] == _waveformName)
        {
            _replaceList->setCurrentIndex(i);
            break;
        }
    }

    // 风格统一，默认没有当前选择项
    _insertList->setCurrentIndex(-1);

    _insertList->setEnabled(true);
}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void WaveWidgetSelectMenu::showEvent(QShowEvent *event)
{
    if (_isTopWaveform)
    {
        _removeButton->setEnabled(false);
    }
    else
    {
        _removeButton->setEnabled(true);
    }

    _loadWaveforms();
    PopupWidget::showEvent(event);
    setCloseBtnFocus();

    QRect r = rect();
    QRect wr = windowManager.geometry();
    if (_y + r.height() > wr.y() + wr.height())
    {
        _y = wr.y() + wr.height() - r.height();
    }

    move(_x, _y);
}

/**************************************************************************************************
 * 按键事件。
 *************************************************************************************************/
void WaveWidgetSelectMenu::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusNextPrevChild(false);
            break;

        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            break;

        default:
            break;
    }

    PopupWidget::keyPressEvent(event);
}

/**************************************************************************************************
 * 设置是否为操作Top Waveform。
 *************************************************************************************************/
void WaveWidgetSelectMenu::setTopWaveform(bool isTopWaveform)
{
    _isTopWaveform = isTopWaveform;
}

/**************************************************************************************************
 * 设置被操作的波形控件。
 *************************************************************************************************/
void WaveWidgetSelectMenu::setWaveformName(const QString &name)
{
    _waveformName = name;
}

/**************************************************************************************************
 * 是否关联了TopWaveform。
 *************************************************************************************************/
bool WaveWidgetSelectMenu::isTopWaveform(void) const
{
    return _isTopWaveform;
}

/**************************************************************************************************
 * 重新刷新。
 *************************************************************************************************/
void WaveWidgetSelectMenu::refresh(void)
{
    if (!isVisible())
    {
        return;
    }

    _loadWaveforms();
}

/**************************************************************************************************
 * 更新MFC波形名称。
 *************************************************************************************************/
void WaveWidgetSelectMenu::updateMFCWaveName(const QString &name)
{
    if (!isVisible())
    {
        return;
    }

    if (_isTopWaveform)
    {
        _replaceList->setItemText(0, name);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
WaveWidgetSelectMenu::WaveWidgetSelectMenu() : PopupWidget()
{
    _isTopWaveform = false;
    setTitleBarText(trs("WaveformSources"));

    int fontSize = 15;
    setFixedWidth(300);

    _replaceList = new IComboList(trs("Replace"));
    _replaceList->setFont(fontManager.textFont(fontSize));
    _replaceList->setStretch(1, 2);
    connect(_replaceList, SIGNAL(currentIndexChanged(int)), this, SLOT(_replaceListSlot(int)));

    _insertList = new IComboList(trs("Insert"));
    _insertList->setFont(fontManager.textFont(fontSize));
    _insertList->setStretch(1, 2);
    connect(_insertList, SIGNAL(currentIndexChanged(int)), this, SLOT(_insertListSlot(int)));

    QHBoxLayout *hlayout = new QHBoxLayout();
    _removeButton = new IButton(trs("Remove"));
    _removeButton->setBorderEnabled(true);
    _removeButton->setFont(fontManager.textFont(fontSize));
    connect(_removeButton, SIGNAL(realReleased()), this, SLOT(_removeButtonSlot()));
    hlayout->addStretch(100);
    hlayout->addWidget(_removeButton, 190);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 1, 2, 1);
    layout->setSpacing(2);
    layout->addWidget(_replaceList);
    layout->addWidget(_insertList);
    layout->addLayout(hlayout);

    contentLayout->addLayout(layout);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
WaveWidgetSelectMenu::~WaveWidgetSelectMenu()
{

}
