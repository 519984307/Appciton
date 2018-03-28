#pragma once
#include <QPushButton>
#include <QString>

class Key: public QPushButton
{
    Q_OBJECT

public:
    Key(QWidget *parent = NULL);
    ~Key();

    void setId(int id)
    {
        _id = id;
    }

    int id(void)
    {
        return _id;
    }

signals:
    void KeyClicked(int id);
    void KeyClicked(const QString &text);

protected:
    void paintEvent(QPaintEvent *e);

private slots:
    void _onClicked();

private:
    int _id;
};
