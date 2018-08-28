/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/27
 **/

#include "WaveWidgetSelectMenu.h"
#include "ParamManager.h"
#include "WindowManager.h"
#include "ECGParam.h"
#include "LanguageManager.h"
#include "ComboBox.h"
#include <QLabel>
#include "Button.h"
#include "SystemManager.h"
#include "Debug.h"
#include <QTimer>
#include <QGridLayout>

class WaveWidgetSelectMenuPrivate
{
public:
    WaveWidgetSelectMenuPrivate()
        : replaceList(NULL),
          insertList(NULL),
          removeButton(NULL),
          isTopWaveform(NULL),
          waveformName(""),
          x(0),
          y(0)

    {
        replaceWaveforms.clear();
        replaceWaveformTitles.clear();
        insertWaveforms.clear();
        insertWaveformTitles.clear();
    }
    void loadWaveforms(void);

    ComboBox  *replaceList;
    ComboBox  *insertList;
    Button    *removeButton;

    bool       isTopWaveform;
    QString    waveformName;

    // 保存替换选项和插入选项中的波形信息。
    QStringList  replaceWaveforms;
    QStringList  replaceWaveformTitles;
    QStringList  insertWaveforms;
    QStringList  insertWaveformTitles;

    int  x;
    int  y;
};

/**************************************************************************************************
 * 载入波形。
 *************************************************************************************************/
void WaveWidgetSelectMenu::loadWaveformsSlot(void)
{
    d_ptr->loadWaveforms();
}

/**************************************************************************************************
 * 替换波形。
 *************************************************************************************************/
void WaveWidgetSelectMenu::replaceListSlot(int index)
{
    if (d_ptr->isTopWaveform)
    {
        ecgParam.setCalcLead(d_ptr->replaceWaveforms[index]);
    }

    ecgParam.setLeadMode3DisplayLead(d_ptr->replaceWaveforms[index]);

    if (windowManager.getUFaceType() == UFACE_MONITOR_BIGFONT)
    {
        windowManager.replacebigWaveform(d_ptr->waveformName, d_ptr->replaceWaveforms[index]);
    }
    else
    {
        windowManager.replaceWaveform(d_ptr->waveformName, d_ptr->replaceWaveforms[index]);
    }

    d_ptr->waveformName = d_ptr->replaceWaveforms[index];
    QTimer::singleShot(0, this, SLOT(loadWaveformsSlot()));
}

/**************************************************************************************************
 * 插入新波形。
 *************************************************************************************************/
void WaveWidgetSelectMenu::insertListSlot(int index)
{
    if (d_ptr->insertList->itemText(index).isEmpty())
    {
        d_ptr->insertList->setCurrentIndex(-1);
        return;
    }

    ecgParam.setLeadMode3DisplayLead(d_ptr->insertWaveforms[index]);
    windowManager.insertWaveform(d_ptr->waveformName, d_ptr->insertWaveforms[index]);
//    _loadWaveforms();
    hide();
}

/**************************************************************************************************
 * 移除。
 *************************************************************************************************/
void WaveWidgetSelectMenu::removeButtonSlot(void)
{
    windowManager.removeWaveform(d_ptr->waveformName);
    hide();
}

/**************************************************************************************************
 * 载入波形控件的名称。
 *************************************************************************************************/
