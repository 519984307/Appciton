#include "AGTrendWidget.h"
#include "ColorManager.h"
#include "FontManager.h"
#include "ParamInfo.h"
#include <QHBoxLayout>
#include "TrendWidgetLabel.h"
#include "AGSymbol.h"
#include "WindowManager.h"
#include "AGMenu.h"
#include "PublicMenuManager.h"
#include <QDebug>

#define     INVALID     0xff

/**************************************************************************************************
 * 设置麻醉剂类型。
 *************************************************************************************************/
void AGTrendWidget::setAnestheticType(AGAnaestheticType type)
{
    if (type == AG_ANAESTHETIC_NO)
    {
        setName("Et" + (QString)AGSymbol::convert(_gasType));
        _fiName->setText("Fi" + (QString)AGSymbol::convert(_gasType));
    }
    else
    {
        setName("Et" + (QString)AGSymbol::convert(type));
        _fiName->setText("Fi" + (QString)AGSymbol::convert(type));
    }
}

/**************************************************************************************************
 * 设置et结果数据。
 *************************************************************************************************/
void AGTrendWidget::setEtData(unsigned char etValue)
{
    if (etValue == INVALID)
    {
        _etStr = InvStr();
    }
    else
    {
        if (_gasType == AG_TYPE_N2O || _gasType == AG_TYPE_O2)
        {
            _etStr = QString::number(etValue);
        }
        else
        {
            unsigned char etValueInt = etValue/10;
            unsigned char etValueDes = etValue%10;
            _etStr = QString::number(etValueInt) + "." + QString::number(etValueDes);
        }
    }

    _etValue->setText(_etStr);
}

/**************************************************************************************************
 * 设置fi结果数据。
 *************************************************************************************************/
void AGTrendWidget::setFiData(unsigned char fiValue)
{
    if (fiValue == INVALID)
    {
        _fiStr = InvStr();
    }
    else
    if (_gasType == AG_TYPE_N2O || _gasType == AG_TYPE_O2)
    {
        _fiStr = QString::number(fiValue);
    }
    else
    {
        unsigned char fiValueInt = fiValue/10;
        unsigned char fiValueDes = fiValue%10;
        _fiStr = QString::number(fiValueInt) + "." + QString::number(fiValueDes);
    }

    _fiValue->setText(_fiStr);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AGTrendWidget::AGTrendWidget(const QString &trendName, const AGTypeGas gasType)
    : TrendWidget(trendName), _gasType(gasType)
{
    _etStr = InvStr();
    _fiStr = InvStr();

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_AG));
    setPalette(palette);

    setName("Et" + (QString)AGSymbol::convert(gasType));
    setUnit("%");

    // 构造资源。
    _etValue = new QLabel();
    _etValue->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    _etValue->setPalette(palette);
    _etValue->setText(InvStr());

    _fiName = new QLabel();
    _fiName->setPalette(palette);
    _fiName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _fiName->setText("Fi" + (QString)AGSymbol::convert(gasType));

    _fiValue = new QLabel();
    _fiValue->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _fiValue->setPalette(palette);
    _fiValue->setText(InvStr());

    QHBoxLayout *fiLayout = new QHBoxLayout();
    fiLayout->addStretch();
    fiLayout->addWidget(_fiName);
    fiLayout->addStretch();
    fiLayout->addWidget(_fiValue);
    fiLayout->addStretch();

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addStretch();
    vLayout->addWidget(_etValue);
    vLayout->addStretch();
    vLayout->addLayout(fiLayout);
    vLayout->addStretch();

    contentLayout->addLayout(vLayout);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AGTrendWidget::~AGTrendWidget()
{

}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void AGTrendWidget::setTextSize()
{
    QRect r;
    int h = ((height()-nameLabel->height()) / 3);
    int w = (width() - unitLabel->width());
    r.setSize(QSize(w, (h * 2)));

    int fontsize = fontManager.adjustNumFontSize(r,true,"2222");
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);

    _etValue->setFont(font);

    r.setSize(QSize(w, h));
    fontsize = fontManager.adjustNumFontSize(r,true,"2222");
    font = fontManager.numFont(fontsize - 5);
    font.setWeight(QFont::Black);
    _fiName->setFont(font);

    font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);
    _fiValue->setFont(font);

}


/**************************************************************************************************
 * 趋势槽函数。
 *************************************************************************************************/
void AGTrendWidget::_releaseHandle(IWidget *)
{
    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - publicMenuManager.width()) / 2;
    int y = r.y() + (r.height() - publicMenuManager.height());
    publicMenuManager.popup(&agMenu, x, y);
}
