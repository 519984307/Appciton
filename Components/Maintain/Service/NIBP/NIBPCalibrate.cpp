#include <QVBoxLayout>
#include <QLabel>
#include "NIBPCalibrate.h"
#include "Debug.h"
#include "NIBPParam.h"
#include "NIBPRepairMenuManager.h"

NIBPCalibrate *NIBPCalibrate::_selfObj = NULL;

//校准模式
/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPCalibrate::NIBPCalibrate() : SubMenu(trs("ServiceCalibrate"))
{
    setDesc(trs("NIBPCalibrate"));

    _itemList.clear();
    _itemList.append(new Calibrate_SetItem());
    _itemList.append(new Calibrate_SetItem());

    startLayout();
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void NIBPCalibrate::layoutExec()
{
    int submenuW = nibpRepairMenuManager.getSubmenuWidth();
    int submenuH = nibpRepairMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 3;

    _item = _itemList.at(0);
    _item->title->setText(trs("CalibratePoint1"));
    _item->title->setFont(fontManager.textFont(fontSize));
    _item->title->setAlignment(Qt::AlignCenter);
    _item->title->setFixedSize(btnWidth, ITEM_H);

    _item->value->setText(" 0 ");
    _item->value->setFont(fontManager.textFont(fontSize));
    _item->value->setAlignment(Qt::AlignCenter);
    _item->value->setFixedSize(btnWidth, ITEM_H);

    _item->range->setVisible(false);

    _item->btn->setText(trs("ServiceCalibrate"));
    _item->btn->setFont(fontManager.textFont(fontSize));
    _item->btn->setFixedSize(btnWidth, ITEM_H);
    connect(_item->btn, SIGNAL(realReleased()), this, SLOT(_calibratedBtn1()));

//    QPixmap pix;
//    QImage image("/usr/local/nPM/icons/select.png");
//    image = image.scaled(25,25);
//    pix.convertFromImage(image);
//    item->label->setPixmap(pix);

    mainLayout->addWidget(_item);

    _item = _itemList.at(1);
    _item->title->setText(trs("CalibratePoint2"));
    _item->title->setFont(fontManager.textFont(fontSize));
    _item->title->setAlignment(Qt::AlignCenter);
    _item->title->setFixedSize(btnWidth, ITEM_H);

    _item->value->setVisible(false);

    _item->range->setFixedSize(btnWidth, ITEM_H);
    _item->range->setSuffix(Unit::getSymbol(UNIT_MMHG));
    _item->range->setSuffixSpace(1);
    _item->range->setFont(fontManager.textFont(fontSize));
    _item->range->setMode(ISPIN_MODE_INT);
    _item->range->setRange(200, 280);
    _item->range->setStep(1);
    _item->range->setValue(250);
    _item->range->setContentsMargins(0, 0, 0, 0);

    _item->btn->setText(trs("ServiceCalibrate"));
    _item->btn->setFont(fontManager.textFont(fontSize));
    _item->btn->setFixedSize(btnWidth, ITEM_H);
    connect(_item->btn, SIGNAL(realReleased()), this, SLOT(_calibratedBtn2()));

//    item->label->setPixmap(pix);

    _label = new QLabel();
    _label->setFixedSize(itemW, ITEM_H);
    _label->setAlignment(Qt::AlignCenter);
    _label->setFont(fontManager.textFont(fontSize));
    _label->setText("");

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setMargin(5);
    hlayout->setSpacing(5);
    hlayout->addWidget(_label,0 , Qt::AlignCenter);

    mainLayout->addWidget(_item);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(hlayout);
}

/**************************************************************************************************
 * 初始化。
 *************************************************************************************************/
void NIBPCalibrate::init()
{
    _label->setText("");

    _btnFlag1 = false;
    _btnFlag2 = false;
    _item = _itemList.at(0);
    _item->btn->setText(trs("ServiceCalibrate"));
    _item->btn->setEnabled(true);

    _item = _itemList.at(1);
    _item->range->setValue(250);
    _item->range->disable(false);
    _item->range->setEnabled(true);
    _item->btn->setText(trs("ServiceCalibrate"));
    _item->btn->setEnabled(true);

    _calibrateFlag = false;
}

void NIBPCalibrate::focusFirstItem()
{
    init();

    nibpParam.switchState(NIBP_SERVICE_CALIBRATE_STATE);

    SubMenu::focusFirstItem();
}

/**************************************************************************************************
 * 校准按钮1信号槽
 *************************************************************************************************/
void NIBPCalibrate::_calibratedBtn1()
{
    _label->setText("");
    if (_calibrateFlag)
    {
        if (!_btnFlag1)
        {
            _btnFlag1 = true;
            unsigned char cmd[3];
            cmd[0] = 0x00;
            cmd[1] = 0x00;
            cmd[2] = 0x00;

            nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_CMD_PRESSURE_POINT,cmd,3);

            _item = _itemList.at(0);
            _item->btn->setText(trs("ServiceCalibrating"));

            _item = _itemList.at(1);
            _item->range->disable(true);
            _item->range->setEnabled(false);
            _item->btn->setEnabled(false);
        }
    }
    else
    {
        nibpRepairMenuManager.messageBox();
    }
}

