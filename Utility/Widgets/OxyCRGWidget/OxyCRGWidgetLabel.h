////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件中的文本标签
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <QString>
#include "OxyCRGWidget.h"
#include "IWidget.h"

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件中的文本标签
////////////////////////////////////////////////////////////////////////////////
class OxyCRGWidgetLabel: public IWidget
{
    Q_OBJECT

public:
    OxyCRGWidgetLabel(const QString &text, int flags = 0, OxyCRGWidget *wave = NULL);
    ~OxyCRGWidgetLabel();

    void setText(const QString &text);
    inline const QString &text() const
    {
        return _text;
    }

    inline const QString &waveName() const {return _waveName;}

    void setFlags(int flags);
    inline int flags() const
    {
        return _flags;
    }

    void resizeToFitText();

protected:
    void paintEvent(QPaintEvent *e);

private:
    QString _waveName;        // 波形控件名称
    QString _text;            // 标签显示的文本
    int _flags;               // 文本对齐标志
    bool _isFixedWidth;       // 是否为固定宽度
};
