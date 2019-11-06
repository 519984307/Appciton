/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/24
 **/
#include "ConfigEditTEMPMenuContent.h"
#include "ComboBox.h"
#include <QLabel>
#include <QGridLayout>
#include "LanguageManager.h"
#include "TEMPSymbol.h"
#include "Framework/Config/Config.h"
#include "ConfigEditMenuWindow.h"
#include "Button.h"
#include "ParamInfo.h"
#include "ParamDefine.h"
#include "ConfigManager.h"

class ConfigEditTEMPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ONE,
        ITEM_CBO_TWO,
        ITEM_CBO_ENABLE,
    };

    explicit ConfigEditTEMPMenuContentPrivate(Config *const config)
        : tempChannelOne(NULL),
          tempChannelTwo(NULL),
          config(config)
    {
    }
    /**
     * @brief loadOption
     */
    void loadOption(void);

    ComboBox *tempChannelOne;
    ComboBox *tempChannelTwo;
    Config *const config;
};



ConfigEditTEMPMenuContent::ConfigEditTEMPMenuContent(Config *const config)
    : MenuContent(trs("ConfigEditTEMPMenu"),
                  trs("ConfigEditTEMPMenuDesc")),
      d_ptr(new ConfigEditTEMPMenuContentPrivate(config))
{
}

ConfigEditTEMPMenuContent::~ConfigEditTEMPMenuContent()
{
    delete d_ptr;
}

void ConfigEditTEMPMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

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
}

void ConfigEditTEMPMenuContent::readyShow()
{
    d_ptr->loadOption();
}

void ConfigEditTEMPMenuContent::onComboIndexUpdated(int index)
{
    Q_UNUSED(index)
    ComboBox *combo = qobject_cast<ComboBox *>(sender());

    if (!combo)
    {
        return;
    }

    int item = combo->property("Item").toInt();
    QString strPath;
    switch (item)
    {
    case ConfigEditTEMPMenuContentPrivate::ITEM_CBO_ONE:
        strPath = "TEMP|ChannelOne";
        break;
    case ConfigEditTEMPMenuContentPrivate::ITEM_CBO_TWO:
        strPath = "TEMP|ChannelTwo";
        break;
    }
    d_ptr->config->setNumValue(strPath, index);
}

void ConfigEditTEMPMenuContent::onAlarmBtnReleased()
{
    MenuWindow *w = this->getMenuWindow();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_T1, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}

void ConfigEditTEMPMenuContentPrivate::loadOption()
{
    int index;

    index = 0;
    config->getNumValue("TEMP|ChannelOne", index);
    tempChannelOne->setCurrentIndex(index);

    index = 0;
    config->getNumValue("TEMP|ChannelTwo", index);
    tempChannelTwo->setCurrentIndex(index);
}
