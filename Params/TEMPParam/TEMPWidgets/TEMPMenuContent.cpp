/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/24
 **/
#include "TEMPMenuContent.h"
#include <QLabel>
#include <QGridLayout>
#include "Framework/UI/Button.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include "TEMPSymbol.h"
#include "TEMPParam.h"
#include "MainMenuWindow.h"
#include "AlarmLimitWindow.h"
#include "WindowManager.h"

class TEMPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ONE,
        ITEM_CBO_TWO,
    };

    TEMPMenuContentPrivate()
        : tempChannelOne(NULL),
          tempChannelTwo(NULL)
    {
    }

    ComboBox *tempChannelOne;
    ComboBox *tempChannelTwo;
};



TEMPMenuContent::TEMPMenuContent()
    : MenuContent(trs("TEMPMenuContent"),
                  trs("TEMPMenuContentDesc")),
      d_ptr(new TEMPMenuContentPrivate)
{
}

TEMPMenuContent::~TEMPMenuContent()
{
    delete d_ptr;
}

void TEMPMenuContent::layoutExec()
{
    QGridLayout *glayout = new QGridLayout(this);
    glayout->setMargin(10);

    QLabel *label;
    ComboBox *combo;
    int comboIndex = 0;
    int layoutIndex = 0;

    // channel 1
    label = new QLabel(trs("TEMPChannelOne"));
    glayout->addWidget(label, layoutIndex, 0);
    combo = new ComboBox;
    // 暂时失能通道类型选择
    combo->setEnabled(false);
    for (int i = 0; i < TEMP_CHANNEL_NR; i++)
    {
        combo->addItem(trs(TEMPSymbol::convert(TEMPChannelType(i))));
    }
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexUpdated(int)));
    combo->setProperty("Item", qVariantFromValue(comboIndex));
    comboIndex++;
    glayout->addWidget(combo, layoutIndex, 1);
    d_ptr->tempChannelOne = combo;
    layoutIndex++;

    // channel 2
    label = new QLabel(trs("TEMPChannelTwo"));
    glayout->addWidget(label, layoutIndex, 0);
    combo = new ComboBox;
    // 暂时失能通道类型选择
    combo->setEnabled(false);
    for (int i = 0; i < TEMP_CHANNEL_NR; i++)
    {
        QString str = TEMPSymbol::convert(TEMPChannelType(i));
        if (i == 0)
        {
            str.replace("1", "2");
        }
        combo->addItem(trs(str));
    }
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexUpdated(int)));
    combo->setProperty("Item", qVariantFromValue(comboIndex));
    comboIndex++;
    glayout->addWidget(combo, layoutIndex, 1);
    d_ptr->tempChannelTwo = combo;
    layoutIndex++;

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    glayout->addWidget(btn, layoutIndex, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));
    layoutIndex++;

    glayout->setRowStretch(layoutIndex , 1);

    // 添加更新温度通道名称信号链接
//    connect(this, SIGNAL(updateTempName(TEMPChannelIndex,TEMPChannelType)),
//            &tempParam, SIGNAL(updateTempName(TEMPChannelIndex,TEMPChannelType)));
}

void TEMPMenuContent::onComboIndexUpdated(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());

    if (!combo)
    {
        return;
    }

    int item = combo->property("Item").toInt();
    switch (item)
    {
    case TEMPMenuContentPrivate::ITEM_CBO_ONE:
        emit updateTempName(TEMP_CHANNEL_ONE,
                                static_cast<TEMPChannelType>(index));
        break;
    case TEMPMenuContentPrivate::ITEM_CBO_TWO:
        emit updateTempName(TEMP_CHANNEL_TWO,
                                static_cast<TEMPChannelType>(index));
        break;
    }
}

void TEMPMenuContent::onAlarmBtnReleased()
{
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_T1, true);
    AlarmLimitWindow w(subParamName);
    windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
}
