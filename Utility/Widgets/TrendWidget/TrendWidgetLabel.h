#pragma once
#include <QString>
#include "TrendWidget.h"
#include "IWidget.h"

class QLabel;
class TrendWidgetLabel: public IWidget
{
    Q_OBJECT

public:
    TrendWidgetLabel(const QString &text, int flags = 0, TrendWidget *wave = NULL);
    ~TrendWidgetLabel();

    void setText(const QString &text);

    void setFlags(int flags);
    inline int flags() const
    {
        return _flags;
    }

    void setFont(const QFont font);

private:
    QLabel *nameLabel;
    int _flags;               // 文本对齐标志
    bool _isFixedWidth;       // 是否为固定宽度
};
