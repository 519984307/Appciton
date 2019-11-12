/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/10/11
 **/



#include "ShortTrendWindow.h"
#include <QGroupBox>
#include <QLayout>
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include "Button.h"
#include "ShortTrendContainer.h"
#include <QStringList>
#include "ParamInfo.h"
#include "IBPParam.h"
#include "NIBPParam.h"
#include "AGParam.h"
#include "ECGDupParam.h"
#include "IBPTrendWidget.h"
#include "SystemManager.h"
#include "CO2Param.h"

#define PARAM_ROW 2      // 每行2组参数

class  ShortTrendWindowPrivate
{
public:
    explicit ShortTrendWindowPrivate(ShortTrendContainer *const trendContainer)
        : lengthCbo(NULL),
          shortTrendContainer(trendContainer),
          defaultBtn(NULL),
          okBtn(NULL),
          cancelBtn(NULL)
    {
    }
    ~ShortTrendWindowPrivate() {}
    QList<ComboBox *> comboList;
    ComboBox *lengthCbo;
    ShortTrendContainer *shortTrendContainer;
    QMap<SubParamID, QString> paraList;
    QList<SubParamID> defaultParaList;
    QList<SubParamID> latastParaList;
    Button *defaultBtn;
    Button *okBtn;
    Button *cancelBtn;

    /**
     * @brief loadOption 加载下拉框选项
     */
    void loadOption(void);

    /**
     * @brief loadParaList 加载参数列表
     */
    void loadParaList();

    /**
     * @brief setMinitrend 设置短趋势
     */
    void reloadMinitrend();

    /**
     * @brief getSubParamID
     * @param id
     * @return
     */
    QList<SubParamID> getSubParamID(SubParamID id);
};


ShortTrendWindow::ShortTrendWindow(ShortTrendContainer *const trendContainer)
    : Dialog(),
      d_ptr(new ShortTrendWindowPrivate(trendContainer))
{
    setFixedSize(800, 580);
    setWindowTitle(trs("MinitrendSetup"));
    QGroupBox *grpBox = new QGroupBox();
    QVBoxLayout *vLayout = new QVBoxLayout;
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setMargin(5);
    gridLayout->setSpacing(5);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(10);
    mainLayout->setSpacing(10);
    grpBox->setLayout(vLayout);
    grpBox->setTitle(trs("SelectParameters"));
    QLabel *lbl;
    QString lblStr = QString();

    int paramCount = d_ptr->shortTrendContainer->getTrendNum();
    // groupBox
    int itemCount = 0;
    for (int i = 0; i < paramCount; i++)
    {
        lblStr = QString("%1 %2").arg(trs("Parameter")).arg(QString::number(i + 1));
        lbl = new QLabel;
        lbl->setFixedWidth(150);
        lbl->setText(lblStr);
        gridLayout->addWidget(lbl, i / PARAM_ROW, itemCount % (PARAM_ROW * 2));
        itemCount++;
        ComboBox *cbo = new ComboBox;
        d_ptr->comboList.append(cbo);
        gridLayout->addWidget(cbo, i / PARAM_ROW, itemCount % (PARAM_ROW * 2));
        itemCount++;
    }
    vLayout->addLayout(gridLayout);
    QHBoxLayout *hLayout = new QHBoxLayout;
    d_ptr->defaultBtn = new Button;
    d_ptr->defaultBtn->setFixedWidth(300);
    d_ptr->defaultBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->defaultBtn->setText(trs("RestoreDefault"));
    connect(d_ptr->defaultBtn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    hLayout->addStretch();
    hLayout->addWidget(d_ptr->defaultBtn);
    hLayout->addStretch();
    vLayout->addLayout(hLayout);

    mainLayout->addWidget(grpBox);
    // length
    hLayout = new QHBoxLayout;
    lbl = new QLabel;
    lbl->setText(trs("MinitrendLength"));
    d_ptr->lengthCbo = new ComboBox;
    hLayout->addWidget(lbl);
    hLayout->addWidget(d_ptr->lengthCbo);

    mainLayout->addLayout(hLayout);
    mainLayout->addStretch();
    // button
    hLayout = new QHBoxLayout;
    d_ptr->okBtn = new Button;
    d_ptr->okBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->okBtn->setText(trs("Ok"));
    connect(d_ptr->okBtn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    hLayout->addWidget(d_ptr->okBtn);
    d_ptr->cancelBtn = new Button;
    d_ptr->cancelBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->cancelBtn->setText(trs("Cancel"));
    connect(d_ptr->cancelBtn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    hLayout->addWidget(d_ptr->cancelBtn);
    mainLayout->addLayout(hLayout);

    setWindowLayout(mainLayout);
    d_ptr->loadParaList();
}

ShortTrendWindow::~ShortTrendWindow()
{
    delete d_ptr;
}

void ShortTrendWindow::showEvent(QShowEvent *e)
{
    d_ptr->shortTrendContainer->getDefaultTrendList(d_ptr->defaultParaList);
    d_ptr->loadOption();
    Dialog::showEvent(e);
}

void ShortTrendWindow::onBtnReleased()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == d_ptr->defaultBtn)
    {
        for (int i = 0; i < d_ptr->comboList.count(); i++)
        {
            ComboBox *cbo = d_ptr->comboList.at(i);
            SubParamID para = d_ptr->defaultParaList.at(i);
            for (int j = 0; j < cbo->count(); j++)
            {
                SubParamID id = d_ptr->paraList.key(cbo->itemText(j));
                if (id == para)
                {
                    cbo->setCurrentIndex(j);
                    break;
                }
            }
        }
    }
    else if (btn == d_ptr->okBtn)
    {
        d_ptr->reloadMinitrend();
        close();
    }
    else if (btn == d_ptr->cancelBtn)
    {
        close();
    }
}

void ShortTrendWindowPrivate::loadOption()
{
    shortTrendContainer->getShortTrendList(latastParaList);
    // 加载参数
    for (int i = 0; i < comboList.count(); i++)
    {
        ComboBox *cbo = comboList.at(i);
        // 加载列表选项
        cbo->clear();
        QMap<SubParamID, QString>::ConstIterator iter = paraList.constBegin();
        for (; iter != paraList.constEnd(); iter++)
        {
            if (!(*iter).isEmpty())
            {
                cbo->addItem(*iter);
            }
        }
        // 设置最新值
        for (int j = 0; j < cbo->count(); j++)
        {
            SubParamID index = paraList.key(cbo->itemText(j));
            if (latastParaList.at(i) == index)
            {
                cbo->setCurrentIndex(j);
            }
        }
    }
    // 加载时间
    lengthCbo->clear();
    for (int i = SHORT_TREND_DURATION_30M; i <= SHORT_TREND_DURATION_480M; i++)
    {
        ShortTrendDuration index = static_cast<ShortTrendDuration>(i);
        lengthCbo->addItem(convert(index));
    }
    int index = static_cast<int>(shortTrendContainer->getTrendDuration());
    lengthCbo->setCurrentIndex(index);
}


void ShortTrendWindowPrivate::loadParaList()
{
    paraList[SUB_PARAM_HR_PR] = trs(paramInfo.getSubParamName(SUB_PARAM_HR_PR));
    if (systemManager.isSupport(CONFIG_RESP))
    {
        paraList[SUB_PARAM_RR_BR] = trs(paramInfo.getSubParamName(SUB_PARAM_RR_BR));
    }
    if (systemManager.isSupport(CONFIG_SPO2))
    {
        paraList[SUB_PARAM_SPO2] = paramInfo.getSubParamName(SUB_PARAM_SPO2);
    }
    if (systemManager.isSupport(CONFIG_IBP))
    {
        SubParamID ibp1 = ibpParam.getSubParamID(IBP_INPUT_1);
        SubParamID ibp2 = ibpParam.getSubParamID(IBP_INPUT_2);
        paraList[ibp1] = paramInfo.getParamWaveformName(
                             ibpParam.getWaveformID(ibpParam.getEntitle(IBP_INPUT_1)));
        paraList[ibp2] = paramInfo.getParamWaveformName(
                             ibpParam.getWaveformID(ibpParam.getEntitle(IBP_INPUT_2)));
    }
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        paraList[SUB_PARAM_NIBP_SYS] = paramInfo.getParamName(PARAM_NIBP);
    }
    if (systemManager.isSupport(CONFIG_CO2) && co2Param.isConnected())
    {
        paraList[SUB_PARAM_ETCO2] = paramInfo.getParamName(PARAM_CO2);
    }
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        paraList[SUB_PARAM_T1] = paramInfo.getParamName(PARAM_TEMP);
    }

    if (systemManager.isSupport(CONFIG_AG))
    {
        paraList[SUB_PARAM_ETN2O] = paramInfo.getSubParamName(SUB_PARAM_ETN2O);
        paraList[SUB_PARAM_ETAA1] = paramInfo.getSubParamName(SUB_PARAM_ETAA1);
        paraList[SUB_PARAM_ETAA2] = paramInfo.getSubParamName(SUB_PARAM_ETAA2);
        paraList[SUB_PARAM_ETO2] = paramInfo.getSubParamName(SUB_PARAM_ETO2);
    }
}

