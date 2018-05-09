#include "IBPManualRuler.h"
#include "LabelButton.h"
#include "IButton.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "NumberInput.h"
#include "IMessageBox.h"

IBPManualRuler *IBPManualRuler::_selfObj = NULL;

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void IBPManualRuler::layoutExec()
{
    int  submenuW = windowManager.getPopMenuWidth();

    setTitleBarText(trs("ManualRulerSet"));

    int itemW = submenuW - 20;

    int fontSize = fontManager.getFontSize(1);
    QFont font = fontManager.textFont(fontSize);
    int btnWidth = itemW / 8;
    int labelWidth = btnWidth;

    _highLimit = new LabelButton(trs("PressureHighRuler"));
    _highLimit->setFont(font);
    _highLimit->label->setFixedSize(labelWidth, _itemH);
    _highLimit->button->setFixedSize(btnWidth, _itemH);
    _highLimit->label->setAlignment(Qt::AlignCenter);
    connect(_highLimit->button, SIGNAL(realReleased()), this, SLOT(_highRulerReleased()));

    _lowLimit = new LabelButton(trs("PressureLowRuler"));
    _lowLimit->setFont(font);
    _lowLimit->label->setFixedSize(labelWidth, _itemH);
    _lowLimit->button->setFixedSize(btnWidth, _itemH);
    _lowLimit->label->setAlignment(Qt::AlignCenter);
    connect(_lowLimit->button, SIGNAL(realReleased()), this, SLOT(_lowRulerReleased()));

    _yes = new IButton(trs("EnglishYESChineseSURE"));
    _yes->setFixedSize(btnWidth, _itemH);
    _yes->setFont(font);
    _yes->setBorderEnabled(true);
    _yes->setAlignment(Qt::AlignCenter);
    connect(_yes, SIGNAL(realReleased()), this, SLOT(_yesReleased()));

    contentLayout->setSpacing(1);
    contentLayout->addWidget(_highLimit);
    contentLayout->addWidget(_lowLimit);
    contentLayout->addWidget(_yes, 0, Qt::AlignCenter);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
IBPManualRuler::IBPManualRuler()
{
    _waveWidget = NULL;
    layoutExec();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
IBPManualRuler::~IBPManualRuler()
{

}

/**************************************************************************************************
 * 设置手动标尺的波形窗口对象。
 *************************************************************************************************/
void IBPManualRuler::setWaveWidget(IBPWaveWidget *widget)
{
    _waveWidget = widget;
}

/**************************************************************************************************
 * 设置上下标尺值。
 *************************************************************************************************/
void IBPManualRuler::setHighLowRuler(int high, int low)
{
    _high = high;
    _low = low;
    _highLimit->setValue(QString::number(_high));
    _lowLimit->setValue(QString::number(_low));
}

/**************************************************************************************************
 * 压力上标尺槽函数。
 *************************************************************************************************/
void IBPManualRuler::_highRulerReleased()
{
    NumberInput numberPad;
    numberPad.setTitleBarText(trs("PressureHighRuler"));
    numberPad.setMaxInputLength(3);
    numberPad.setInitString(_highLimit->button->text());
    if (numberPad.exec())
    {
        QString text = numberPad.getStrValue();
        bool ok = false;
        int value = text.toInt(&ok);
        if (ok)
        {
            if (value > 360)
            {
                IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "-50-360", QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
            else if (value <= _low)
            {
                IMessageBox messageBox(trs("Prompt"), trs("LessThan") + trs("PressureLowRuler"), QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
            else
            {
                _highLimit->button->setText(text);
                _high = value;
            }
        }
    }
}

/**************************************************************************************************
 * 压力下标尺槽函数。
 *************************************************************************************************/
void IBPManualRuler::_lowRulerReleased()
{
    NumberInput numberPad;
    numberPad.setTitleBarText(trs("PressureLowRuler"));
    numberPad.setMaxInputLength(3);
    numberPad.setInitString(_lowLimit->button->text());
    if (numberPad.exec())
    {
        QString text = numberPad.getStrValue();
        bool ok = false;
        int value = text.toInt(&ok);
        if (ok)
        {
            if (value < -50)
            {
                IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "-50-360", QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
            else if (value >= _high)
            {
                IMessageBox messageBox(trs("Prompt"), trs("GreaterThan") + trs("PressureLowRuler"), QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
            else
            {
                _lowLimit->button->setText(text);
                _low = value;
            }
        }
    }
}

/**************************************************************************************************
 * 确定槽函数。
 *************************************************************************************************/
void IBPManualRuler::_yesReleased()
{
    _waveWidget->setLimit(_low, _high);
    _waveWidget->setRuler(IBP_MANUAL_SCALE_INDEX);
    done(1);
}
