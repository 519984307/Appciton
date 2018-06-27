#include "Window.h"
#include <QEvent>
#include <QLabel>
#include "Button.h"
#include <QBoxLayout>
#include <QFrame>
#include <QDebug>
#include "FontManager.h"

#define TITLE_BAR_HEIGHT 40

class WindowPrivate
{
public:
    WindowPrivate()
        :m_widget(NULL){}

    QWidget *m_widget;
    QLabel *m_titleLbl;
};

Window::Window(QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint),
      d_ptr(new WindowPrivate)
{
    QLabel *titleLbl = new QLabel();
    titleLbl->setFixedHeight(TITLE_BAR_HEIGHT);
    titleLbl->setText(this->windowTitle());
    titleLbl->setAlignment(Qt::AlignCenter);
    d_ptr->m_titleLbl = titleLbl;
    Button *closeBtn = new Button();
    closeBtn->setIconSize(QSize(24,24));
    closeBtn->setBorderWidth(0);
    closeBtn->setFixedSize(TITLE_BAR_HEIGHT, TITLE_BAR_HEIGHT);
    closeBtn->setIcon(QIcon(":/ui/close.svg"));
    closeBtn->setButtonStyle(Button::ButtonIconOnly);
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0,0,0,0);
    hlayout->addWidget(titleLbl, 1);
    hlayout->addWidget(closeBtn);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0,0,0,0);
    vLayout->setSpacing(0);
    vLayout->addLayout(hlayout);
    QFrame *line = new QFrame;
    line->setFrameStyle(QFrame::HLine|QFrame::Sunken);
    line->setLineWidth(0);
    line->setMidLineWidth(1);
    vLayout->addWidget(line);


    d_ptr->m_widget = new QWidget();
    vLayout->addWidget(d_ptr->m_widget, 1);
    connect(closeBtn, SIGNAL(clicked(bool)), this, SLOT(close()));

    setFont(fontManager.textFont(17));
}

Window::~Window()
{

}

QLayout *Window::getWindowLayout()
{
    return d_ptr->m_widget->layout();
}

void Window::setWindowLayout(QLayout *layout)
{
    d_ptr->m_widget->setLayout(layout);
}

void Window::changeEvent(QEvent *ev)
{
    QDialog::changeEvent(ev);
    if(ev->type() == QEvent::WindowTitleChange)
    {
        d_ptr->m_titleLbl->setText(windowTitle());
    }
}
