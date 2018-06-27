#pragma once
#include <QWidget>
#include <QScrollArea>

class MenuSidebarPrivate;
class MenuSidebar : public QScrollArea
{
    Q_OBJECT
public:
    MenuSidebar(QWidget *parent = NULL);
    ~MenuSidebar();

    /* add a string item */
    void addItem(const QString &text);

    /* reimplement */
    QSize sizeHint() const;


signals:
    void selectItemChanged(const QString &text);

protected:
    void resizeEvent(QResizeEvent *ev);
    void showEvent(QShowEvent *ev);
    void scrollContentsBy(int dx, int dy);

private slots:
    void onItemClicked();

private:
    QScopedPointer<MenuSidebarPrivate> d_ptr;
};

