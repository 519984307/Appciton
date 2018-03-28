#pragma once
#include "MenuGroup.h"
#include <QScopedPointer>
#include "Config.h"

class ConfigEditMenuGrpPrivate;
class ConfigEditMenuGrp : public MenuGroup
{
    Q_OBJECT
public:
    /**
     * @brief getInstance
     * @return  get an instance
     */
    static ConfigEditMenuGrp &getInstance();

    ~ConfigEditMenuGrp();

    /**
     * @brief initializeSubMenu initialize the sub menu
     */
    void initializeSubMenu();

    /**
     * @brief setCurrentEditConfig set the current edit config object
     * @param config
     */
    void setCurrentEditConfig(Config *config);

    /**
     * @brief getCurrentEditConfig get the current edit config object
     * @return  the config object
     */
    Config *getCurrentEditConfig() const;

    /**
     * @brief setCurrentEditConfigName set the current edit config name
     * @param name
     */
    void setCurrentEditConfigName(const QString &name);

    /**
     * @brief getCurrentEditConfigName get teh current edit config name
     * @return return the edit config name
     */
    QString getCurrentEditConfigName() const;


private:
    ConfigEditMenuGrp();
    QScopedPointer<ConfigEditMenuGrpPrivate> d_ptr;
};

#define configEditMenuGrp (ConfigEditMenuGrp::getInstance())
