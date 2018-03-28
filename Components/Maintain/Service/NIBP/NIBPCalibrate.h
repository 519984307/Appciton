#pragma once
#include "SubMenu.h"
#include "NIBPSymbol.h"
#include <QHBoxLayout>
#include "IComboList.h"
#include <QList>
#include "LabelButton.h"
#include "ISpinBox.h"

class Calibrate_SetItem : public QWidget
{
public:
    Calibrate_SetItem() : QWidget()
    {
        title = new QLabel();
        value = new QLabel();
        range = new ISpinBox(trs("ServicePressureRange"),false);
        btn = new LButtonEx();
//        label = new QLabel();

        QHBoxLayout *manlayout = new QHBoxLayout();
        manlayout->setSpacing(2);
        manlayout->setMargin(0);

        manlayout->addWidget(title);             //标题
        manlayout->addWidget(value);             //设置的压力值
        manlayout->addWidget(range);             //校准点的值
        manlayout->addWidget(btn);               //校准按钮
//        manlayout->addWidget(label);             //校准完成标志

        setLayout(manlayout);

        setFocusPolicy(Qt::StrongFocus);
        setFocusProxy(btn);
    }

public:
    QLabel *title;
    QLabel *value;
    ISpinBox *range;
    LButtonEx *btn;
//    QLabel *label;
};

class QLabel;
class NIBPCalibrate : public SubMenu
{
    Q_OBJECT

public:
    static NIBPCalibrate &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new NIBPCalibrate();
        }

        return *_selfObj;
    }
    static NIBPCalibrate *_selfObj;

    //进入校准模式的应答
    void unPacket(bool flag);

    void setText(QString str);

    //校准点应答
    void unPacketPressure(bool flag);

    //初始化
    void init(void);

    ~NIBPCalibrate();

protected:
    virtual void layoutExec(void);
    virtual void focusFirstItem(void);

private slots:
    void _calibratedBtn1(void);              //校准点1按钮
    void _calibratedBtn2(void);              //校准点2按钮

private:
    NIBPCalibrate();

    QList<Calibrate_SetItem*> _itemList;
    Calibrate_SetItem *_item;
    QLabel *_label;

    bool _btnFlag1;                          //用于判定是第一个校准点的返回值
    bool _btnFlag2;                          //用于判定是第二个校准点的返回值
    int _pressurevalue;                       //校准点的值

    bool _calibrateFlag;                     //进入模式标志
};


#define nibpcalibrate (NIBPCalibrate::construction())
