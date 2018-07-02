#pragma once
#include <QAbstractButton>
#include <QScopedPointer>

class ButtonPrivate;
class Button : public QAbstractButton
{
    Q_OBJECT
public:

    enum ButtonStyle {
        ButtonTextOnly,
        ButtonIconOnly,
        ButtonTextBesideIcon,
        ButtonTextUnderIcon,
    };

    /**
     * @brief Button constructor
     * @param text  button text
     * @param icon button icon
     * @param parent
     */
    Button(const QString &text=QString(), const QIcon &icon=QIcon(), QWidget *parent = NULL);

    ~Button();

    /**
     * @brief setBorderWidth
     */
    void setBorderWidth(int width);

    /**
     * @brief borderWidth
     * @return
     */
    int borderWidth() const;

    /**
     * @brief setBorderRadius set the button border radius
     * @param radius
     */
    void setBorderRadius(int radius);

    /**
     * @brief borderRadius get teh border radius
     * @return  the radius value
     */
    int borderRadius() const;

    /**
     * @brief setButtonStyle set the button style
     * @param style
     */
    void setButtonStyle(ButtonStyle style);

    /**
     * @brief buttonStyle get the button style
     * @return
     */
    ButtonStyle buttonStyle() const;

    /* reimplement */
    QSize sizeHint() const;

protected:
    /* reimplement */
    void paintEvent(QPaintEvent *ev);

private:
    QScopedPointer<ButtonPrivate> d_ptr;
};