/**************************************************************************************************
 * 校准按钮2信号槽
 *************************************************************************************************/
void NIBPCalibrate::_calibratedBtn2()
{
    _label->setText("");
    if (_calibrateFlag)
    {
        if (!_btnFlag2)
        {
            _btnFlag2 = true;
            _item = _itemList.at(1);
            QString value = _item->range->getText();
            _pressurevalue = value.toInt();

            unsigned char cmd[3];
            cmd[0] = 0x01;
            cmd[1] = _pressurevalue & 0xFF;
            cmd[2] = (_pressurevalue & 0xFF00) >> 8;
            nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_CMD_PRESSURE_POINT,cmd,3);

            _item = _itemList.at(1);
            _item->btn->setText(trs("ServiceCalibrating"));
            _item->range->disable(true);
            _item->range->setEnabled(false);
            _item->btn->setEnabled(false);
        }
    }
    else
    {
        nibpRepairMenuManager.messageBox();
    }
}

/**************************************************************************************************
 * 进入模式的应答。
 *************************************************************************************************/
void NIBPCalibrate::unPacket(bool flag)
{
    _calibrateFlag = flag;
    if( flag )
    {
        _item = _itemList.at(0);
        _item->btn->setEnabled(true);

        _item = _itemList.at(1);
        _item->range->disable(true);
        _item->range->setEnabled(false);
        _item->btn->setEnabled(false);
    }
    else
    {
        _item = _itemList.at(0);
        _item->btn->setEnabled(false);

        _item = _itemList.at(1);
        _item->range->disable(true);
        _item->range->setEnabled(false);
        _item->btn->setEnabled(false);
    }
}

void NIBPCalibrate::setText(QString str)
{
    _label->setText(str);
}

/**************************************************************************************************
 * 校准点应答。
 *************************************************************************************************/
void NIBPCalibrate::unPacketPressure(bool flag)
{
    if (_btnFlag1)
    {
        _btnFlag1 = false;
        if( flag )
        {
            _item = _itemList.at(0);
            _item->btn->setText(trs("ServiceSuccess"));

            _item = _itemList.at(1);
            _item->range->setValue(250);
            _item->range->disable(false);
            _item->range->setEnabled(true);
            _item->btn->setEnabled(true);
            _label->setText(trs("ServiceCalibrateNextOne"));
        }
        else
        {
            _item = _itemList.at(0);
            _item->btn->setText(trs("ServiceAgain"));

            _item = _itemList.at(1);
            _item->range->disable(true);
            _item->range->setEnabled(false);
            _item->btn->setEnabled(false);
            _label->setText(trs("CalibrateFail") + "," + trs("ServiceCalibrateAgain"));
        }
    }
    if (_btnFlag2)
    {
        _btnFlag2 = false;
        if( flag )
        {
            _item = _itemList.at(0);
            _item->btn->setText(trs("ServiceCalibrate"));
            _item->btn->setEnabled(true);

            _item = _itemList.at(1);
            _item->range->disable(true);
            _item->range->setEnabled(false);
            _item->btn->setText(trs("ServiceCalibrate"));
            _item->btn->setEnabled(false);
            _label->setText(trs("CalibrateSuccess"));
        }
        else
        {
            _item = _itemList.at(0);
            _item->btn->setText(trs("ServiceCalibrate"));
            _item->btn->setEnabled(true);

            _item = _itemList.at(1);
            _item->range->setValue(250);
            _item->range->disable(true);
            _item->range->setEnabled(false);
            _item->btn->setText(trs("ServiceCalibrate"));
            _item->btn->setEnabled(false);
            _label->setText(trs("CalibrateFail") + "," + trs("ServiceCalibrateAgain"));
        }
    }
    _btnFlag1 = false;
    _btnFlag2 = false;
}

NIBPCalibrate::~NIBPCalibrate()
{

}

