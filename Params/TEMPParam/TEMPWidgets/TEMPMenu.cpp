/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/24
 **/
#include "TEMPMenu.h"
#include "ComboBox.h"
#include <QLabel>
#include <QGridLayout>
#include "LanguageManager.h"
#include "TEMPSymbol.h"
#include "TEMPParam.h"
#include "MainMenuWindow.h"
#include "Button.h"

class TEMPMenuPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ONE,
        ITEM_CBO_TWO,
        ITEM_CBO_ENABLE,
    };

    TEMPMenuPrivate()
        : tempChannelOne(NULL),
          tempChannelTwo(NULL),
          tempChannelDisable(NULL)
    {
    }
    /**
     * @brief loadOption
     */
    void loadOption(void);

    ComboBox *tempChannelOne;
    ComboBox *tempChannelTwo;
    ComboBox *tempChannelDisable;
};



TEMPMenu::TEMPMenu()
    : MenuContent(trs("TEMPMenu"),
                  trs("TEMPMenuDesc")),
      d_ptr(new TEMPMenuPrivate)
{
}

TEMPMenu::~TEMPMenu()
{
    delete d_ptr;
}

void TEMPMenu::layoutExec()
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


    // module diable
    label = new QLabel(trs("TEMPChannelDisable"));
    glayout->addWidget(label, layoutIndex, 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("No")
                    << trs("Yes")
                   );
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexUpdated(int)));
    combo->setProperty("Item", qVariantFromValue(comboIndex));
    comboIndex++;
    glayout->addWidget(combo, layoutIndex, 1);
    d_ptr->tempChannelDisable = combo;
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

void TEMPMenu::readyShow()
{
    d_ptr->loadOption();
}

void TEMPMenu::onComboIndexUpdated(int index)
{
    Q_UNUSED(index)
    ComboBox *combo = qobject_cast<ComboBox *>(sender());

    if (!combo)
    {
        return;
    }

    int item = combo->property("item").toInt();
    switch (item)
    {
    case TEMPMenuPrivate::ITEM_CBO_ONE:
        break;
    case TEMPMenuPrivate::ITEM_CBO_TWO:
        break;
    case TEMPMenuPrivate::ITEM_CBO_ENABLE:
        if (index)
        {
            tempParam.setErrorDisable();
        }
        else
        {
            tempParam.setModuleEnable();
        }
        break;
    }
}

void TEMPMenu::onAlarmBtnReleased()
{
    MainMenuWindow *w = MainMenuWindow::getInstance();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_T1, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}

void TEMPMenuPrivate::loadOption()
{
    if (!tempParam.getErrorDisable())
    {
        tempChannelDisable->setCurrentIndex(0);
    }
    else
    {
        tempChannelDisable->setCurrentIndex(1);
    }
}
