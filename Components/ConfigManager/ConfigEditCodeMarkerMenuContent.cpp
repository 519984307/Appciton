/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/19
 **/
#include "ConfigEditCodeMarkerMenuContent.h"
#include "ComboBox.h"
#include <QLabel>
#include <QGridLayout>
#include "LanguageManager.h"
#include <QMap>
#include <QVBoxLayout>
#include <QEvent>
#include "ConfigManager.h"
#include "IConfig.h"

class ConfigEditCodeMarkerMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_CODE_MARKER1 = 0,
        ITEM_CBO_MAX = 27,
    };

    explicit ConfigEditCodeMarkerMenuContentPrivate(Config *const config)
        : languageIndex(-1),
          config(config)
        , codeMarkerFirst(NULL)
        , codeMarkerLast(NULL)
    {
    }
    void loadOptions();

    QMap <MenuItem, ComboBox *> combos;
    int languageIndex;
    QStringList allCodeMarkers;  // all codemarker types
    QStringList selectedCodeMarkers;  // current selected codemarker types
    Config *const config;

    QLabel * codeMarkerFirst;  // 仅查看该菜单时,设置该label为可聚焦方式，便于旋转飞梭查看视图
    QLabel * codeMarkerLast;  // 仅查看该菜单时,设置该label为可聚焦方式，便于旋转飞梭查看视图
};

ConfigEditCodeMarkerMenuContent::ConfigEditCodeMarkerMenuContent(Config *const config):
    MenuContent(trs("ConfigEditCodeMarkerMenu"),
                trs("ConfigEditCodeMarkerMenuDesc")),
    d_ptr(new ConfigEditCodeMarkerMenuContentPrivate(config))
{
}

ConfigEditCodeMarkerMenuContent::~ConfigEditCodeMarkerMenuContent()
{
    delete d_ptr;
}

void ConfigEditCodeMarkerMenuContent::hideEvent(QHideEvent *ev)
{
    MenuContent::hideEvent(ev);

    if (d_ptr->languageIndex != languageManager.getCurLanguage())
    {
        languageManager.reload(languageManager.getCurLanguage());
    }
}

void ConfigEditCodeMarkerMenuContentPrivate::loadOptions()
{
    languageIndex = 0;
    systemConfig.getNumAttr("General|Language", "CurrentOption", languageIndex);
    if (languageIndex != languageManager.getCurLanguage())
    {
        languageManager.reload(languageIndex);
    }

    QString codemarkerStr;
    QString indexStr = "CodeMarker|Marker";
    indexStr += QString::number(languageIndex, 10);

    config->getStrValue(indexStr, codemarkerStr);
    allCodeMarkers = codemarkerStr.split(',');

    codemarkerStr.clear();
    QString markerStr = "CodeMarker|SelectMarker|Language";
    markerStr += QString::number(languageIndex, 10);
    config->getStrValue(markerStr, codemarkerStr);
    selectedCodeMarkers = codemarkerStr.split(',');

    int totalSize = allCodeMarkers.size();
    int selectSize = selectedCodeMarkers.size();
    for (int i = 0; i < ITEM_CBO_MAX; ++i)
    {
        MenuItem item = static_cast<MenuItem>(i);
        combos[item]->blockSignals(true);
        combos[item]->clear();
        for (int j = 0; j < totalSize; ++j)
        {
            combos[item]->addItem(trs(allCodeMarkers.at(j)));
        }

        if (i < selectSize)
        {
            int len = combos[item]->count();
            for (int k = 0; k < len; k++)
            {
                if (combos[item]->itemText(k) == trs(selectedCodeMarkers[i]))
                {
                    combos[item]->setCurrentIndex(k);
                    break;
                }
            }
        }
        else
        {
            int len = combos[item]->count();
            for (int k = 0; k < len; k++)
            {
                if (combos[item]->itemText(k) == "")
                {
                    combos[item]->setCurrentIndex(k);
                    break;
                }
            }
        }
        combos[item]->blockSignals(false);
    }
}