void ShortTrendWindowPrivate::reloadMinitrend()
{
    // 设置短趋势
    for (int i = 0; i < comboList.count(); i++)
    {
        ComboBox *cbo = comboList.at(i);
        SubParamID paraIndex = paraList.key(cbo->currentText());
        if (paraIndex != latastParaList.at(i))
        {
            //　有改变就重设
            SubParamID id = paraList.key(cbo->currentText(), SUB_PARAM_HR_PR);
            QList<SubParamID> subParams;
            subParams = getSubParamID(id);
            if (subParams.isEmpty())
            {
                continue;
            }
            shortTrendContainer->clearTrendItemSubParam(i);
            shortTrendContainer->addSubParamToTrendItem(i, subParams);
        }
    }

    // 设置时间
    ShortTrendDuration trendDuration = static_cast<ShortTrendDuration>(lengthCbo->currentIndex());
    shortTrendContainer->setTrendDuration(trendDuration);
}

QList<SubParamID> ShortTrendWindowPrivate::getSubParamID(SubParamID id)
{
    ParamID paraID = paramInfo.getParamID(id);
    QList<SubParamID> subparams;
    switch (paraID)
    {
    case PARAM_DUP_ECG:
        subparams = ecgDupParam.getShortTrendList();
        break;
    case PARAM_IBP:
    {
        SubParamID ibp1 = ibpParam.getSubParamID(IBP_INPUT_1);
        SubParamID ibp2 = ibpParam.getSubParamID(IBP_INPUT_2);
        if (id == ibp1)
        {
            subparams = ibpParam.getShortTrendList(IBP_INPUT_1);
        }
        else if (id == ibp2)
        {
            subparams = ibpParam.getShortTrendList(IBP_INPUT_2);
        }
        break;
    }
    case PARAM_CO2:
    case PARAM_NIBP:
    case PARAM_DUP_RESP:
    case PARAM_SPO2:
    case PARAM_TEMP:
        subparams = paramInfo.getSubParams(paraID);
        break;
    case PARAM_AG:
    {
        subparams = agParam.getShortTrendList(id);
        break;
    }
    default:
        break;
    }

    return subparams;
}