void WaveWidgetSelectMenuPrivate::loadWaveforms(void)
{
    QStringList displayedWaveform;
    windowManager.getDisplayedWaveform(displayedWaveform);

    replaceWaveforms.clear();
    replaceWaveformTitles.clear();
    insertWaveforms.clear();
    insertWaveformTitles.clear();
    replaceList->blockSignals(true);
    insertList->blockSignals(true);
    // 获取ECG当前支持的波形窗体。
    ecgParam.getAvailableWaveforms(replaceWaveforms, replaceWaveformTitles, isTopWaveform);
    insertWaveforms = replaceWaveforms;
    insertWaveformTitles = replaceWaveformTitles;

    if (isTopWaveform)
    {
        // 如果是Top Waveform则从insert菜单中剔除PADS;
        insertWaveforms.removeFirst();
        insertWaveformTitles.removeFirst();
    }


    if (ecgParam.getLeadMode() == ECG_LEAD_MODE_3)
    {
        QStringList ecgLead3WaveName;
        ecgLead3WaveName.clear();
        ecgLead3WaveName << ecgParam.getWaveWidgetName(ECG_LEAD_I);
        ecgLead3WaveName << ecgParam.getWaveWidgetName(ECG_LEAD_II);
        ecgLead3WaveName << ecgParam.getWaveWidgetName(ECG_LEAD_III);
        int index = -1;
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
            for (int i = 0; i <= 2; ++i)
            {
                int pos = insertWaveforms.indexOf(ecgLead3WaveName.at(i));
                if (-1 != pos)
                {
                    insertWaveforms.removeAt(pos);
                    insertWaveformTitles.removeAt(pos);
                }

                if (-1 == ecgLead3WaveName.indexOf(waveformName))
                {
                    pos = replaceWaveforms.indexOf(ecgLead3WaveName.at(i));
                    if (-1 != pos)
                    {
                        replaceWaveforms.removeAt(pos);
                        replaceWaveformTitles.removeAt(pos);
                    }
                }
            }
        }
    }

    // 其他参数支持的波形窗体。
    QList<Param *> params;
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
    insertWaveforms += otherWaveforms;
    insertWaveformTitles += otherWaveformTitles;
    if (!isTopWaveform)  // 只有非Top Waveform才能替换成非ECG窗体。
    {
        replaceWaveforms += otherWaveforms;
        replaceWaveformTitles += otherWaveformTitles;
    }

    // 排除已经显示的波形。
    for (int i = 0; i < displayedWaveform.size(); i++)
    {
        int pos = -1;
        if ((pos = replaceWaveforms.indexOf(displayedWaveform[i])) != -1)
        {
            if (replaceWaveforms[pos] != waveformName)  // 不移除自己。
            {
                replaceWaveforms.removeAt(pos);
                replaceWaveformTitles.removeAt(pos);
            }
        }

        if ((pos = insertWaveforms.indexOf(displayedWaveform[i])) != -1)
        {
            insertWaveforms.removeAt(pos);
            insertWaveformTitles.removeAt(pos);
        }
    }


    // 将波形名称添加到下拉框。
    replaceList->clear();
    replaceList->addItems(replaceWaveformTitles);
    insertList->clear();
    insertList->addItems(insertWaveformTitles);

    // 添加一项空值，以便放弃插入波形
    insertList->addItem(QString());


    // 定位到当前项。
    for (int i = 0; i < replaceWaveforms.size(); i++)
    {
        if (replaceWaveforms[i] == waveformName)
        {
            replaceList->setCurrentIndex(i);
            break;
        }
    }

    // 风格统一，默认没有当前选择项

    insertList->setCurrentIndex(-1);
    insertList->blockSignals(false);

    insertList->setEnabled(true);

    replaceList->blockSignals(false);
}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void WaveWidgetSelectMenu::showEvent(QShowEvent *event)
{
    if (d_ptr->isTopWaveform)
    {
        d_ptr->removeButton->setEnabled(false);
    }
    else
    {
        d_ptr->removeButton->setEnabled(true);
    }

    d_ptr->loadWaveforms();
    Window::showEvent(event);

    QRect r = rect();
    QRect wr = windowManager.geometry();
    if (d_ptr->y + r.height() > wr.y() + wr.height())
    {
        d_ptr->y = wr.y() + wr.height() - r.height();
    }

    move(d_ptr->x, d_ptr->y);
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

    Window::keyPressEvent(event);
}

/**************************************************************************************************
 * 设置是否为操作Top Waveform。
 *************************************************************************************************/
void WaveWidgetSelectMenu::setTopWaveform(bool isTopWaveform)
{
    d_ptr->isTopWaveform = isTopWaveform;
}

/**************************************************************************************************
 * 设置被操作的波形控件。
 *************************************************************************************************/
void WaveWidgetSelectMenu::setWaveformName(const QString &name)
{
    d_ptr->waveformName = name;
}

/**************************************************************************************************
 * 是否关联了TopWaveform。
 *************************************************************************************************/
bool WaveWidgetSelectMenu::isTopWaveform(void) const
{
    return d_ptr->isTopWaveform;
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

    d_ptr->loadWaveforms();
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

    if (d_ptr->isTopWaveform)
    {
        d_ptr->replaceList->setItemText(0, name);
    }
}

void WaveWidgetSelectMenu::setShowPoint(int x, int y)
{
    d_ptr->x = x;
    d_ptr->y = y;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
WaveWidgetSelectMenu::WaveWidgetSelectMenu()
    : Window(),
      d_ptr(new WaveWidgetSelectMenuPrivate)
{
    d_ptr->isTopWaveform = false;
    setWindowTitle(trs("WaveformSources"));

//    int fontSize = 15;
    setFixedWidth(300);

    QLabel *label;
    ComboBox *combo;
    Button *button;
    int layoutIndex = 0;
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    label = new QLabel(trs("Replace"));
    layout->addWidget(label, layoutIndex, 0);
    combo = new ComboBox;
    layout->addWidget(combo, layoutIndex, 1);
    connect(combo, SIGNAL(currentIndexChanged(int)), this,
            SLOT(replaceListSlot(int)));
    layoutIndex++;
    d_ptr->replaceList = combo;

    label = new QLabel(trs("Insert"));
    layout->addWidget(label, layoutIndex, 0);
    combo = new ComboBox;
    layout->addWidget(combo, layoutIndex, 1);
    connect(combo, SIGNAL(currentIndexChanged(int)), this,
            SLOT(insertListSlot(int)));
    layoutIndex++;
    d_ptr->insertList = combo;

    button = new Button(trs("Remove"));
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, layoutIndex, 1);
    connect(button, SIGNAL(realReleased()), this,
            SLOT(removeButtonSlot()));
    d_ptr->removeButton = button;

    setWindowLayout(layout);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
WaveWidgetSelectMenu &WaveWidgetSelectMenu::getInstance()
{
    static WaveWidgetSelectMenu *instance = NULL;
    if (!instance)
    {
        instance = new WaveWidgetSelectMenu;
    }
    return *instance;
}

WaveWidgetSelectMenu::~WaveWidgetSelectMenu()
{
    delete d_ptr;
}

