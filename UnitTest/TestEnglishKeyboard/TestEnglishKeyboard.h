#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include "EnglishPanel.h"
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

        connect(&englishPanel, SIGNAL(enter(const QString&)), this, SLOT(entered(const QString&)));
    }

    ~Face()
    {
        deleteEnglishPanel();
    }

private slots:
    void entered(const QString &s)
    {
        debug("%s\n", qPrintable(s));
    }

    void popup()
    {
        if (englishPanel.isVisible())
        {
            englishPanel.hide();
        }
        else
        {
            englishPanel.setInitString("this is a test string!");
            englishPanel.show();
        }
    }
};
