#include <QHBoxLayout>
#include <QVBoxLayout>
#include "ECGSTTrendWidget.h"
#include "ParamManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "Debug.h"
#include "ECGMenu.h"
#include "WindowManager.h"
#include "PublicMenuManager.h"
#include "ECGSymbol.h"
#include "TrendWidgetLabel.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void ECGSTTrendWidget::_releaseHandle(IWidget *)
{
    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - publicMenuManager.width()) / 2;
    int y = r.y() + (r.height() - publicMenuManager.height());
    publicMenuManager.popup(&ecgMenu, x, y);
}

/**************************************************************************************************
 * 设置HR的值。
 *************************************************************************************************/
void ECGSTTrendWidget::setSTValue(ECGST lead, short st)
{
    if (st == InvData())
    {
        _stString[lead] = InvStr();
    }
    else
    {
        _stString[lead].sprintf("%.1f", st / 10.0);
    }
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void ECGSTTrendWidget::isAlarm(int id, bool flag)
{
    _isAlarm[id] = flag;

    updateAlarm(_isAlarm[ECG_ST_I] || _isAlarm[ECG_ST_II] || _isAlarm[ECG_ST_III] ||
                _isAlarm[ECG_ST_aVR] || _isAlarm[ECG_ST_aVL] || _isAlarm[ECG_ST_aVF] ||
                _isAlarm[ECG_ST_V1] || _isAlarm[ECG_ST_V2] || _isAlarm[ECG_ST_V3] ||
                _isAlarm[ECG_ST_V4] || _isAlarm[ECG_ST_V5] || _isAlarm[ECG_ST_V6]);
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void ECGSTTrendWidget::showValue(void)
{
    QPalette p = palette();
    if (_isAlarm[ECG_ST_I] || _isAlarm[ECG_ST_II] || _isAlarm[ECG_ST_III] ||
            _isAlarm[ECG_ST_aVR] || _isAlarm[ECG_ST_aVL] || _isAlarm[ECG_ST_aVF] ||
            _isAlarm[ECG_ST_V1] || _isAlarm[ECG_ST_V2] || _isAlarm[ECG_ST_V3] ||
            _isAlarm[ECG_ST_V4] || _isAlarm[ECG_ST_V5] || _isAlarm[ECG_ST_V6])
    {
//        if (p.window().color() != Qt::white)
//        {
//            p.setColor(QPalette::Window, Qt::white);
//            p.setColor(QPalette::WindowText, Qt::red);
//            setPalette(p);
//        }

        for (int i = ECG_ST_I;i < ECG_ST_NR;i++)
        {
            p = _stLabel[i]->palette();
            if (_isAlarm[i])
            {
                if (p.windowText().color() != Qt::red)
                {
                    p.setColor(QPalette::WindowText, Qt::red);
                    _stLabel[i]->setPalette(p);
                    _stValue[i]->setPalette(p);
                }
            }
            else
            {
                if (p.window().color() != Qt::black)
                {
                    p = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
                    setPalette(p);
                    _stLabel[i]->setPalette(p);
                    _stValue[i]->setPalette(p);
                }
            }
        }
    }
    else
    {
        for (int i = ECG_ST_I;i < ECG_ST_NR;i++)
        {
            if (p.window().color() != Qt::black)
            {
                p = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
                setPalette(p);
                _stLabel[i]->setPalette(p);
                _stValue[i]->setPalette(p);
            }
        }
    }
    for (int i = ECG_ST_I;i < ECG_ST_NR;i++)
    {
        _stValue[i]->setText(_stString[i]);
    }
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void ECGSTTrendWidget::setTextSize(void)
{
//    QRect r = this->rect();
//    r.adjust(nameLabel->width(), 0, 0, 0);
    QRect r;
    r.setSize(QSize(((width()-nameLabel->width())/6),(height()/4)));
    // 字体。
//    int fontsize = fontManager.adjustNumFontSizeXML(r);
//    fontsize = fontManager.getFontSize(fontsize);
    int fontsize = fontManager.adjustNumFontSize(r,true,"2222");
    QFont font = fontManager.numFont(fontsize, true);
//    font.setStretch(105); // 横向放大。
    font.setWeight(QFont::Black);

    QFont font1 = fontManager.numFont(fontsize);
    font1.setWeight(QFont::Black);

    for (int i = ECG_ST_I;i<ECG_ST_NR;i++)
    {
        _stLabel[i]->setFont(font1);

        _stValue[i]->setFont(font);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGSTTrendWidget::ECGSTTrendWidget() : TrendWidget("ECGSTTrendWidget")
{
    // 设置标题栏的相关信息。
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    setPalette(palette);
    setName(trs("ST"));
    setUnit(Unit::getSymbol(UNIT_MV));



    QHBoxLayout *hlayout[ECG_ST_NR];
    for (int i = ECG_ST_I;i<ECG_ST_NR;i++)
    {
        _stLabel[i] = new QLabel();
        _stLabel[i]->setPalette(palette);
        _stLabel[i]->setAlignment(Qt::AlignCenter);
        _stLabel[i]->setText(trs(ECGSymbol::convert((ECGST)i)));

        _stValue[i] = new QLabel();
        _stValue[i]->setPalette(palette);
        _stValue[i]->setAlignment(Qt::AlignCenter);
        _stValue[i]->setMinimumWidth(30);
        _stValue[i]->setText(InvStr());

        hlayout[i] = new QHBoxLayout();
        hlayout[i]->setMargin(1);
        hlayout[i]->setSpacing(1);
        hlayout[i]->addWidget(_stLabel[i]);
        hlayout[i]->addSpacing(2);
        hlayout[i]->addWidget(_stValue[i]);

        _isAlarm[i] = false;
        _stString[i] = InvStr();
    }

    QVBoxLayout *vlayout0 = new QVBoxLayout();
    vlayout0->setMargin(0);
    vlayout0->setSpacing(0);
    vlayout0->addLayout(hlayout[ECG_ST_I]);
    vlayout0->addLayout(hlayout[ECG_ST_II]);
    vlayout0->addLayout(hlayout[ECG_ST_III]);
    vlayout0->addLayout(hlayout[ECG_ST_aVR]);


    QVBoxLayout *vlayout1 = new QVBoxLayout();
    vlayout1->setMargin(1);
    vlayout1->setSpacing(1);
    vlayout1->addLayout(hlayout[ECG_ST_aVL]);
    vlayout1->addLayout(hlayout[ECG_ST_aVF]);
    vlayout1->addLayout(hlayout[ECG_ST_V1]);
    vlayout1->addLayout(hlayout[ECG_ST_V2]);

    QVBoxLayout *vlayout2 = new QVBoxLayout();
    vlayout2->setMargin(1);
    vlayout2->setSpacing(1);
    vlayout2->addLayout(hlayout[ECG_ST_V3]);
    vlayout2->addLayout(hlayout[ECG_ST_V4]);
    vlayout2->addLayout(hlayout[ECG_ST_V5]);
    vlayout2->addLayout(hlayout[ECG_ST_V6]);

    QHBoxLayout *mainlayout = new QHBoxLayout();
    mainlayout->setMargin(1);
    mainlayout->setSpacing(1);
    mainlayout->addLayout(vlayout0,1);
    mainlayout->addLayout(vlayout1,1);
    mainlayout->addLayout(vlayout2,1);

//    contentLayout->addStretch(1);
    contentLayout->addLayout(mainlayout);
//    contentLayout->addStretch(1);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGSTTrendWidget::~ECGSTTrendWidget()
{

}
