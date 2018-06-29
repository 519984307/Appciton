#pragma once
#include "PopupWidget.h"
#include "OxyCRGWidget.h"
#include "FontManager.h"
#include "IComboList.h"
#include "ISpinBox.h"
#include <QScopedPointer>


class OxyCRGSetupWidgetPrivate;

class OxyCRGSetupWidget:public PopupWidget
{
    Q_OBJECT
public:
    OxyCRGSetupWidget();
    ~OxyCRGSetupWidget();

    QFont defaultFont(void)
    {
        return fontManager.textFont(fontManager.getFontSize(2));
    }
    /**
     * @brief setWaveTypeIndex  设置波形类型选择条目
     * @param index 波形选择条目
     */
    void setWaveTypeIndex(int index);
    /**
     * @brief getWaveTypeIndex  获得波形类型选择条目
     * @return 返回波形选择条目
     */
    int getWaveTypeIndex(void)const;

    int getHRLow(bool &status)const;
    int getHRHigh(bool &status)const;
    int getSPO2Low(bool &status)const;
    int getSPO2High(bool &status)const;
    int getCO2Low(bool &status)const;
    int getCO2High(bool &status)const;
    int getRESPLow(bool &status)const;
    int getRESPHigh(bool &status)const;
public slots:
    void onComboListUpdated(int index);
    void onSpinBoxUpdated(QString valueStr, int index);
private:
    QScopedPointer<OxyCRGSetupWidgetPrivate> d_ptr;
};
