#pragma once
#include <QScopedPointer>
#include "SubMenu.h"
#include "WiFiProfileEditor.h"

class WiFiProfileMenuPrivate;
class WiFiProfileMenu: public SubMenu
{
    Q_OBJECT
public:
    static WiFiProfileMenu &construction();
    ~WiFiProfileMenu();

    WiFiProfileInfo getCurrentWifiProfile() const;

signals:
    void selectProfile(WiFiProfileInfo profile);

protected:
    void layoutExec();
    const QScopedPointer<WiFiProfileMenuPrivate> d_ptr;
    void timerEvent(QTimerEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

private:
    WiFiProfileMenu();
    Q_DECLARE_PRIVATE(WiFiProfileMenu)
    Q_DISABLE_COPY(WiFiProfileMenu)

    Q_PRIVATE_SLOT(d_func(), void onProfileSelect(int))
    Q_PRIVATE_SLOT(d_func(), void onWifiConnected(const QString &ssid))
    Q_PRIVATE_SLOT(d_func(), void updateWifiProfileSlot(bool isEnabled))
    Q_PRIVATE_SLOT(d_func(), void updateConfigSlot(void))
};

#define wifiProfileMenu (WiFiProfileMenu::construction())
