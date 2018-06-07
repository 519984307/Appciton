#pragma once
#include "SubMenu.h"
#include <QLabel>

class SoftWareVersion : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction  初始化方法
     * @return
     */
    static SoftWareVersion &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new SoftWareVersion();
        }
        return *_selfObj;
    }
    static SoftWareVersion *_selfObj;
    ~SoftWareVersion();
protected:
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec(void);
    /**
     * @brief readyShow  加载数据方法
     */
    virtual void readyShow(void);
private slots:

private:
    SoftWareVersion();

    enum SoftWareVersionType
    {
        SOFT_TYPE_SYSTEM_SOFTWARE_VERSION=0,
        SOFT_TYPE_POWER_MANAGER_SOFTWARE_VERSION,
        SOFT_TYPE_UBOOT,
        SOFT_TYPE_KERNEL,
        SOFT_TYPE_KEYBOARD_MODULE,
        SOFT_TYPE_RECORDER_MODULE,
        SOFT_TYPE_ECG_ALGORITHM_TYPE,
        SOFT_TYPE_BOOT_STANDBY_LOGO_VERSION,
        SOFT_TYPE_NR
    };
    typedef struct{
        QLabel  *typeName;
        QLabel  *typeInfo;
    }LABEL_TYPE;

    LABEL_TYPE  _labelType[SOFT_TYPE_NR];
    QString     _stringTypeName[SOFT_TYPE_NR];
};
#define softWareVersion (SoftWareVersion::construction())
