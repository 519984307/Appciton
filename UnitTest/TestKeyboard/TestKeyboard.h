#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include "NumberPanel.h"
#include "Debug.h"

class Face : public QWidget
{
    Q_OBJECT

public:
    Face() : QWidget()
    {
        QPushButton *btn = new QPushButton("Keybord");
        btn->setFixedHeight(50);
        connect(btn, SIGNAL(released()), this, SLOT(popup()));

        QHBoxLayout *layout = new QHBoxLayout();
        layout->addWidget(btn);

        setLayout(layout);
        setFixedSize(480, 320);

        connect(&numberPanel, SIGNAL(enter(const QString&)), this, SLOT(entered(const QString&)));
    }

    ~Face()
    {
        deleteNumberPanel();
    }

private slots:
    void entered(const QString &s)
    {
        debug("%s\n", qPrintable(s));
    }

    void popup()
    {
        if (numberPanel.isVisible())
        {
            numberPanel.hide();
        }
        else
        {
            numberPanel.setInitString("0123456789*.");
            numberPanel.show();
        }
    }
};