void ConfigEditCodeMarkerMenuContent::readyShow()
{
    bool isOnlyToRead = configManager.isReadOnly();
    for (int i = 0; i < ConfigEditCodeMarkerMenuContentPrivate::
         ITEM_CBO_MAX; i++)
    {
        d_ptr->combos[ConfigEditCodeMarkerMenuContentPrivate
                ::MenuItem(i)]->setEnabled(!isOnlyToRead);
    }

    if (isOnlyToRead)
    {
        d_ptr->codeMarkerFirst->setFocusPolicy(Qt::StrongFocus);
        d_ptr->codeMarkerLast->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        d_ptr->codeMarkerFirst->setFocusPolicy(Qt::NoFocus);
        d_ptr->codeMarkerLast->setFocusPolicy(Qt::NoFocus);
    }
}

void ConfigEditCodeMarkerMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    ComboBox *comboBox;
    ConfigEditCodeMarkerMenuContentPrivate::MenuItem itemId;

    for (int i = 0; i < ConfigEditCodeMarkerMenuContentPrivate::ITEM_CBO_MAX / 2 + 1; i++)
    {
        label = new QLabel(trs(QString::number(i + 1)));
        if (i == 0)
        {
            d_ptr->codeMarkerFirst = label;
        }
        layout->addWidget(label, i, 0, Qt::AlignRight);
        comboBox = new ComboBox;
        layout->addWidget(comboBox, i, 1);
        itemId = static_cast<ConfigEditCodeMarkerMenuContentPrivate::MenuItem>(i);
        d_ptr->combos.insert(itemId, comboBox);
        comboBox->setProperty("Item", qVariantFromValue(i));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    }
    int itemHalfTemp = ConfigEditCodeMarkerMenuContentPrivate::ITEM_CBO_MAX / 2 + 1;
    for (int i = itemHalfTemp; i < itemHalfTemp * 2 - 1; i++)
    {
        label = new QLabel(trs(QString::number(i + 1)));
        if (i == itemHalfTemp * 2 - 2)
        {
            d_ptr->codeMarkerLast = label;
        }
        layout->addWidget(label, (i - itemHalfTemp), 2, Qt::AlignRight);
        comboBox = new ComboBox;
        layout->addWidget(comboBox, (i - itemHalfTemp), 3);
        itemId = static_cast<ConfigEditCodeMarkerMenuContentPrivate::MenuItem>(i);
        d_ptr->combos.insert(itemId, comboBox);
        comboBox->setProperty("Item", qVariantFromValue(i));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    }
    int itemMax = ConfigEditCodeMarkerMenuContentPrivate::ITEM_CBO_MAX;
    layout->setRowStretch(itemMax, 1);

    d_ptr->loadOptions(); // 初始化的时候加载一遍CodeMarker
}
void ConfigEditCodeMarkerMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    if (!combo)
    {
        return;
    }
    int itemType = combo->property("Item").toInt();
    // new code marker
    d_ptr->selectedCodeMarkers[itemType] = d_ptr->allCodeMarkers[index];
    QString s;
    for (int i = 0; i < d_ptr->selectedCodeMarkers.count(); i++)
    {
        if (d_ptr->selectedCodeMarkers[i] == d_ptr->selectedCodeMarkers[itemType]
                && i != itemType)
        {
            for (int j = 0; j < d_ptr->allCodeMarkers.count(); j++)
            {
                 s = d_ptr->allCodeMarkers[j];
                if (!d_ptr->selectedCodeMarkers.contains(s))
                {
                    ConfigEditCodeMarkerMenuContentPrivate::MenuItem item
                            = static_cast<ConfigEditCodeMarkerMenuContentPrivate::MenuItem>(i);
                    d_ptr->combos[item]->setCurrentIndex(j);
                    d_ptr->selectedCodeMarkers[i] = d_ptr->allCodeMarkers[j];
                    break;
                }
            }
        }
    }
    QStringList tmpList(d_ptr->selectedCodeMarkers);
    QString strValue = tmpList.join(",");
    int num = 0;
    systemConfig.getNumAttr("General|Language", "CurrentOption", num);
    QString markerStr = "CodeMarker|SelectMarker|Language";
    markerStr += QString::number(num, 10);
    d_ptr->config->setStrValue(markerStr, strValue);
}
