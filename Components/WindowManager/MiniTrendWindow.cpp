/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/10/9
 **/

#include "MiniTrendWindow.h"
#include <QGroupBox>
#include <QLayout>
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include "Button.h"
#include "ShortTrendContainer.h"
#include <QStringList>
#include "ParamInfo.h"
#include "IBPParam.h"

#define PARAM_ROW 2      // 每行ｎ个控件

class  MiniTrendWindowPrivate
{
public:
    explicit MiniTrendWindowPrivate(ShortTrendContainer *const trendContainer)
        : lengthCbo(NULL),
          minitrendContainer(trendContainer),
          defaultBtn(NULL),
          okBtn(NULL),
          cancelBtn(NULL)
    {
        minitrendContainer->getShortTrendList(defaultParaList);

        durationList[SHORT_TREND_DURATION_30M] = "30 min";
        durationList[SHORT_TREND_DURATION_60M] = "1 h";
        durationList[SHORT_TREND_DURATION_120M] = "2 h";
        durationList[SHORT_TREND_DURATION_240M] = "4 h";
        durationList[SHORT_TREND_DURATION_480M] = "8 h";
    }
    ~MiniTrendWindowPrivate() {}
    QList<ComboBox *> comboList;
    ComboBox *lengthCbo;
    ShortTrendContainer *minitrendContainer;
    QMap<SubParamID, QString> paraList;
    QStringList defaultParaList;
    Button *defaultBtn;
    Button *okBtn;
    Button *cancelBtn;
    QMap<ShortTrendDuration, QString> durationList;     // 翻译时间

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
    void setMinitrend();
};


MiniTrendWindow::MiniTrendWindow(ShortTrendContainer *const trendContainer)
    : Window(),
      d_ptr(new MiniTrendWindowPrivate(trendContainer))
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

    int paramCount = d_ptr->minitrendContainer->getTrendNum();
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
    d_ptr->defaultBtn->setText(trs("Defaults"));
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

MiniTrendWindow::~MiniTrendWindow()
{
    delete d_ptr;
}

void MiniTrendWindow::showEvent(QShowEvent *e)
{
    d_ptr->loadOption();
    Window::showEvent(e);
}

void MiniTrendWindow::onBtnReleased()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == d_ptr->defaultBtn)
    {
        for (int i = 0; i < d_ptr->comboList.count(); i++)
        {
            ComboBox *cbo = d_ptr->comboList.at(i);
            QString para = d_ptr->defaultParaList.at(i);
            for (int j = 0; j < cbo->count(); j++)
            {
                if (cbo->itemText(j) == para)
                {
                    cbo->setCurrentIndex(j);
                    break;
                }
            }
        }
    }
    else if (btn == d_ptr->okBtn)
    {
        d_ptr->setMinitrend();
        close();
    }
    else if (btn == d_ptr->cancelBtn)
    {
        close();
    }
}

void MiniTrendWindowPrivate::loadOption()
{
    QStringList latestItemList;
    minitrendContainer->getShortTrendList(latestItemList);
    // 加载参数
    for (int i = 0; i < comboList.count(); i++)
    {
        ComboBox *cbo = comboList.at(i);
        // 加载列表选项
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
            if (latestItemList.at(i) == cbo->itemText(j))
            {
                cbo->setCurrentIndex(j);
            }
        }
    }
    // 加载时间
    for (int i = SHORT_TREND_DURATION_30M; i <= SHORT_TREND_DURATION_480M; i++)
    {
        ShortTrendDuration index = static_cast<ShortTrendDuration>(i);
        lengthCbo->addItem(durationList[index]);
    }
    int index = static_cast<int>(minitrendContainer->getTrendDuration());
    lengthCbo->setCurrentIndex(index);
}


void MiniTrendWindowPrivate::loadParaList()
{
    paraList[SUB_PARAM_HR_PR] = trs(paramInfo.getSubParamName(SUB_PARAM_HR_PR));
    paraList[SUB_PARAM_RR_BR] = trs(paramInfo.getSubParamName(SUB_PARAM_RR_BR));
    paraList[SUB_PARAM_SPO2] = paramInfo.getSubParamName(SUB_PARAM_SPO2);
    paraList[SUB_PARAM_ART_SYS] = paramInfo.getIBPPressName(SUB_PARAM_ART_SYS);
    paraList[SUB_PARAM_PA_SYS] = paramInfo.getIBPPressName(SUB_PARAM_PA_SYS);
    paraList[SUB_PARAM_ETCO2] = paramInfo.getParamName(PARAM_CO2);
    paraList[SUB_PARAM_T1] = paramInfo.getParamName(PARAM_TEMP);
    paraList[SUB_PARAM_ETN2O] = paramInfo.getParamName(PARAM_AG);
}

void MiniTrendWindowPrivate::setMinitrend()
{
    // 设置短趋势
    for (int i = 0; i < comboList.count(); i++)
    {
        ComboBox *cbo = comboList.at(i);
        SubParamID id = paraList.key(cbo->currentText(), SUB_PARAM_HR_PR);
        QList<SubParamID> subParams;
        minitrendContainer->getSubParamList(id, subParams);
        if (subParams.isEmpty())
        {
            subParams.append(id);
        }
        if (cbo->currentText() != defaultParaList.at(i))
        {
            //　有改变就重设
            minitrendContainer->clearTrendItemSubParam(i);
            minitrendContainer->addSubParamToTrendItem(i, subParams);
        }
    }

    // 设置时间
    QString lengthStr = lengthCbo->currentText();
    ShortTrendDuration trendDuration =
        durationList.key(lengthStr, minitrendContainer->getTrendDuration());
    minitrendContainer->setTrendDuration(trendDuration);
}
