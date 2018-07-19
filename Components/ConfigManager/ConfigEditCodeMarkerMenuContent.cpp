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
#include "ConfigEditMenuWindow.h"
#include <QEvent>

class ConfigEditCodeMarkerMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_CODE_MARKER1 = 0,
        ITEM_CBO_CODE_MARKER2,
        ITEM_CBO_CODE_MARKER3,
        ITEM_CBO_CODE_MARKER4,
        ITEM_CBO_CODE_MARKER5,
        ITEM_CBO_CODE_MARKER6,
        ITEM_CBO_CODE_MARKER7,
        ITEM_CBO_CODE_MARKER8,
        ITEM_CBO_CODE_MARKER9,
        ITEM_CBO_CODE_MARKER10,
        ITEM_CBO_CODE_MARKER11,
        ITEM_CBO_CODE_MARKER12,
        ITEM_CBO_CODE_MARKER13,
        ITEM_CBO_CODE_MARKER14,
        ITEM_CBO_CODE_MARKER15,
        ITEM_CBO_CODE_MARKER16,
        ITEM_CBO_CODE_MARKER17,
        ITEM_CBO_CODE_MARKER18,
        ITEM_CBO_CODE_MARKER19,
        ITEM_CBO_CODE_MARKER20,
        ITEM_CBO_CODE_MARKER21,
        ITEM_CBO_CODE_MARKER22,
        ITEM_CBO_CODE_MARKER23,
        ITEM_CBO_CODE_MARKER24,
        ITEM_CBO_CODE_MARKER25,
        ITEM_CBO_CODE_MARKER26,
        ITEM_CBO_CODE_MARKER27,
        ITEM_CBO_CODE_MARKER28,
        ITEM_CBO_MAX,
    };

    void loadOptions();

    QMap <MenuItem, ComboBox *> combos;
    int languageIndex;
    QStringList allCodeMarkers;  // all codemarker types
    QStringList selectedCodeMarkers;  // current selected codemarker types
};

ConfigEditCodeMarkerMenuContent::ConfigEditCodeMarkerMenuContent():
    MenuContent(trs("ConfigEditCodeMarkerMenu"),
                trs("ConfigEditCodeMarkerMenuDesc")),
    d_ptr(new ConfigEditCodeMarkerMenuContentPrivate)
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
    Config *config = ConfigEditMenuWindow
                     ::getInstance()->getCurrentEditConfig();
    config->getNumAttr("Local|Language", "CurrentOption", languageIndex);
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
    d_ptr->loadOptions();
}

void ConfigEditCodeMarkerMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    ComboBox *comboBox;
    ConfigEditCodeMarkerMenuContentPrivate::MenuItem itemId;

    for (int i = 0; i < ConfigEditCodeMarkerMenuContentPrivate::ITEM_CBO_MAX / 2; i++)
    {
        label = new QLabel(trs(QString::number(i + 1)));
        layout->addWidget(label, i, 0);
        comboBox = new ComboBox;
        layout->addWidget(comboBox, i, 1);
        itemId = static_cast<ConfigEditCodeMarkerMenuContentPrivate::MenuItem>(i);
        d_ptr->combos.insert(itemId, comboBox);
        comboBox->setProperty("Item", qVariantFromValue(i));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    }
    int itemHalfTemp = ConfigEditCodeMarkerMenuContentPrivate::ITEM_CBO_MAX / 2;
    for (int i = itemHalfTemp; i < itemHalfTemp * 2; i++)
    {
        label = new QLabel(trs(QString::number(i + 1)));
        layout->addWidget(label, (i - itemHalfTemp), 2);
        comboBox = new ComboBox;
        layout->addWidget(comboBox, (i - itemHalfTemp), 3);
        itemId = static_cast<ConfigEditCodeMarkerMenuContentPrivate::MenuItem>(i);
        d_ptr->combos.insert(itemId, comboBox);
        comboBox->setProperty("Item", qVariantFromValue(i));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    }
    int itemMax = ConfigEditCodeMarkerMenuContentPrivate::ITEM_CBO_MAX;
    layout->setRowStretch(itemMax, 1);
}
void ConfigEditCodeMarkerMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    if (!combo)
    {
        return;
    }
    Config *config = ConfigEditMenuWindow
                     ::getInstance()->getCurrentEditConfig();
    int itemType = combo->property("Item").toInt();
    // new code marker
    d_ptr->selectedCodeMarkers[itemType] = d_ptr->allCodeMarkers[index];
    QStringList tmpList(d_ptr->selectedCodeMarkers);
    QString strValue = tmpList.join(",");
    int num = 0;
    config->getNumAttr("Local|Language", "CurrentOption", num);
    QString markerStr = "CodeMarker|SelectMarker|Language";
    markerStr += QString::number(num, 10);
    config->setStrValue(markerStr, strValue);
}
