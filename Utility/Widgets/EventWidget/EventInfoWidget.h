#pragma once
#include <QWidget>

class EventInfoWidget : public QWidget
{
    Q_OBJECT
public:
    EventInfoWidget(QWidget *parent = NULL);
    ~EventInfoWidget();

    void loadDataInfo(QString info, QString time, QString index);

protected:
    void paintEvent(QPaintEvent *e);

private:
    QString eventInfo;
    QString eventTime;
    QString eventIndex;
};
