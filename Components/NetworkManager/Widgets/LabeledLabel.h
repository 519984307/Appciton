#ifndef LABELED_LABEL_H
#define LABELED_LABEL_H

#include <QWidget>

class QLabel;

/***************************************************************************************************
 * Label + Label的组合控件。
 **************************************************************************************************/
class LabeledLabel : public QWidget
{
    Q_OBJECT
public:
    LabeledLabel(const QString &label, const QString &text, QWidget *parent = NULL);
    ~LabeledLabel() {}

    // 设置文本标签内容
    void setText(const QString &text);

protected:
    void paintEvent(QPaintEvent *);

private:
    QLabel *_label;
    QLabel *_colon;
    QLabel *_text;
};

#endif // LABELEDLABEL_H
